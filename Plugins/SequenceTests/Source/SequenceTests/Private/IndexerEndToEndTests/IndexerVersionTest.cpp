// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Util/Async.h"
#include "Util/SequenceSupport.h"
#include "IndexerEndToEndTests/Helpers/IndexerRequestsTestData.h"
#include "Helpers/BatchTestBuilder.h" // Include the BatchTestBuilder header
#include "Helpers/IndexerRequestsTestData.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FIndexerVersionTest, "SequencePlugin.EndToEnd.IndexerTests.VersionTest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/* Latent command used to poll off main thread to see if our requests are done */
DEFINE_LATENT_AUTOMATION_COMMAND_TWO_PARAMETER(FIsDoneVersionTests, const UIndexerRequestsTestData *, IndexerRequestsTestData, FAutomationTestBase *, VersionTest);

/* Latent command used to batch process requests w/o exceeding network threading limits */
DEFINE_LATENT_AUTOMATION_COMMAND_FIVE_PARAMETER(FProcessVersionRequestBatch, const int32, WatchIndex, const int32, FinishIndex, const UIndexerRequestsTestData *, IndexerRequestsTestData, const TSuccessCallback<FSeqVersion>, SuccessCallback, const FFailureCallback, FailureCallback);

bool FProcessVersionRequestBatch::Update()
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

bool FIsDoneVersionTests::Update()
{
    while(this->IndexerRequestsTestData->GetPendingRequests() > 0)
    {
        return false;
    }

    if (this->IndexerRequestsTestData->GetAllRequestsSuccessful())
    {
        VersionTest->AddInfo(TEXT("All version requests completed"));
    }
    else
    {
        VersionTest->AddError(FString::Printf(TEXT("Not all version requests returned successfully")));
    }
    
    return true;
}

void FIndexerVersionTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
    OutBeautifiedNames.Add(TEXT("Indexer Version Test"));
    OutTestCommands.Add(TEXT(""));
}

bool FIndexerVersionTest::RunTest(const FString& Parameters)
{
    const TArray<int64> Networks = USequenceSupport::GetAllNetworkIds();
    UIndexerRequestsTestData * IndexerRequestsTestData = UIndexerRequestsTestData::Make(Networks.Num());

    const TSuccessCallback<FSeqVersion> GenericSuccess = [this, IndexerRequestsTestData](const FSeqVersion& Version)
    {
        TestNotNull(TEXT("Version"), &Version);
        TestFalse(TEXT("AppVersion should not be empty"), Version.appVersion.IsEmpty());
        TestFalse(TEXT("SchemaVersion should not be empty"), Version.schemaVersion.IsEmpty());
        TestFalse(TEXT("WebRPCVersion should not be empty"), Version.webrpcVersion.IsEmpty());
        TestFalse(TEXT("SchemaHash should not be empty"), Version.schemaHash.IsEmpty());
        
        const FString Message = "Version request succeeded";
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
        ADD_LATENT_AUTOMATION_COMMAND(FProcessVersionRequestBatch(BatchTestBuilder.GetStartIndex(), BatchTestBuilder.GetEndIndex(), IndexerRequestsTestData, GenericSuccess, GenericFailure));
        BatchTestBuilder.BuildNextBatch();
    }
    
    ADD_LATENT_AUTOMATION_COMMAND(FIsDoneVersionTests(IndexerRequestsTestData, this));
    return true;
}