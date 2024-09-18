// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Util/Async.h"
#include "Util/SequenceSupport.h"
#include "Engine/World.h"
#include "IndexerPingTestData.h"
#include "SequencePlugin/Private/Indexer/Indexer.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FIndexerPingTest, "SequencePlugin.EndToEnd.IndexerTests.PingTest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/* Latent command used to poll off main thread to see if our pings are done */
DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FIsDone, const UIndexerPingTestData *, IndexerPingTestData);

/* Latent command used to batch process pings w/o exceeding network threading limits */
DEFINE_LATENT_AUTOMATION_COMMAND_FIVE_PARAMETER(FProcessPingBatch, const int32, WatchIndex, const int32, FinishIndex, const UIndexerPingTestData *, IndexerPingTestData, const TSuccessCallback<bool>, SuccessCallback, const FFailureCallback, FailureCallback);

bool FProcessPingBatch::Update()
{
    while(IndexerPingTestData->GetPingsComplete() < WatchIndex)
    {
        return false;
    }

    const TArray<int64> Networks = USequenceSupport::GetAllNetworkIds();
    
    for (int i = WatchIndex; i <= FinishIndex; i++)
    {
        UE_LOG(LogTemp, Display, TEXT("Index: %d, Pinging Network: %lld"), i, Networks[i]);
        IndexerPingTestData->GetIndexer()->Ping(Networks[i], SuccessCallback, FailureCallback);
    }
    
    return true;
}

bool FIsDone::Update()
{
    while(this->IndexerPingTestData->GetPendingPings() > 0)
    {
        return false;
    }
    
    return true;
}

void FIndexerPingTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
    OutBeautifiedNames.Add(TEXT("Indexer Ping Test"));
    OutTestCommands.Add(TEXT(""));
}

bool FIndexerPingTest::RunTest(const FString& Parameters)
{
    const TArray<int64> Networks = USequenceSupport::GetAllNetworkIds();
    UIndexerPingTestData * IndexerPingTestData = UIndexerPingTestData::Make(Networks.Num());

    const TSuccessCallback<bool> GenericSuccess = [this, IndexerPingTestData](const bool bSuccess)
    {
        const FString Message = (bSuccess) ? "Ping Success" : "Ping Failure";
        AddInfo(FString::Printf(TEXT("%s. Remaining pings: %d"), *Message, IndexerPingTestData->DecrementPendingPings()));
    };

    const FFailureCallback GenericFailure = [this, IndexerPingTestData](const FSequenceError& Error)
    {
        const FString Message = "Ping Failure";
        AddError(FString::Printf(TEXT("%s: %s. Remaining pings: %d"), *Message, *Error.Message, IndexerPingTestData->DecrementPendingPings()));
        IndexerPingTestData->PingFailed();
    };

    AddInfo(FString::Printf(TEXT("Starting %d pings"), IndexerPingTestData->GetPendingPings()));

    constexpr int32 BatchSize = 5;
    int32 StartIndex = 0;
    int32 EndIndex = FMath::Min(BatchSize-1, Networks.Num() - 1);

    while (StartIndex < Networks.Num() - 1)
    {
        ADD_LATENT_AUTOMATION_COMMAND(FProcessPingBatch(StartIndex, EndIndex, IndexerPingTestData, GenericSuccess, GenericFailure));
        StartIndex += BatchSize;
        EndIndex = FMath::Min((StartIndex + BatchSize - 1), Networks.Num() - 1);
    }
    
    ADD_LATENT_AUTOMATION_COMMAND(FIsDone(IndexerPingTestData));
    return true;
}