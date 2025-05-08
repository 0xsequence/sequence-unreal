

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Util/Async.h"
#include "Util/SequenceSupport.h"
#include "IndexerEndToEndTests/Helpers/IndexerRequestsTestData.h"
#include "Helpers/BatchTestBuilder.h" // Include the BatchTestBuilder header
#include "Helpers/IndexerRequestsTestData.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FIndexerPingTest, "SequencePlugin.EndToEnd.IndexerTests.PingTest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/* Latent command used to poll off main thread to see if our pings are done */
DEFINE_LATENT_AUTOMATION_COMMAND_THREE_PARAMETER(FIsDone, const UIndexerRequestsTestData *, IndexerRequestsTestData, FAutomationTestBase *, PingTest, const TArray<int64>*, FailedNetworks);

/* Latent command used to batch process pings w/o exceeding network threading limits */
DEFINE_LATENT_AUTOMATION_COMMAND_FIVE_PARAMETER(FProcessPingBatch, const int32, WatchIndex, const int32, FinishIndex, const UIndexerRequestsTestData *, IndexerRequestsTestData, const TSuccessCallback<bool>, SuccessCallback, const FFailureCallback, FailureCallback);

bool FProcessPingBatch::Update()
{
    while(IndexerRequestsTestData->GetRequestsComplete() < WatchIndex)
    {
        return false;
    }

    const TArray<int64> Networks = USequenceSupport::GetAllNetworkIds();
    
    for (int i = WatchIndex; i <= FinishIndex; i++)
    {
        UE_LOG(LogTemp, Display, TEXT("Index: %d, Pinging Network: %lld"), i, Networks[i]);
        IndexerRequestsTestData->GetIndexer()->Ping(Networks[i], SuccessCallback, FailureCallback);
    }
    
    return true;
}

bool FIsDone::Update()
{
    while(this->IndexerRequestsTestData->GetPendingRequests() > 0)
    {
        return false;
    }

    if (this->IndexerRequestsTestData->GetAllRequestsSuccessful())
    {
        PingTest->AddInfo(TEXT("All pings completed"));
    }
    else
    {
        FString FailedNetworksStr;
        for (const int64& NetworkId : *FailedNetworks)
        {
            const FString NetworkName = USequenceSupport::GetNetworkName(NetworkId);
            FailedNetworksStr += FString::Printf(TEXT("%s (%lld), "), *NetworkName, NetworkId);
        }
        PingTest->AddError(FString::Printf(TEXT("Ping failed for networks: %s"), *FailedNetworksStr));
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
    UIndexerRequestsTestData* IndexerRequestsTestData = UIndexerRequestsTestData::Make(Networks.Num());
    
    // Create a shared pointer to track failed networks
    TArray<int64>* FailedNetworks = new TArray<int64>();

    const TSuccessCallback<bool> GenericSuccess = [this, IndexerRequestsTestData](const bool bSuccess)
    {
        const FString Message = (bSuccess) ? "Ping Success" : "Ping Failure";
        AddInfo(FString::Printf(TEXT("%s. Remaining pings: %d"), *Message, IndexerRequestsTestData->DecrementPendingRequests()));
    };

    const FFailureCallback GenericFailure = [this, IndexerRequestsTestData, Networks, FailedNetworks](const FSequenceError& Error)
    {
        // Track which network failed
        const int32 CurrentIndex = Networks.Num() - IndexerRequestsTestData->GetPendingRequests();
        FailedNetworks->Add(Networks[CurrentIndex]);
        
        const FString Message = "Ping Failure";
        AddError(FString::Printf(TEXT("%s for network %lld: %s. Remaining pings: %d"), 
            *Message, 
            Networks[CurrentIndex],
            *Error.Message, 
            IndexerRequestsTestData->DecrementPendingRequests()));
        IndexerRequestsTestData->RequestFailed();
    };

    AddInfo(FString::Printf(TEXT("Starting %d pings"), IndexerRequestsTestData->GetPendingRequests()));

    constexpr int32 BatchSize = 1;
    FBatchTestBuilder BatchTestBuilder(BatchSize, Networks.Num());

    while (BatchTestBuilder.CanBuildBatch())
    {
        ADD_LATENT_AUTOMATION_COMMAND(FProcessPingBatch(BatchTestBuilder.GetStartIndex(), BatchTestBuilder.GetEndIndex(), IndexerRequestsTestData, GenericSuccess, GenericFailure));
        BatchTestBuilder.BuildNextBatch();
    }
    
    ADD_LATENT_AUTOMATION_COMMAND(FIsDone(IndexerRequestsTestData, this, FailedNetworks));
    return true;
}