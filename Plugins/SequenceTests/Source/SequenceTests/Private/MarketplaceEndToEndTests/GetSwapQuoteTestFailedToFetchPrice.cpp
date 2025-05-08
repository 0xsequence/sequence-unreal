// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include <cstdlib>

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Engine/World.h"
#include "Helpers/MarketplaceRequestsTestData.h"
#include "SequencePlugin/Public/Sequence/SequencePay.h"
#include "Tests/AutomationCommon.h"
#include "Tests/AutomationEditorCommon.h"
#include "JsonObjectConverter.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FGetSwapQuoteFailedToFetchPriceTest, "SequencePlugin.EndToEnd.MarketplaceTests.GetSwapQuoteFailedToFetchPriceTest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter | EAutomationTestFlags::ClientContext)


/* Latent command used to poll off main thread to see if our requests are done */
DEFINE_LATENT_AUTOMATION_COMMAND_TWO_PARAMETER(FGetSwapQuoteFailedToFetchPriceTestIsDone, const UMarketplaceRequestsTestData *, IndexerRequestsTestData, FAutomationTestBase *, FGetSwapQuoteFailedToFetchPriceTest);

bool FGetSwapQuoteFailedToFetchPriceTestIsDone::Update()
{
    while(this->IndexerRequestsTestData->GetPendingRequests() > 0)
    {
        return false;
    }

    if (this->IndexerRequestsTestData->GetAllRequestsSuccessful())
    {
        FGetSwapQuoteFailedToFetchPriceTest->AddInfo(TEXT("Get Swap Quote Failed To Fetch Price request completed"));
    }
    else
    {
        FGetSwapQuoteFailedToFetchPriceTest->AddError(FString::Printf(TEXT("Get Swap Quote Failed To Fetch Price request failed")));
    }
    
    return true;
}

void FGetSwapQuoteFailedToFetchPriceTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
    OutBeautifiedNames.Add(TEXT("Get Swap Quote Failed To FetchPrice Test"));
    OutTestCommands.Add(TEXT(""));
}

bool FGetSwapQuoteFailedToFetchPriceTest::RunTest(const FString& Parameters)
{
    AddInfo(TEXT("Testing Get Quote Prices"));
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(true));
    ADD_LATENT_AUTOMATION_COMMAND(FEngineWaitLatentCommand(1.0f));
    ADD_LATENT_AUTOMATION_COMMAND(FFunctionLatentCommand([this]()
    {      
        USequenceSupport* Support = NewObject<USequenceSupport>();
        const FString USDC = "0xaf88d065e77c8cC2239327C5EDb3A432268e5831";
        const FString USDCe = "0xff970a61a04b1ca14834a43f5de4533ebddb5cc8";
        const FString Amount = "1000000000000000000000000000000000000";
        const FString UserAddress = "0xe8db071f698aBA1d60babaE8e08F5cBc28782108";

        UMarketplaceRequestsTestData* MarketplaceTestData = UMarketplaceRequestsTestData::Make(1);
        
        const TSuccessCallback<FSeqSwapQuote> GenericSuccess = [this, MarketplaceTestData](FSeqSwapQuote SwapQuote)
        {
            const FString Message = "Get Swap Quote Failure";
            AddError(FString::Printf(TEXT("%s as we expected an error. Remaining tests: %d"), 
                *Message, 
                MarketplaceTestData->DecrementPendingRequests()));
            MarketplaceTestData->RequestFailed();
        };

        const FFailureCallback GenericFailure = [this, MarketplaceTestData](const FSequenceError& Error)
        {
            if(Error.Type == ResponseParseError)
            {
                AddInfo(FString::Printf(TEXT("Success! Remaining tests: %d"), MarketplaceTestData->DecrementPendingRequests()));
                return;
            }

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
            UserAddress,
            USDC,
            USDCe,
            Amount,
            true,
            GenericSuccess,
            GenericFailure
        );
    
        ADD_LATENT_AUTOMATION_COMMAND(FGetSwapQuoteFailedToFetchPriceTestIsDone(MarketplaceTestData, this));
        return true;
    }));
    return true;
}