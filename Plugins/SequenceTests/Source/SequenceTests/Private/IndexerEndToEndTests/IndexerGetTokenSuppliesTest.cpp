// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "CoreMinimal.h"
#include "Helpers/IndexerEndToEndTestsCommon.h"
#include "Misc/AutomationTest.h"
#include "Util/Async.h"
#include "Util/SequenceSupport.h"
#include "Helpers/IndexerRequestsTestData.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FIndexerGetTokenSuppliesTest, "SequencePlugin.EndToEnd.IndexerTests.GetTokenSuppliesTest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/* Latent command used to poll off main thread to see if our requests are done */
DEFINE_LATENT_AUTOMATION_COMMAND_TWO_PARAMETER(FIsDoneGetTokenSupplies, const UIndexerRequestsTestData *, IndexerRequestsTestData, FAutomationTestBase *, GetTokenSuppliesTest);

bool FIsDoneGetTokenSupplies::Update()
{
    while(this->IndexerRequestsTestData->GetPendingRequests() > 0)
    {
        return false;
    }

    if (this->IndexerRequestsTestData->GetAllRequestsSuccessful())
    {
        GetTokenSuppliesTest->AddInfo(TEXT("GetTokenSupplies request completed"));
    }
    else
    {
        GetTokenSuppliesTest->AddError(FString::Printf(TEXT("GetTokenSupplies request failed")));
    }
    
    return true;
}

void FIndexerGetTokenSuppliesTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
    OutBeautifiedNames.Add(TEXT("Indexer Get Token Supplies Test"));
    OutTestCommands.Add(TEXT(""));
}

bool FIndexerGetTokenSuppliesTest::RunTest(const FString& Parameters)
{
    const int64 PolygonNetworkId = IndexerEndToEndTestsCommon::PolygonNetworkId;
    const FString TestAddress = IndexerEndToEndTestsCommon::TestContractAddress_Skyweaver;
    UIndexerRequestsTestData * IndexerRequestsTestData = UIndexerRequestsTestData::Make(1);

    const TSuccessCallback<FSeqGetTokenSuppliesReturn> GenericSuccess = [this, IndexerRequestsTestData](const FSeqGetTokenSuppliesReturn& TokenSupplies)
    {
        TestNotNull(TEXT("TokenSupplies"), &TokenSupplies);
        TestNotNull(TEXT("TokenSupplies.page"), &TokenSupplies.page);
        TestEqual(TEXT("Contract type should be ERC1155"), TokenSupplies.contractType, ERC1155);
        TestNotNull(TEXT("TokenSupplies.tokenIDs"), &TokenSupplies.tokenIDs);
        TestTrue(TEXT("TokenSupplies.tokenIDs should not be empty"), TokenSupplies.tokenIDs.Num() > 0);

        const FString Message = "GetTokenSupplies request succeeded";
        AddInfo(FString::Printf(TEXT("%s. Remaining Requests: %d"), *Message, IndexerRequestsTestData->DecrementPendingRequests()));
    };

    const FFailureCallback GenericFailure = [this, IndexerRequestsTestData](const FSequenceError& Error)
    {
        const FString Message = "Request Failure";
        AddError(FString::Printf(TEXT("%s: %s. Remaining requests: %d"), *Message, *Error.Message, IndexerRequestsTestData->DecrementPendingRequests()));
        IndexerRequestsTestData->RequestFailed();
    };

    AddInfo(FString::Printf(TEXT("Starting GetTokenSupplies request")));

    FSeqGetTokenSuppliesArgs Args;
    Args.contractAddress = TestAddress;
    IndexerRequestsTestData->GetIndexer()->GetTokenSupplies(PolygonNetworkId, Args, GenericSuccess, GenericFailure);
    
    ADD_LATENT_AUTOMATION_COMMAND(FIsDoneGetTokenSupplies(IndexerRequestsTestData, this));
    return true;
}