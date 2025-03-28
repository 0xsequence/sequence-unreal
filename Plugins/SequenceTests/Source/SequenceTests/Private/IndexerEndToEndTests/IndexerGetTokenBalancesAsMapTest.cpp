// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "CoreMinimal.h"
#include "Helpers/IndexerEndToEndTestsCommon.h"
#include "Misc/AutomationTest.h"
#include "Util/Async.h"
#include "Util/SequenceSupport.h"
#include "Helpers/IndexerRequestsTestData.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FIndexerGetTokenBalancesOrganizedInMapTest, "SequencePlugin.EndToEnd.IndexerTests.GetTokenBalancesOrganizedInMapTest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/* Latent command used to poll off main thread to see if our requests are done */
DEFINE_LATENT_AUTOMATION_COMMAND_TWO_PARAMETER(FIsDoneGetTokenBalancesOrganizedInMap, const UIndexerRequestsTestData *, IndexerRequestsTestData, FAutomationTestBase *, GetTokenBalancesOrganizedInMapTest);

bool FIsDoneGetTokenBalancesOrganizedInMap::Update()
{
    while(this->IndexerRequestsTestData->GetPendingRequests() > 0)
    {
        return false;
    }

    if (this->IndexerRequestsTestData->GetAllRequestsSuccessful())
    {
        GetTokenBalancesOrganizedInMapTest->AddInfo(TEXT("GetTokenBalancesOrganizedInMap request completed"));
    }
    else
    {
        GetTokenBalancesOrganizedInMapTest->AddError(FString::Printf(TEXT("GetTokenBalancesOrganizedInMap request failed")));
    }
    
    return true;
}

void FIndexerGetTokenBalancesOrganizedInMapTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
    OutBeautifiedNames.Add(TEXT("Indexer Get Token Balances Organized In Map Test"));
    OutTestCommands.Add(TEXT(""));
}

bool FIndexerGetTokenBalancesOrganizedInMapTest::RunTest(const FString& Parameters)
{
    const int64 PolygonNetworkId = MarketplaceEndToEndTestsCommon::PolygonNetworkId;
    const FString& TestAddress = MarketplaceEndToEndTestsCommon::TestAddress;
    UIndexerRequestsTestData * IndexerRequestsTestData = UIndexerRequestsTestData::Make(1);

    const TSuccessCallback<FSeqGetTokenBalancesReturn> GenericSuccess = [this, IndexerRequestsTestData](const FSeqGetTokenBalancesReturn& TokenBalances)
    {
        TestNotNull(TEXT("TokenBalances"), &TokenBalances);
        TestNotNull(TEXT("TokenBalances.page"), &TokenBalances.page);
        TestNotNull(TEXT("TokenBalances.balances"), &TokenBalances.balances);
        TestTrue(TEXT("TokenBalances.balances should not be empty"), TokenBalances.balances.Num() > 0);

        TMap<int64, FSeqTokenBalance> TokenBalanceMap = UIndexer::GetTokenBalancesAsMap(TokenBalances.balances);
        
        TestNotNull(TEXT("TokenBalanceMap"), &TokenBalanceMap);
        TestTrue(TEXT("TokenBalanceMap should not be empty"), TokenBalanceMap.Num() > 0);

        const FString Message = "GetTokenBalancesOrganizedInMap request succeeded";
        AddInfo(FString::Printf(TEXT("%s. Remaining Requests: %d"), *Message, IndexerRequestsTestData->DecrementPendingRequests()));
    };

    const FFailureCallback GenericFailure = [this, IndexerRequestsTestData](const FSequenceError& Error)
    {
        const FString Message = "Request Failure";
        AddError(FString::Printf(TEXT("%s: %s. Remaining requests: %d"), *Message, *Error.Message, IndexerRequestsTestData->DecrementPendingRequests()));
        IndexerRequestsTestData->RequestFailed();
    };

    AddInfo(FString::Printf(TEXT("Starting GetTokenBalancesOrganizedInMap request")));

    FSeqGetTokenBalancesArgs Args;
    Args.accountAddress = TestAddress;
    IndexerRequestsTestData->GetIndexer()->GetTokenBalances(PolygonNetworkId, Args, GenericSuccess, GenericFailure);
    
    ADD_LATENT_AUTOMATION_COMMAND(FIsDoneGetTokenBalancesOrganizedInMap(IndexerRequestsTestData, this));
    return true;
}
