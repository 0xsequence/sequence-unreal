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

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FListCollectibleOffersWithHighestPricedOfferFirst, "SequencePlugin.EndToEnd.MarketplaceTests.ListCollectibleOffersWithHighestPricedOfferFirst", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter | EAutomationTestFlags::ClientContext)


void FListCollectibleOffersWithHighestPricedOfferFirst::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
    OutBeautifiedNames.Add(TEXT("List All Collectible Listings With Lowest Price Listings First Test"));
    OutTestCommands.Add(TEXT(""));
}

bool FListCollectibleOffersWithHighestPricedOfferFirst::RunTest(const FString& Parameters)
{
    AddInfo(TEXT("Testing List Currencies"));
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(true));
    ADD_LATENT_AUTOMATION_COMMAND(FEngineWaitLatentCommand(1.0f));
    ADD_LATENT_AUTOMATION_COMMAND(FFunctionLatentCommand([this]()
    {      
        USequenceSupport* Support = NewObject<USequenceSupport>();

        UMarketplaceRequestsTestData* MarketplaceTestData = UMarketplaceRequestsTestData::Make(1);
        
        const TSuccessCallback<TArray<FSeqCollectibleOrder>> GenericSuccess = [this, MarketplaceTestData](TArray<FSeqCollectibleOrder> Orders)
        {
            if(Orders.Num() == 0)
            {
                const FString Message = "List currencies Failure";
                AddError(FString::Printf(TEXT("%s returned no currencies. Remaining tests: %d"), 
                    *Message,
                    MarketplaceTestData->DecrementPendingRequests()));
                MarketplaceTestData->RequestFailed();
            }
            else
            {
                FString Message;
                for(FSeqCollectibleOrder Order : Orders)
                {
                    Message += USequenceSupport::StructToString(Order) + "\n";
                }
                AddInfo(FString::Printf(TEXT("%s. Remaining pings: %d"), *Message, MarketplaceTestData->DecrementPendingRequests()));
            }
        };

        const FFailureCallback GenericFailure = [this, MarketplaceTestData](const FSequenceError& Error)
        {
            const FString Message = "List currencies Failure";
            AddError(FString::Printf(TEXT("%s with error: %s. Remaining tests: %d"), 
                *Message, 
                *Error.Message, 
                MarketplaceTestData->DecrementPendingRequests()));
            MarketplaceTestData->RequestFailed();
        };
		    
        MarketplaceTestData->GetMarketplace()->ListAllCollectibleOffersWithHighestPricedOfferFirst(
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