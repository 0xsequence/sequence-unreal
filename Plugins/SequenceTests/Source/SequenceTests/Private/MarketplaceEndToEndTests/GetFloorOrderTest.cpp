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

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FGetFloorOrderTest, "SequencePlugin.EndToEnd.MarketplaceTests.GetFloorOrder", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter | EAutomationTestFlags::ClientContext)


void FGetFloorOrderTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
    OutBeautifiedNames.Add(TEXT("Get Floor Order Test"));
    OutTestCommands.Add(TEXT(""));
}

bool FGetFloorOrderTest::RunTest(const FString& Parameters)
{
    AddInfo(TEXT("Testing Get Floor Order"));
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(true));
    ADD_LATENT_AUTOMATION_COMMAND(FEngineWaitLatentCommand(1.0f));
    ADD_LATENT_AUTOMATION_COMMAND(FFunctionLatentCommand([this]()
    {      
        USequenceSupport* Support = NewObject<USequenceSupport>();

        UMarketplaceRequestsTestData* MarketplaceTestData = UMarketplaceRequestsTestData::Make(1);
        
        const TSuccessCallback<FSeqCollectibleOrder> GenericSuccess = [this, MarketplaceTestData](FSeqCollectibleOrder Order)
        {
            AddInfo(FString::Printf(TEXT("ID: %s. Remaining tests: %d"), *Order.Order.OrderId, MarketplaceTestData->DecrementPendingRequests()));
        };

        const FFailureCallback GenericFailure = [this, MarketplaceTestData](const FSequenceError& Error)
        {
            const FString Message = "Get Floor Order Failure";
            AddError(FString::Printf(TEXT("%s with error: %s. Remaining tests: %d"), 
                *Message, 
                *Error.Message, 
                MarketplaceTestData->DecrementPendingRequests()));
            MarketplaceTestData->RequestFailed();
        };
		    
        MarketplaceTestData->GetMarketplace()->GetFloorOrder(
            Support->GetNetworkId(ENetwork::PolygonChain),
            "0x44b3f42e2BF34F62868Ff9e9dAb7C2F807ba97Cb",
            FSeqCollectiblesFilter::Empty(),
            GenericSuccess,
            GenericFailure
        );
    
        return true;
    }));
    return true;
}