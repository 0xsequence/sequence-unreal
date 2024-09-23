// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Util/Async.h"
#include "Util/SequenceSupport.h"
#include "IndexerEndToEndTests/Helpers/IndexerRequestsTestData.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FIndexerVersionTest, "SequencePlugin.EndToEnd.IndexerTests.VersionTest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/* Latent command used to poll off main thread to see if our requests are done */
DEFINE_LATENT_AUTOMATION_COMMAND_TWO_PARAMETER(FIsDoneRequests, const UIndexerRequestsTestData *, IndexerRequestsTestData, FAutomationTestBase *, VersionTest);

/* Latent command used to batch process requests w/o exceeding network threading limits */
DEFINE_LATENT_AUTOMATION_COMMAND_FIVE_PARAMETER(FProcessRequestBatch, const int32, WatchIndex, const int32, FinishIndex, const UIndexerRequestsTestData *, IndexerRequestsTestData, const TSuccessCallback<FSeqVersion>, SuccessCallback, const FFailureCallback, FailureCallback);

bool FProcessRequestBatch::Update()
{
    while(IndexerRequestsTestData->GetRequestsComplete() < WatchIndex)
    {
        return false;
    }

    const TArray<int64> Networks = USequenceSupport::GetAllNetworkIds();
    
    for (int i = WatchIndex; i <= FinishIndex; i++)
    {
        UE_LOG(LogTemp, Display, TEXT("Index: %d, Testing Network: %lld"), i, Networks[i]);
        IndexerRequestsTestData->GetIndexer()->Version(Networks[i], SuccessCallback, FailureCallback);
    }
    
    return true;
}

bool FIsDoneRequests::Update()
{
    while(this->IndexerRequestsTestData->GetPendingRequests() > 0)
    {
        return false;
    }

    if (this->IndexerRequestsTestData->GetAllRequestsSuccessful())
    {
        VersionTest->AddInfo(TEXT("All pings completed"));
    }
    else
    {
        VersionTest->AddError(FString::Printf(TEXT("Not all pings returned successfully")));
    }
    
    return true;
}

void FIndexerVersionTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
    OutBeautifiedNames.Add(TEXT("Indexer Ping Test"));
    OutTestCommands.Add(TEXT(""));
}

bool FIndexerVersionTest::RunTest(const FString& Parameters)
{
    const TArray<int64> Networks = USequenceSupport::GetAllNetworkIds();
    UIndexerRequestsTestData * IndexerRequestTestData = UIndexerRequestsTestData::Make(Networks.Num());

    const TSuccessCallback<FSeqVersion> GenericSuccess = [this, IndexerRequestTestData](const FSeqVersion version)
    {
        TestNotNull(TEXT("Version"), &version);
        TestFalse(TEXT("AppVersion should not be empty, given " + version.appVersion), version.appVersion.IsEmpty());
        TestFalse(TEXT("SchemaVersion should not be empty, given " + version.schemaVersion), version.schemaVersion.IsEmpty());
        TestFalse(TEXT("WebRPCVersion should not be empty, given " + version.webrpcVersion), version.webrpcVersion.IsEmpty());
        TestFalse(TEXT("SchemaHash should not be empty, given " + version.schemaHash), version.schemaHash.IsEmpty());
        
        const FString Message = "Version request succeeded";
        AddInfo(FString::Printf(TEXT("%s. Remaining Requests: %d"), *Message, IndexerRequestTestData->DecrementPendingRequests()));
    };

    const FFailureCallback GenericFailure = [this, IndexerRequestTestData](const FSequenceError& Error)
    {
        const FString Message = "Request Failure";
        AddError(FString::Printf(TEXT("%s: %s. Remaining requests: %d"), *Message, *Error.Message, IndexerRequestTestData->DecrementPendingRequests()));
        IndexerRequestTestData->RequestFailed();
    };

    AddInfo(FString::Printf(TEXT("Starting %d requests"), IndexerRequestTestData->GetPendingRequests()));

    constexpr int32 BatchSize = 5;
    int32 StartIndex = 0;
    int32 EndIndex = FMath::Min(BatchSize-1, Networks.Num() - 1);

    while (StartIndex < Networks.Num() - 1)
    {
        ADD_LATENT_AUTOMATION_COMMAND(FProcessRequestBatch(StartIndex, EndIndex, IndexerRequestTestData, GenericSuccess, GenericFailure));
        StartIndex += BatchSize;
        EndIndex = FMath::Min((StartIndex + BatchSize - 1), Networks.Num() - 1);
    }
    
    ADD_LATENT_AUTOMATION_COMMAND(FIsDoneRequests(IndexerRequestTestData, this));
    return true;
}