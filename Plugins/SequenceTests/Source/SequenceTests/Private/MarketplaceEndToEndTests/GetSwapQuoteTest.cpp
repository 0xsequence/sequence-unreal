// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include <cstdlib>

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "SequencePlugin/Public/Sequence/SequenceAPI.h"
#include "Engine/World.h"
#include "Helpers/MarketplaceRequestsTestData.h"
#include "Marketplace/Marketplace.h"
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
        const FString USDC = "0xaf88d065e77c8cC2239327C5EDb3A432268e5831";
        const FString USDCe = "0xff970a61a04b1ca14834a43f5de4533ebddb5cc8";
        const FString Amount = "1000";
        const FString UserAddress = "0xe8db071f698aBA1d60babaE8e08F5cBc28782108";

        UMarketplaceRequestsTestData* MarketplaceTestData = UMarketplaceRequestsTestData::Make(1);
        
        const TSuccessCallback<FSeqSwapQuote> GenericSuccess = [this, MarketplaceTestData](FSeqSwapQuote SwapQuote)
        {
            AddInfo(FString::Printf(TEXT("ID: %s. Remaining tests: %d"), *SwapQuote.Price, MarketplaceTestData->DecrementPendingRequests()));
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
		    
        MarketplaceTestData->GetMarketplace()->GetSwapQuote(
            Support->GetNetworkId(ENetwork::ArbitrumOne),
            UserAddress,
            USDC,
            USDCe,
            Amount,
            true,
            GenericSuccess,
            GenericFailure
        );
    
        ADD_LATENT_AUTOMATION_COMMAND(FGetSwapQuoteTestIsDone(MarketplaceTestData, this));
        return true;
    }));
    return true;
}