#include <cstdlib>

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Engine/World.h"
#include "Helpers/MarketplaceRequestsTestData.h"
#include "SequencePlugin/Public/Sequence/SequencePay.h"
#include "Tests/AutomationCommon.h"
#include "Tests/AutomationEditorCommon.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FGetSupportedSwapTokensTest, "SequencePlugin.EndToEnd.MarketplaceTests.GetSupportedSwapTokensTest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter | EAutomationTestFlags::ClientContext)

DEFINE_LATENT_AUTOMATION_COMMAND_TWO_PARAMETER(FGetSupportedSwapTokensTestIsDone, const UMarketplaceRequestsTestData*, IndexerRequestsTestData, FAutomationTestBase*, FGetSupportedSwapTokensTest);

bool FGetSupportedSwapTokensTestIsDone::Update()
{
    while(this->IndexerRequestsTestData->GetPendingRequests() > 0)
    {
        return false;
    }

    if (this->IndexerRequestsTestData->GetAllRequestsSuccessful())
    {
        FGetSupportedSwapTokensTest->AddInfo(TEXT("Get Supported Swap Tokens: Request Completed"));
    }
    else
    {
        FGetSupportedSwapTokensTest->AddError(FString::Printf(TEXT("Get Supported Swap Tokens: Request Failed")));
    }
    
    return true;
}

void FGetSupportedSwapTokensTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
    OutBeautifiedNames.Add(TEXT("Get Supported Swap Tokens Test"));
    OutTestCommands.Add(TEXT(""));
}

bool FGetSupportedSwapTokensTest::RunTest(const FString& Parameters)
{
    AddInfo(TEXT("Testing Get Supported Swap Tokens"));
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(true));
    ADD_LATENT_AUTOMATION_COMMAND(FEngineWaitLatentCommand(1.0f));
    ADD_LATENT_AUTOMATION_COMMAND(FFunctionLatentCommand([this]()
    {
        UMarketplaceRequestsTestData* MarketplaceTestData = UMarketplaceRequestsTestData::Make(1);
        USequencePay* Pay = NewObject<USequencePay>();

        const TSuccessCallback<FSeqGetLifiChainsResponse> GenericSuccess = [this, MarketplaceTestData, Pay](const FSeqGetLifiChainsResponse& ChainsResponse)
        {
            const TSuccessCallback<FSeqGetLifiTokensResponse> OnSupportedTokensSuccess = [this, MarketplaceTestData, Pay](const FSeqGetLifiTokensResponse& TokensResponse)
            {
                for (const FSeqLifiToken Token : TokensResponse.Tokens)
                {
                    AddInfo(FString::Printf(TEXT("Supported Token: %s, ChainId: %d, Contract: %s"), *Token.TokenId, Token.ChainId, *Token.ContractAddress));
                }
                
                AddInfo(FString::Printf(TEXT("Remaining tests: %d"), MarketplaceTestData->DecrementPendingRequests()));
            };

            const FFailureCallback GenericFailure = [this, MarketplaceTestData](const FSequenceError& Error)
            {
                const FString Message = "Failed to get supported tokens for chains.";
                AddError(FString::Printf(TEXT("%s with error: %s. Remaining tests: %d"), 
                    *Message, *Error.Message, MarketplaceTestData->DecrementPendingRequests()));
                
                MarketplaceTestData->RequestFailed();
            };
            
            Pay->GetSupportedSwapTokens(ChainsResponse.Chains, OnSupportedTokensSuccess, GenericFailure);
        };

        const FFailureCallback GenericFailure = [this, MarketplaceTestData](const FSequenceError& Error)
        {
            const FString Message = "Failed to get supported swap chains.";
            AddError(FString::Printf(TEXT("%s with error: %s. Remaining tests: %d"),
                *Message, *Error.Message, MarketplaceTestData->DecrementPendingRequests()));
            
            MarketplaceTestData->RequestFailed();
        };
        
        Pay->GetSupportedSwapChains(GenericSuccess, GenericFailure);

        ADD_LATENT_AUTOMATION_COMMAND(FGetSupportedSwapTokensTestIsDone(MarketplaceTestData, this));
        return true;
    }));
    return true;
}