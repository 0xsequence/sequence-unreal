#include <cstdlib>

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Engine/World.h"
#include "Helpers/MarketplaceRequestsTestData.h"
#include "SequencePlugin/Public/Sequence/SequencePay.h"
#include "Tests/AutomationCommon.h"
#include "Tests/AutomationEditorCommon.h"
#include "JsonObjectConverter.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FGetSwapPriceTest, "SequencePlugin.EndToEnd.MarketplaceTests.GetSwapPriceTest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter | EAutomationTestFlags::ClientContext)


/* Latent command used to poll off main thread to see if our requests are done */
DEFINE_LATENT_AUTOMATION_COMMAND_TWO_PARAMETER(FGetSwapPriceTestIsDone, const UMarketplaceRequestsTestData *, IndexerRequestsTestData, FAutomationTestBase *, FGetSwapPriceTest);

bool FGetSwapPriceTestIsDone::Update()
{
    while(this->IndexerRequestsTestData->GetPendingRequests() > 0)
    {
        return false;
    }

    if (this->IndexerRequestsTestData->GetAllRequestsSuccessful())
    {
        FGetSwapPriceTest->AddInfo(TEXT("Get Swap Price request completed"));
    }
    else
    {
        FGetSwapPriceTest->AddError(FString::Printf(TEXT("Get Swap Price request failed")));
    }
    
    return true;
}

void FGetSwapPriceTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
    OutBeautifiedNames.Add(TEXT("Get Swap Price Test"));
    OutTestCommands.Add(TEXT(""));
}

bool FGetSwapPriceTest::RunTest(const FString& Parameters)
{
    AddInfo(TEXT("Testing Get Swap Price"));
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(true));
    ADD_LATENT_AUTOMATION_COMMAND(FEngineWaitLatentCommand(1.0f));
    ADD_LATENT_AUTOMATION_COMMAND(FFunctionLatentCommand([this]()
    {      
        const USequenceSupport* Support = NewObject<USequenceSupport>();
        const FString WalletAddress = "0xe8db071f698aBA1d60babaE8e08F5cBc28782108";
        const FString SellUSDC = "0xaf88d065e77c8cC2239327C5EDb3A432268e5831";
        const FString BuyUSDCe = "0xff970a61a04b1ca14834a43f5de4533ebddb5cc8";
        const FString BuyAmount = "1000";

        UMarketplaceRequestsTestData* MarketplaceTestData = UMarketplaceRequestsTestData::Make(1);
        
        const TSuccessCallback<FSeqSwapPrice> GenericSuccess = [this, MarketplaceTestData](const FSeqSwapPrice& SwapPrice)
        {
            AddInfo(FString::Printf(TEXT("CurrencyAddress: %s, Price: %d, Remaining tests: %d"), *SwapPrice.CurrencyAddress, SwapPrice.Price, MarketplaceTestData->DecrementPendingRequests()));
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
        Pay->GetSwapPrice(
            Support->GetNetworkId(ENetwork::ArbitrumOne),
            WalletAddress,
            SellUSDC,
            BuyUSDCe,
            BuyAmount,
            GenericSuccess,
            GenericFailure
        );

        ADD_LATENT_AUTOMATION_COMMAND(FGetSwapPriceTestIsDone(MarketplaceTestData, this));
        return true;
    }));
    return true;
}