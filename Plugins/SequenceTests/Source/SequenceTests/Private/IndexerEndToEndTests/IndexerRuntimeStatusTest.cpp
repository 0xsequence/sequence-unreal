

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Util/Async.h"
#include "Util/SequenceSupport.h"
#include "Helpers/IndexerRequestsTestData.h"
#include "Helpers/BatchTestBuilder.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FIndexerRuntimeStatusTest, "SequencePlugin.EndToEnd.IndexerTests.RuntimeStatusTest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/* Latent command used to poll off main thread to see if our requests are done */
DEFINE_LATENT_AUTOMATION_COMMAND_TWO_PARAMETER(FIsDoneRuntimeStatusTests, const UIndexerRequestsTestData *, IndexerRequestsTestData, FAutomationTestBase *, RuntimeStatusTest);

/* Latent command used to batch process requests w/o exceeding network threading limits */
DEFINE_LATENT_AUTOMATION_COMMAND_FIVE_PARAMETER(FProcessRuntimeRequests, const int32, WatchIndex, const int32, FinishIndex, const UIndexerRequestsTestData *, IndexerRequestsTestData, const TSuccessCallback<FSeqRuntimeStatus>, SuccessCallback, const FFailureCallback, FailureCallback);

bool FProcessRuntimeRequests::Update()
{
    while(IndexerRequestsTestData->GetRequestsComplete() < WatchIndex)
    {
        return false;
    }

    const TArray<int64> Networks = USequenceSupport::GetAllNetworkIds();
    
    for (int i = WatchIndex; i <= FinishIndex; i++)
    {
        UE_LOG(LogTemp, Display, TEXT("Index: %d, Testing Network: %lld"), i, Networks[i]);
        IndexerRequestsTestData->GetIndexer()->RuntimeStatus(Networks[i], SuccessCallback, FailureCallback);
    }
    
    return true;
}

bool FIsDoneRuntimeStatusTests::Update()
{
    while(this->IndexerRequestsTestData->GetPendingRequests() > 0)
    {
        return false;
    }

    if (this->IndexerRequestsTestData->GetAllRequestsSuccessful())
    {
        RuntimeStatusTest->AddInfo(TEXT("All runtime status requests completed"));
    }
    else
    {
        RuntimeStatusTest->AddError(FString::Printf(TEXT("Not all runtime status requests returned successfully")));
    }
    
    return true;
}

void FIndexerRuntimeStatusTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
    OutBeautifiedNames.Add(TEXT("Indexer Runtime Status Test"));
    OutTestCommands.Add(TEXT(""));
}

bool FIndexerRuntimeStatusTest::RunTest(const FString& Parameters)
{
    const TArray<int64> Networks = USequenceSupport::GetAllNetworkIds();
    UIndexerRequestsTestData * IndexerRequestsTestData = UIndexerRequestsTestData::Make(Networks.Num());

    const TSuccessCallback<FSeqRuntimeStatus> GenericSuccess = [this, IndexerRequestsTestData](const FSeqRuntimeStatus& Status)
    {
        TestNotNull(TEXT("RuntimeStatus"), &Status);
        TestTrue(TEXT("Indexer should be enabled"), Status.indexerEnabled);
        TestTrue(TEXT("Health should be OK"), Status.healthOK);
        TestFalse(TEXT("StartTime should not be empty"), Status.startTime.IsEmpty());
        TestFalse(TEXT("Branch should not be empty"), Status.branch.IsEmpty());
        TestFalse(TEXT("CommitHash should not be empty"), Status.commitHash.IsEmpty());
        TestTrue(TEXT("ChainID should be supported"), USequenceSupport::IsNetworkIdSupported(Status.chainID));
        TestTrue(TEXT("Uptime should be greater than 0"), Status.uptime > 0);
        TestNotNull(TEXT("Checks should not be null"), &Status.checks);
        TestTrue(TEXT("Checks should be running"), Status.checks.running);
        TestFalse(TEXT("SyncMode should not be empty"), Status.checks.syncMode.IsEmpty());
        TestTrue(TEXT("LastBlockNum should be greater than 0"), Status.checks.lastBlockNum > 0);

        const FString Message = "Runtime status request succeeded";
        AddInfo(FString::Printf(TEXT("%s. Remaining Requests: %d"), *Message, IndexerRequestsTestData->DecrementPendingRequests()));
    };

    const FFailureCallback GenericFailure = [this, IndexerRequestsTestData](const FSequenceError& Error)
    {
        const FString Message = "Request Failure";
        AddError(FString::Printf(TEXT("%s: %s. Remaining requests: %d"), *Message, *Error.Message, IndexerRequestsTestData->DecrementPendingRequests()));
        IndexerRequestsTestData->RequestFailed();
    };

    AddInfo(FString::Printf(TEXT("Starting %d requests"), IndexerRequestsTestData->GetPendingRequests()));

    constexpr int32 BatchSize = 5;
    FBatchTestBuilder BatchTestBuilder(BatchSize, Networks.Num());

    while (BatchTestBuilder.CanBuildBatch())
    {
        ADD_LATENT_AUTOMATION_COMMAND(FProcessRuntimeRequests(BatchTestBuilder.GetStartIndex(), BatchTestBuilder.GetEndIndex(), IndexerRequestsTestData, GenericSuccess, GenericFailure));
        BatchTestBuilder.BuildNextBatch();
    }
    
    ADD_LATENT_AUTOMATION_COMMAND(FIsDoneRuntimeStatusTests(IndexerRequestsTestData, this));
    return true;
}