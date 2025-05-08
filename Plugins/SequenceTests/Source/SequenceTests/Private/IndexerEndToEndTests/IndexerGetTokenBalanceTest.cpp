

#include "CoreMinimal.h"
#include "Helpers/IndexerEndToEndTestsCommon.h"
#include "Helpers/IndexerRequestsTestData.h"
#include "Misc/AutomationTest.h"
#include "Util/Async.h"
#include "Util/SequenceSupport.h"
#include "Helpers/IndexerRequestsTestData.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FIndexerGetTokenBalancesTest, "SequencePlugin.EndToEnd.IndexerTests.GetTokenBalancesTest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/* Latent command used to poll off main thread to see if our requests are done */
DEFINE_LATENT_AUTOMATION_COMMAND_TWO_PARAMETER(FIsDoneGetTokenBalances, const UIndexerRequestsTestData *, IndexerRequestsTestData, FAutomationTestBase *, GetTokenBalancesTest);

bool FIsDoneGetTokenBalances::Update()
{
    while(this->IndexerRequestsTestData->GetPendingRequests() > 0)
    {
        return false;
    }

    if (this->IndexerRequestsTestData->GetAllRequestsSuccessful())
    {
        GetTokenBalancesTest->AddInfo(TEXT("GetTokenBalances request completed"));
    }
    else
    {
        GetTokenBalancesTest->AddError(FString::Printf(TEXT("GetTokenBalances request failed")));
    }
    
    return true;
}

void FIndexerGetTokenBalancesTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
    OutBeautifiedNames.Add(TEXT("Indexer Get Token Balances Test"));
    OutTestCommands.Add(TEXT(""));
}

bool FIndexerGetTokenBalancesTest::RunTest(const FString& Parameters)
{
    const int64 PolygonNetworkId = MarketplaceEndToEndTestsCommon::PolygonNetworkId;
    const FString& TestAddress = MarketplaceEndToEndTestsCommon::TestAddress;
    UIndexerRequestsTestData * IndexerRequestsTestData = UIndexerRequestsTestData::Make(1);

    const TSuccessCallback<FSeqGetTokenBalancesReturn> GenericSuccess = [this, IndexerRequestsTestData](const FSeqGetTokenBalancesReturn& TokenBalances)
    {
        TestNotNull(TEXT("TokenBalances"), &TokenBalances);
        TestNotNull(TEXT("TokenBalances.page"), &TokenBalances.page);
        TestNotNull(TEXT("TokenBalances.balances"), &TokenBalances.balances);
        TestTrue(TEXT("TokenBalances.balances should not be empty"), TokenBalances.balances.Num() > 0);

        const FString Message = "GetTokenBalances request succeeded";
        AddInfo(FString::Printf(TEXT("%s. Remaining Requests: %d"), *Message, IndexerRequestsTestData->DecrementPendingRequests()));
    };

    const FFailureCallback GenericFailure = [this, IndexerRequestsTestData](const FSequenceError& Error)
    {
        const FString Message = "Request Failure";
        AddError(FString::Printf(TEXT("%s: %s. Remaining requests: %d"), *Message, *Error.Message, IndexerRequestsTestData->DecrementPendingRequests()));
        IndexerRequestsTestData->RequestFailed();
    };

    AddInfo(FString::Printf(TEXT("Starting GetTokenBalances request")));

    FSeqGetTokenBalancesArgs Args;
    Args.accountAddress = TestAddress;
    IndexerRequestsTestData->GetIndexer()->GetTokenBalances(PolygonNetworkId, Args, GenericSuccess, GenericFailure);
    
    ADD_LATENT_AUTOMATION_COMMAND(FIsDoneGetTokenBalances(IndexerRequestsTestData, this));
    return true;
}