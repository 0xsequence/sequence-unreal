// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "CoreMinimal.h"
#include "Helpers/IndexerEndToEndTestsCommon.h"
#include "Misc/AutomationTest.h"
#include "Util/Async.h"
#include "Helpers/IndexerRequestsTestData.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FIndexerGetTransactionHistoryTest, "SequencePlugin.EndToEnd.IndexerTests.GetTransactionHistoryTest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/* Latent command used to poll off main thread to see if our requests are done */
DEFINE_LATENT_AUTOMATION_COMMAND_TWO_PARAMETER(FIsDoneGetTransactionHistory, const UIndexerRequestsTestData *, IndexerRequestsTestData, FAutomationTestBase *, GetTransactionHistoryTest);

bool FIsDoneGetTransactionHistory::Update()
{
    if (IndexerRequestsTestData->GetPendingRequests() == 0)
    {
        if (IndexerRequestsTestData->GetAllRequestsSuccessful())
        {
            GetTransactionHistoryTest->AddInfo(TEXT("GetTransactionHistory request completed successfully"));
        }
        else
        {
            GetTransactionHistoryTest->AddError(TEXT("GetTransactionHistory request failed"));
        }
        return true;
    }
    return false;
}

void FIndexerGetTransactionHistoryTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
    OutBeautifiedNames.Add(TEXT("Indexer Get Transaction History Test"));
    OutTestCommands.Add(TEXT(""));
}

bool FIndexerGetTransactionHistoryTest::RunTest(const FString& Parameters)
{
    const int64 PolygonNetworkId = IndexerEndToEndTestsCommon::PolygonNetworkId;
    const FString TestAddress = IndexerEndToEndTestsCommon::TestAddress;
    UIndexerRequestsTestData* IndexerRequestsTestData = UIndexerRequestsTestData::Make(1);

    const TSuccessCallback<FSeqGetTransactionHistoryReturn> GenericSuccess = [this, IndexerRequestsTestData](const FSeqGetTransactionHistoryReturn& History)
    {
        TestNotNull(TEXT("History"), &History);
        TestNotNull(TEXT("History.page"), &History.page);
        TestNotEqual(TEXT("Page Size should not be -1"), History.page.pageSize, -1);
        TestNotNull(TEXT("History.transactions"), &History.transactions);
        TestTrue(TEXT("History.transactions should not be empty"), History.transactions.Num() > 0);
        TestNotNull(TEXT("History.transactions transactions contains transaction hash"), &History.transactions[0].txnHash);


        const FString Message = "GetTransactionHistory request succeeded";
        AddInfo(FString::Printf(TEXT("%s. Remaining Requests: %d"), *Message, IndexerRequestsTestData->DecrementPendingRequests()));
    };

    const FFailureCallback GenericFailure = [this, IndexerRequestsTestData](const FSequenceError& Error)
    {
        const FString Message = "GetTransactionHistory Request Failure";
        AddError(FString::Printf(TEXT("%s: %s. Remaining requests: %d"), *Message, *Error.Message, IndexerRequestsTestData->DecrementPendingRequests()));
        IndexerRequestsTestData->RequestFailed();
    };

    AddInfo(TEXT("Starting GetTransactionHistory request"));

    FSeqGetTransactionHistoryArgs Args;
    Args.filter.accountAddress = TestAddress;
    IndexerRequestsTestData->GetIndexer()->GetTransactionHistory(PolygonNetworkId, Args, GenericSuccess, GenericFailure);
    
    ADD_LATENT_AUTOMATION_COMMAND(FIsDoneGetTransactionHistory(IndexerRequestsTestData, this));
    return true;
}