// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "Util/Async.h"
#include "Util/SequenceSupport.h"
#include "Engine/World.h"
#include "IndexerPingTestData.h"
#include "SequencePlugin/Private/Indexer/Indexer.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FIndexerPingTest, "SequencePlugin.EndToEnd.IndexerTests.PingTest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

void FIndexerPingTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
    OutBeautifiedNames.Add(TEXT("Indexer Ping Test"));
    OutTestCommands.Add(TEXT(""));
}

bool FIndexerPingTest::RunTest(const FString& Parameters)
{
    UIndexer* Indexer = NewObject<UIndexer>();
    const TArray<int64> Networks = USequenceSupport::GetAllNetworkIds();
    UIndexerPingTestData * IndexerPingTestData = UIndexerPingTestData::Make(Networks.Num());

    const TFunction<bool(void)> OnDone = [this, IndexerPingTestData]()
    {
        if (IndexerPingTestData->GetPendingPings() == 0)
        {
            AddInfo(TEXT("All pings completed"));
            return true;
        }
        else
        {
            AddError(FString::Printf(TEXT("Test timed out. %d pings did not complete."), IndexerPingTestData->GetPendingPings()));
            return true;
        }
    };

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
    
    for (const int64& Id : Networks)
    {
        Indexer->Ping(Id, GenericSuccess, GenericFailure);
    }
    
    ADD_LATENT_AUTOMATION_COMMAND(FEngineWaitLatentCommand(10.0f));
    
    // Add a latent command to wait for all pings to complete or timeout
    ADD_LATENT_AUTOMATION_COMMAND(FEngineWaitLatentCommand(30.0f));
    
    ADD_LATENT_AUTOMATION_COMMAND(FFunctionLatentCommand([this, IndexerPingTestData]()
    {
        if (IndexerPingTestData->GetPendingPings() == 0)
        {
            AddInfo(TEXT("All pings completed"));
            return true;
        }
        else
        {
            AddError(FString::Printf(TEXT("Test timed out. %d pings did not complete."), IndexerPingTestData->GetPendingPings()));
            return true;
        }
    }));

    return true;
}