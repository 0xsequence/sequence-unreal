#include "JsonObjectConverter.h"
#include "CoreMinimal.h"
#include "SequencePlugin/Public/Sequence/SequenceWallet.h"
#include "Engine/World.h"
#include "Helpers/MarketplaceRequestsTestData.h"
#include "Marketplace/SequenceMarketplace.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "Tests/AutomationEditorCommon.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FListCurrenciesTest, "SequencePlugin.EndToEnd.MarketplaceTests.ListCurrenciesTest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter | EAutomationTestFlags::ClientContext)

/* Latent command used to poll off main thread to see if our requests are done */
DEFINE_LATENT_AUTOMATION_COMMAND_TWO_PARAMETER(FListCurrenciesTestIsDone, const UMarketplaceRequestsTestData *, MarketPlaceRequestsTestData, FAutomationTestBase *, FListCurrenciesTest);

bool FListCurrenciesTestIsDone::Update()
{
    while(this->MarketPlaceRequestsTestData->GetPendingRequests() > 0)
    {
        return false;
    }

    if (this->MarketPlaceRequestsTestData->GetAllRequestsSuccessful())
    {
        FListCurrenciesTest->AddInfo(TEXT("GetFloorOrderTest request completed"));
    }
    else
    {
        FListCurrenciesTest->AddError(FString::Printf(TEXT("GetFloorOrderTest request failed")));
    }
    
    return true;
}

void FListCurrenciesTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
    OutBeautifiedNames.Add(TEXT("List Currencies Test"));
    OutTestCommands.Add(TEXT(""));
}

bool FListCurrenciesTest::RunTest(const FString& Parameters)
{
    AddInfo(TEXT("Testing List Currencies"));
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(true));
    ADD_LATENT_AUTOMATION_COMMAND(FEngineWaitLatentCommand(1.0f));
    ADD_LATENT_AUTOMATION_COMMAND(FFunctionLatentCommand([this]()
    {      
        USequenceMarketplace* Marketplace = NewObject<USequenceMarketplace>();
        USequenceSupport* Support = NewObject<USequenceSupport>();

        UMarketplaceRequestsTestData* MarketplaceTestData = UMarketplaceRequestsTestData::Make(1);
        
        const TSuccessCallback<FString> GenericSuccess = [this, MarketplaceTestData](const FString Message)
        {
            AddInfo(FString::Printf(TEXT("%s. Remaining pings: %d"), *Message, MarketplaceTestData->DecrementPendingRequests()));
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

        Marketplace->ListCurrencies(Support->GetNetworkId(PolygonChain), [this, MarketplaceTestData, GenericSuccess](FSeqListCurrenciesReturn Response)
        {
            FString Currencies = "Currencies: \n";

            for(FSeqCurrency Currency : Response.Currencies)
            {
                Currencies += USequenceSupport::StructToString(Currency) + "\n";
            }

            GenericSuccess(Currencies);
        }, GenericFailure);
    
        ADD_LATENT_AUTOMATION_COMMAND(FListCurrenciesTestIsDone(MarketplaceTestData, this));
        return true;
    }));
    return true;
}