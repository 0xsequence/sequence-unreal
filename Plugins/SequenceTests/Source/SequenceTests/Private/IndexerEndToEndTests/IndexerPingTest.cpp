// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "Util/Async.h"
#include "Util/SequenceSupport.h"
#include "Engine/World.h"
#include "SequencePlugin/Private/Indexer/Indexer.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FIndexerPingTest, "SequencePlugin.EndToEnd.IndexerTests.PingTest",
                                  EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

void FIndexerPingTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
    OutBeautifiedNames.Add(TEXT("Indexer Ping Test"));
    OutTestCommands.Add(TEXT(""));
}

bool FIndexerPingTest::RunTest(const FString& Parameters)
{
    UIndexer* Indexer = NewObject<UIndexer>();
    
    int32 PendingPings = 0;
    bool bAllPingsSuccessful = true;

    auto OnSuccess = [this, &PendingPings, &bAllPingsSuccessful](const FString& Message)
    {
        AddInfo(FString::Printf(TEXT("%s. Remaining pings: %d"), *Message, PendingPings - 1));
        --PendingPings;
    };

    auto OnFailure = [this, &PendingPings, &bAllPingsSuccessful](const FString& Message, const FSequenceError& Error)
    {
        AddError(FString::Printf(TEXT("%s: %s. Remaining pings: %d"), *Message, *Error.Message, PendingPings - 1));
        bAllPingsSuccessful = false;
        --PendingPings;
    };

    const TSuccessCallback<bool> GenericSuccess = [OnSuccess](const bool bSuccess)
    {
        OnSuccess(bSuccess ? "Ping Success" : "Ping Failure");
    };

    const FFailureCallback GenericFailure = [OnFailure](const FSequenceError& Error)
    {
        OnFailure("Ping Failure", Error);
    };

    TArray<int64> ids = USequenceSupport::GetAllNetworkIds();
    PendingPings = ids.Num();

    AddInfo(FString::Printf(TEXT("Starting %d pings"), PendingPings));

    for (const int64& id : ids)
    {
        Indexer->Ping(id, GenericSuccess, GenericFailure);
    }

    // Add a latent command to wait for all pings to complete or timeout
    ADD_LATENT_AUTOMATION_COMMAND(FEngineWaitLatentCommand(30.0f));
    ADD_LATENT_AUTOMATION_COMMAND(FFunctionLatentCommand([this, &PendingPings, &bAllPingsSuccessful]()
    {
        if (PendingPings == 0)
        {
            AddInfo(TEXT("All pings completed"));
            return true;
        }
        else
        {
            AddError(FString::Printf(TEXT("Test timed out. %d pings did not complete."), PendingPings));
            return true;
        }
    }));

    return true;
}