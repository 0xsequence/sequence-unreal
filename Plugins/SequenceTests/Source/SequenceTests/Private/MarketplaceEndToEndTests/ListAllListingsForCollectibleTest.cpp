#include <cstdlib>

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Engine/World.h"
#include "Helpers/MarketplaceRequestsTestData.h"
#include "Marketplace/SequenceMarketplace.h"
#include "Tests/AutomationCommon.h"
#include "Tests/AutomationEditorCommon.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FListAllListingsForCollectible, "SequencePlugin.EndToEnd.MarketplaceTests.ListAllListingsForCollectible", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter | EAutomationTestFlags::ClientContext)

/* Latent command used to poll off main thread to see if our requests are done */
DEFINE_LATENT_AUTOMATION_COMMAND_TWO_PARAMETER(FListAllListingsForCollectibleIsDone, const UMarketplaceRequestsTestData *, MarketPlaceRequestsTestData, FAutomationTestBase *, FListAllListingsForCollectible);

bool FListAllListingsForCollectibleIsDone::Update()
{
    while(this->MarketPlaceRequestsTestData->GetPendingRequests() > 0)
    {
        return false;
    }

    if (this->MarketPlaceRequestsTestData->GetAllRequestsSuccessful())
    {
        FListAllListingsForCollectible->AddInfo(TEXT("GetFloorOrderTest request completed"));
    }
    else
    {
        FListAllListingsForCollectible->AddError(FString::Printf(TEXT("GetFloorOrderTest request failed")));
    }
    
    return true;
}

void FListAllListingsForCollectible::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
    OutBeautifiedNames.Add(TEXT("List All Collectible Listings With Lowest Price Listings First Test"));
    OutTestCommands.Add(TEXT(""));
}

bool FListAllListingsForCollectible::RunTest(const FString& Parameters)
{
    AddInfo(TEXT("Testing List All Listings For Collectible"));
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
                const FString Message = "List All Listings For Collectible Failure";
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
                AddInfo(FString::Printf(TEXT("%s. Remaining tests: %d"), *Message, MarketplaceTestData->DecrementPendingRequests()));
            }
        };

        const FFailureCallback GenericFailure = [this, MarketplaceTestData](const FSequenceError& Error)
        {
            const FString Message = "List All Listings For Collectible Failure";
            AddError(FString::Printf(TEXT("%s with error: %s. Remaining tests: %d"), 
                *Message, 
                *Error.Message, 
                MarketplaceTestData->DecrementPendingRequests()));
            MarketplaceTestData->RequestFailed();
        };
		    
        MarketplaceTestData->GetMarketplace()->ListAllListingsForCollectible(
            Support->GetNetworkId(ENetwork::PolygonChain),
            "0x079294e6ffec16234578c672fa3fbfd4b6c48640",
            "1",
            FSeqCollectiblesFilter::Empty(),
            GenericSuccess,
            GenericFailure
        );
    
        ADD_LATENT_AUTOMATION_COMMAND(FListAllListingsForCollectibleIsDone(MarketplaceTestData, this));
        return true;
    }));
    return true;
}