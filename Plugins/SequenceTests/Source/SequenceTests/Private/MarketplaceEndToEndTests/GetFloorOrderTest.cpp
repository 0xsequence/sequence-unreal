#include <cstdlib>

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Engine/World.h"
#include "Helpers/MarketplaceRequestsTestData.h"
#include "Marketplace/SequenceMarketplace.h"
#include "Tests/AutomationCommon.h"
#include "Tests/AutomationEditorCommon.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FGetFloorOrderTest, "SequencePlugin.EndToEnd.MarketplaceTests.GetFloorOrder", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter | EAutomationTestFlags::ClientContext)

/* Latent command used to poll off main thread to see if our requests are done */
DEFINE_LATENT_AUTOMATION_COMMAND_TWO_PARAMETER(FGetFloorOrderTestIsDone, const UMarketplaceRequestsTestData *, MarketPlaceRequestsTestData, FAutomationTestBase *, FGetFloorOrderTest);

bool FGetFloorOrderTestIsDone::Update()
{
    while(this->MarketPlaceRequestsTestData->GetPendingRequests() > 0)
    {
        return false;
    }

    if (this->MarketPlaceRequestsTestData->GetAllRequestsSuccessful())
    {
        FGetFloorOrderTest->AddInfo(TEXT("GetFloorOrderTest request completed"));
    }
    else
    {
        FGetFloorOrderTest->AddError(FString::Printf(TEXT("GetFloorOrderTest request failed")));
    }
    
    return true;
}

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
    
        ADD_LATENT_AUTOMATION_COMMAND(FGetFloorOrderTestIsDone(MarketplaceTestData, this));
        return true;
    }));
    return true;
}