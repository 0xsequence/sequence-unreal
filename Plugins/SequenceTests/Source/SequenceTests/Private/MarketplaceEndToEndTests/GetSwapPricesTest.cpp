// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include <cstdlib>

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Engine/World.h"
#include "Helpers/MarketplaceRequestsTestData.h"
#include "SequencePlugin/Public/Marketplace/SequenceMarketplace.h"
#include "Tests/AutomationCommon.h"
#include "Tests/AutomationEditorCommon.h"
#include "JsonObjectConverter.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FGetSwapPricesTest, "SequencePlugin.EndToEnd.MarketplaceTests.GetSwapPricesTest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter | EAutomationTestFlags::ClientContext)

/* Latent command used to poll off main thread to see if our requests are done */
DEFINE_LATENT_AUTOMATION_COMMAND_TWO_PARAMETER(FGetSwapPricesTestIsDone, const UMarketplaceRequestsTestData *, IndexerRequestsTestData, FAutomationTestBase *, FGetSwapPricesTest);

bool FGetSwapPricesTestIsDone::Update()
{
    while(this->IndexerRequestsTestData->GetPendingRequests() > 0)
    {
        return false;
    }

    if (this->IndexerRequestsTestData->GetAllRequestsSuccessful())
    {
        FGetSwapPricesTest->AddInfo(TEXT("Get Swap Prices request completed"));
    }
    else
    {
        FGetSwapPricesTest->AddError(FString::Printf(TEXT("Get Swap Prices request failed")));
    }
    
    return true;
}

void FGetSwapPricesTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
    OutBeautifiedNames.Add(TEXT("Get Swap Prices Test"));
    OutTestCommands.Add(TEXT(""));
}

bool FGetSwapPricesTest::RunTest(const FString& Parameters)
{
    AddInfo(TEXT("Testing Get Swap Prices"));
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(true));
    ADD_LATENT_AUTOMATION_COMMAND(FEngineWaitLatentCommand(1.0f));
    ADD_LATENT_AUTOMATION_COMMAND(FFunctionLatentCommand([this]()
    {      
        USequenceSupport* Support = NewObject<USequenceSupport>();
        const FString USDC = "0xaf88d065e77c8cC2239327C5EDb3A432268e5831";
        const FString USDCe = "0xff970a61a04b1ca14834a43f5de4533ebddb5cc8";
        const FString amount = "1000";

        UMarketplaceRequestsTestData* MarketplaceTestData = UMarketplaceRequestsTestData::Make(1);
        
        const TSuccessCallback<TArray<FSeqSwapPrice>> GenericSuccess = [this, MarketplaceTestData](TArray<FSeqSwapPrice> SwapPrices)
        {
            AddInfo(FString::Printf(TEXT("ID: %d. Remaining tests: %d"), SwapPrices.Num(), MarketplaceTestData->DecrementPendingRequests()));
        };

        const FFailureCallback GenericFailure = [this, MarketplaceTestData](const FSequenceError& Error)
        {
            const FString Message = "Get Swap Prices Failure";
            AddError(FString::Printf(TEXT("%s with error: %s. Remaining tests: %d"), 
                *Message, 
                *Error.Message, 
                MarketplaceTestData->DecrementPendingRequests()));
            MarketplaceTestData->RequestFailed();
        };
		    
        MarketplaceTestData->GetMarketplace()->GetSwapPrices(
            Support->GetNetworkId(ENetwork::ArbitrumOne),
            USDC,
            USDCe,
            amount,
            GenericSuccess,
            GenericFailure
        );

        ADD_LATENT_AUTOMATION_COMMAND(FGetSwapPricesTestIsDone(MarketplaceTestData, this));
        return true;
    }));
    
    return true;
}