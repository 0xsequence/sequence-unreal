#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Engine/World.h"
#include "Helpers/MarketplaceRequestsTestData.h"
#include "Marketplace/SequenceMarketplace.h"
#include "Tests/AutomationCommon.h"
#include "Tests/AutomationEditorCommon.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FGetHighestPriceListingForCollectibleTest, "SequencePlugin.EndToEnd.MarketplaceTests.GetHighestPriceListingForCollectibleTest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter | EAutomationTestFlags::ClientContext)

/* Latent command used to poll off main thread to see if our requests are done */
DEFINE_LATENT_AUTOMATION_COMMAND_TWO_PARAMETER(FGetHighestPriceListingForCollectibleTestIsDone, const UMarketplaceRequestsTestData *, MarketPlaceRequestsTestData, FAutomationTestBase *, FGetHighestPriceListingForCollectibleTest);

bool FGetHighestPriceListingForCollectibleTestIsDone::Update()
{
    while(this->MarketPlaceRequestsTestData->GetPendingRequests() > 0)
    {
        return false;
    }

    if (this->MarketPlaceRequestsTestData->GetAllRequestsSuccessful())
    {
        FGetHighestPriceListingForCollectibleTest->AddInfo(TEXT("GetFloorOrderTest request completed"));
    }
    else
    {
        FGetHighestPriceListingForCollectibleTest->AddError(FString::Printf(TEXT("GetFloorOrderTest request failed")));
    }
    
    return true;
}

void FGetHighestPriceListingForCollectibleTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
    OutBeautifiedNames.Add(TEXT("Get Highest Price Listing For Collectible Test"));
    OutTestCommands.Add(TEXT(""));
}

bool FGetHighestPriceListingForCollectibleTest::RunTest(const FString& Parameters)
{
    AddInfo(TEXT("Testing Get Highest Price Listing For Collectible"));
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
            const FString Message = "Get Highest Price Listing For Collectible Failure";
            AddError(FString::Printf(TEXT("%s with error: %s. Remaining tests: %d"), 
                *Message, 
                *Error.Message, 
                MarketplaceTestData->DecrementPendingRequests()));
            MarketplaceTestData->RequestFailed();
        };
		    
        MarketplaceTestData->GetMarketplace()->GetHighestPriceListingForCollectible(
            Support->GetNetworkId(ENetwork::PolygonChain),
            "0x079294e6ffec16234578c672fa3fbfd4b6c48640",
            "1",
            FSeqCollectiblesFilter::Empty(),
            GenericSuccess,
            GenericFailure
        );
    
        ADD_LATENT_AUTOMATION_COMMAND(FGetHighestPriceListingForCollectibleTestIsDone(MarketplaceTestData, this));
        return true;
    }));
    return true;
}