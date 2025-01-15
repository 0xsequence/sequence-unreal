// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "SequencePlugin/Public/Sequence/SequenceAPI.h"
#include "Engine/World.h"
#include "Helpers/MarketplaceRequestsTestData.h"
#include "Marketplace/Marketplace.h"
#include "Tests/AutomationCommon.h"
#include "Tests/AutomationEditorCommon.h"
#include "JsonObjectConverter.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FGetHighestPriceOfferForCollectibleTest, "SequencePlugin.EndToEnd.MarketplaceTests.GetHighestPriceOfferForCollectibleTest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter | EAutomationTestFlags::ClientContext)


void FGetHighestPriceOfferForCollectibleTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
    OutBeautifiedNames.Add(TEXT("Get Highest Price Offer For Collectible Test"));
    OutTestCommands.Add(TEXT(""));
}

bool FGetHighestPriceOfferForCollectibleTest::RunTest(const FString& Parameters)
{
    AddInfo(TEXT("Testing Get Highest Price Offer For Collectible"));
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(true));
    ADD_LATENT_AUTOMATION_COMMAND(FEngineWaitLatentCommand(1.0f));
    ADD_LATENT_AUTOMATION_COMMAND(FFunctionLatentCommand([this]()
    {      
        USequenceSupport* Support = NewObject<USequenceSupport>();

        UMarketplaceRequestsTestData* MarketplaceTestData = UMarketplaceRequestsTestData::Make(1);
        
        const TSuccessCallback<FSeqCollectibleOrder> GenericSuccess = [this, MarketplaceTestData](FSeqCollectibleOrder Order)
        {
            AddInfo(FString::Printf(TEXT("%s. Remaining tests: %d"), *Order.Order.OrderId, MarketplaceTestData->DecrementPendingRequests()));
        };

        const FFailureCallback GenericFailure = [this, MarketplaceTestData](const FSequenceError& Error)
        {
            const FString Message = "Get Highest Price Offer For Collectible Failure";
            AddError(FString::Printf(TEXT("%s with error: %s. Remaining tests: %d"), 
                *Message, 
                *Error.Message, 
                MarketplaceTestData->DecrementPendingRequests()));
            MarketplaceTestData->RequestFailed();
        };
		    
        MarketplaceTestData->GetMarketplace()->GetHighestPriceOfferForCollectible(
            Support->GetNetworkId(ENetwork::PolygonChain),
            "0x079294e6ffec16234578c672fa3fbfd4b6c48640",
            "1",
            FSeqCollectiblesFilter::Empty(),
            GenericSuccess,
            GenericFailure
        );
    
        return true;
    }));
    return true;
}