#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Engine/World.h"
#include "Helpers/MarketplaceRequestsTestData.h"
#include "Marketplace/SequenceMarketplace.h"
#include "Tests/AutomationCommon.h"
#include "Tests/AutomationEditorCommon.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FGetLowestPriceListingForCollectibleTest, "SequencePlugin.EndToEnd.MarketplaceTests.GetLowestPriceListingForCollectibleTest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter | EAutomationTestFlags::ClientContext)

/* Latent command used to poll off main thread to see if our requests are done */
DEFINE_LATENT_AUTOMATION_COMMAND_TWO_PARAMETER(FGetLowestPriceListingForCollectibleTestIsDone, const UMarketplaceRequestsTestData *, MarketPlaceRequestsTestData, FAutomationTestBase *, FGetLowestPriceListingForCollectibleTest);

bool FGetLowestPriceListingForCollectibleTestIsDone::Update()
{
    while(this->MarketPlaceRequestsTestData->GetPendingRequests() > 0)
    {
        return false;
    }

    if (this->MarketPlaceRequestsTestData->GetAllRequestsSuccessful())
    {
        FGetLowestPriceListingForCollectibleTest->AddInfo(TEXT("GetFloorOrderTest request completed"));
    }
    else
    {
        FGetLowestPriceListingForCollectibleTest->AddError(FString::Printf(TEXT("GetFloorOrderTest request failed")));
    }
    
    return true;
}

void FGetLowestPriceListingForCollectibleTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
    OutBeautifiedNames.Add(TEXT("Get Lowest Price Listing For Collectible Test"));
    OutTestCommands.Add(TEXT(""));
}

bool FGetLowestPriceListingForCollectibleTest::RunTest(const FString& Parameters)
{
    AddInfo(TEXT("Testing Get Lowest Price Listing For Collectible"));
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
            const FString Message = "Get Lowest Price Listing For Collectible Failure";
            AddError(FString::Printf(TEXT("%s with error: %s. Remaining tests: %d"), 
                *Message, 
                *Error.Message, 
                MarketplaceTestData->DecrementPendingRequests()));
            MarketplaceTestData->RequestFailed();
        };
		    
        MarketplaceTestData->GetMarketplace()->GetLowestPriceListingForCollectible(
            Support->GetNetworkId(ENetwork::PolygonChain),
            "0x079294e6ffec16234578c672fa3fbfd4b6c48640",
            "1",
            FSeqCollectiblesFilter::Empty(),
            GenericSuccess,
            GenericFailure
        );
    
        ADD_LATENT_AUTOMATION_COMMAND(FGetLowestPriceListingForCollectibleTestIsDone(MarketplaceTestData, this));
        return true;
    }));
    return true;
}