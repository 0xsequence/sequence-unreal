#include <cstdlib>

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Engine/World.h"
#include "Helpers/MarketplaceRequestsTestData.h"
#include "SequencePlugin/Public/Sequence/SequencePay.h"
#include "Tests/AutomationCommon.h"
#include "Tests/AutomationEditorCommon.h"
#include "JsonObjectConverter.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FGetSupportedSwapChainsTest, "SequencePlugin.EndToEnd.MarketplaceTests.GetSupportedSwapChainsTest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter | EAutomationTestFlags::ClientContext)

DEFINE_LATENT_AUTOMATION_COMMAND_TWO_PARAMETER(FGetSupportedSwapChainsTestIsDone, const UMarketplaceRequestsTestData*, IndexerRequestsTestData, FAutomationTestBase*, FGetSupportedSwapChainsTest);

bool FGetSupportedSwapChainsTestIsDone::Update()
{
    while(this->IndexerRequestsTestData->GetPendingRequests() > 0)
    {
        return false;
    }

    if (this->IndexerRequestsTestData->GetAllRequestsSuccessful())
    {
        FGetSupportedSwapChainsTest->AddInfo(TEXT("Get Supported Swap Chains: Request Completed"));
    }
    else
    {
        FGetSupportedSwapChainsTest->AddError(FString::Printf(TEXT("Get Supported Swap Chains: Request Failed")));
    }
    
    return true;
}

void FGetSupportedSwapChainsTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
    OutBeautifiedNames.Add(TEXT("Get Supported Swap Chains Test"));
    OutTestCommands.Add(TEXT(""));
}

bool FGetSupportedSwapChainsTest::RunTest(const FString& Parameters)
{
    AddInfo(TEXT("Testing Get Supported Swap Chains"));
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(true));
    ADD_LATENT_AUTOMATION_COMMAND(FEngineWaitLatentCommand(1.0f));
    ADD_LATENT_AUTOMATION_COMMAND(FFunctionLatentCommand([this]()
    {      
        UMarketplaceRequestsTestData* MarketplaceTestData = UMarketplaceRequestsTestData::Make(1);
        
        const TSuccessCallback<FSeqGetLifiChainsResponse> GenericSuccess = [this, MarketplaceTestData](const FSeqGetLifiChainsResponse& Response)
        {
            for (const int64 ChainId : Response.Chains)
            {
                AddInfo(FString::Printf(TEXT("Supported ChainId: %d"), ChainId));
            }
            
            AddInfo(FString::Printf(TEXT("Remaining tests: %d"), MarketplaceTestData->DecrementPendingRequests()));
        };

        const FFailureCallback GenericFailure = [this, MarketplaceTestData](const FSequenceError& Error)
        {
            const FString Message = "Get Swap Price Failure";
            AddError(FString::Printf(TEXT("%s with error: %s. Remaining tests: %d"), 
                *Message, 
                *Error.Message, 
                MarketplaceTestData->DecrementPendingRequests()));
            MarketplaceTestData->RequestFailed();
        };
        
        USequencePay* Pay = NewObject<USequencePay>();
        Pay->GetSupportedSwapChains(GenericSuccess, GenericFailure);

        ADD_LATENT_AUTOMATION_COMMAND(FGetSupportedSwapChainsTestIsDone(MarketplaceTestData, this));
        return true;
    }));
    return true;
}