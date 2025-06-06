#include <cstdlib>

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Engine/World.h"
#include "Helpers/MarketplaceRequestsTestData.h"
#include "SequencePlugin/Public/Sequence/SequencePay.h"
#include "Tests/AutomationCommon.h"
#include "Tests/AutomationEditorCommon.h"
#include "JsonObjectConverter.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FGetSwapQuoteTest, "SequencePlugin.EndToEnd.MarketplaceTests.GetSwapQuoteTest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter | EAutomationTestFlags::ClientContext)


/* Latent command used to poll off main thread to see if our requests are done */
DEFINE_LATENT_AUTOMATION_COMMAND_TWO_PARAMETER(FGetSwapQuoteTestIsDone, const UMarketplaceRequestsTestData *, IndexerRequestsTestData, FAutomationTestBase *, FGetSwapQuoteTest);

bool FGetSwapQuoteTestIsDone::Update()
{
    while(this->IndexerRequestsTestData->GetPendingRequests() > 0)
    {
        return false;
    }

    if (this->IndexerRequestsTestData->GetAllRequestsSuccessful())
    {
        FGetSwapQuoteTest->AddInfo(TEXT("Get Swap Quote request completed"));
    }
    else
    {
        FGetSwapQuoteTest->AddError(FString::Printf(TEXT("Get Swap Quote request failed")));
    }
    
    return true;
}

void FGetSwapQuoteTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
    OutBeautifiedNames.Add(TEXT("Get Swap Quote Test"));
    OutTestCommands.Add(TEXT(""));
}

bool FGetSwapQuoteTest::RunTest(const FString& Parameters)
{
    AddInfo(TEXT("Testing Get Quote Prices"));
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(true));
    ADD_LATENT_AUTOMATION_COMMAND(FEngineWaitLatentCommand(1.0f));
    ADD_LATENT_AUTOMATION_COMMAND(FFunctionLatentCommand([this]()
    {      
        USequenceSupport* Support = NewObject<USequenceSupport>();
        const FString WalletAddress = "0xe8db071f698aBA1d60babaE8e08F5cBc28782108";
        const FString USDC = "0xaf88d065e77c8cC2239327C5EDb3A432268e5831";
        const FString USDCe = "0xff970a61a04b1ca14834a43f5de4533ebddb5cc8";

        // Only one is accepted for this request
        const FString BuyAmount = "";
        const FString SellAmount = "1000";

        UMarketplaceRequestsTestData* MarketplaceTestData = UMarketplaceRequestsTestData::Make(1);
        
        const TSuccessCallback<FSeqSwapQuote> GenericSuccess = [this, MarketplaceTestData](const FSeqSwapQuote SwapQuote)
        {
            AddInfo(FString::Printf(TEXT("CurrencyAddress: %s, CurrencyBalance: %s, Price: %s, MaxPrice: %s"), *SwapQuote.CurrencyAddress, *SwapQuote.CurrencyBalance, *SwapQuote.Price, *SwapQuote.MaxPrice));
            AddInfo(FString::Printf(TEXT("TransactionData: %s, TransactionValue: %s, ApproveData: %s, Amount: %s, AmountMin: %s"), *SwapQuote.TransactionData, *SwapQuote.TransactionValue, *SwapQuote.ApproveData, *SwapQuote.Amount, *SwapQuote.AmountMin));
            AddInfo(FString::Printf(TEXT("Remaining tests: %d"), MarketplaceTestData->DecrementPendingRequests()));
        };

        const FFailureCallback GenericFailure = [this, MarketplaceTestData](const FSequenceError& Error)
        {
            const FString Message = "Get Swap Quote Failure";
            AddError(FString::Printf(TEXT("%s with error: %s. Remaining tests: %d"), 
                *Message, 
                *Error.Message, 
                MarketplaceTestData->DecrementPendingRequests()));
            MarketplaceTestData->RequestFailed();
        };
        
        USequencePay* Pay = NewObject<USequencePay>();
        Pay->GetSwapQuote(
            Support->GetNetworkId(ENetwork::ArbitrumOne),
            WalletAddress,
            USDC,
            USDCe,
            BuyAmount,
            SellAmount,
            true,
            GenericSuccess,
            GenericFailure
        );
    
        ADD_LATENT_AUTOMATION_COMMAND(FGetSwapQuoteTestIsDone(MarketplaceTestData, this));
        return true;
    }));
    return true;
}