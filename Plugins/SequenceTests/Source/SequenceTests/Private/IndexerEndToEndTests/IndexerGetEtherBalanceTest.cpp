// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Util/Async.h"
#include "Util/SequenceSupport.h"
#include "Helpers/IndexerRequestsTestData.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FIndexerGetEtherBalanceTest, "SequencePlugin.EndToEnd.IndexerTests.GetEtherBalanceTest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/* Latent command used to poll off main thread to see if our requests are done */
DEFINE_LATENT_AUTOMATION_COMMAND_TWO_PARAMETER(FIsDoneGetEtherBalance, const UIndexerRequestsTestData *, IndexerRequestsTestData, FAutomationTestBase *, GetEtherBalanceTest);

bool FIsDoneGetEtherBalance::Update()
{
    while(this->IndexerRequestsTestData->GetPendingRequests() > 0)
    {
        return false;
    }

    if (this->IndexerRequestsTestData->GetAllRequestsSuccessful())
    {
        GetEtherBalanceTest->AddInfo(TEXT("GetEtherBalance request completed"));
    }
    else
    {
        GetEtherBalanceTest->AddError(FString::Printf(TEXT("GetEtherBalance request failed")));
    }
    
    return true;
}

void FIndexerGetEtherBalanceTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
    OutBeautifiedNames.Add(TEXT("Indexer Get Ether Balance Test"));
    OutTestCommands.Add(TEXT(""));
}

bool FIndexerGetEtherBalanceTest::RunTest(const FString& Parameters)
{
    const int64 PolygonNetworkId = 137; // Polygon network ID
    const FString TestAddress = TEXT("0x8e3E38fe7367dd3b52D1e281E4e8400447C8d8B9");
    UIndexerRequestsTestData * IndexerRequestsTestData = UIndexerRequestsTestData::Make(1);

    const TSuccessCallback<FSeqEtherBalance> GenericSuccess = [this, IndexerRequestsTestData, TestAddress](const FSeqEtherBalance& Balance)
    {
        TestNotNull(TEXT("EtherBalance"), &Balance);
        TestEqual(TEXT("Account address should match"), Balance.accountAddress.ToLower(), TestAddress.ToLower());
        TestTrue(TEXT("Balance should be greater than 0"), Balance.balanceWei > 0);

        const FString Message = "GetEtherBalance request succeeded";
        AddInfo(FString::Printf(TEXT("%s. Remaining Requests: %d"), *Message, IndexerRequestsTestData->DecrementPendingRequests()));
    };

    const FFailureCallback GenericFailure = [this, IndexerRequestsTestData](const FSequenceError& Error)
    {
        const FString Message = "Request Failure";
        AddError(FString::Printf(TEXT("%s: %s. Remaining requests: %d"), *Message, *Error.Message, IndexerRequestsTestData->DecrementPendingRequests()));
        IndexerRequestsTestData->RequestFailed();
    };

    AddInfo(FString::Printf(TEXT("Starting GetEtherBalance request")));

    IndexerRequestsTestData->GetIndexer()->GetEtherBalance(PolygonNetworkId, TestAddress, GenericSuccess, GenericFailure);
    
    ADD_LATENT_AUTOMATION_COMMAND(FIsDoneGetEtherBalance(IndexerRequestsTestData, this));
    return true;
}