#include "CoreMinimal.h"
#include "Helpers/IndexerEndToEndTestsCommon.h"
#include "Misc/AutomationTest.h"
#include "Util/Async.h"
#include "Util/SequenceSupport.h"
#include "Helpers/IndexerRequestsTestData.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FIndexerGetTokenSuppliesMapTest, "SequencePlugin.EndToEnd.IndexerTests.GetTokenSuppliesMapTest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

DEFINE_LATENT_AUTOMATION_COMMAND_TWO_PARAMETER(FIsDoneGetTokenSuppliesMap, const UIndexerRequestsTestData *, IndexerRequestsTestData, FAutomationTestBase *, GetTokenSuppliesMapTest);

bool FIsDoneGetTokenSuppliesMap::Update()
{
    if (IndexerRequestsTestData->GetPendingRequests() == 0)
    {
        if (IndexerRequestsTestData->GetAllRequestsSuccessful())
        {
            GetTokenSuppliesMapTest->AddInfo(TEXT("GetTokenSuppliesMap request completed"));
        }
        else
        {
            GetTokenSuppliesMapTest->AddError(TEXT("GetTokenSuppliesMap request failed"));
        }
        return true;
    }
    return false;
}

void FIndexerGetTokenSuppliesMapTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
    OutBeautifiedNames.Add(TEXT("Indexer Get Token Supplies Map Test"));
    OutTestCommands.Add(TEXT(""));
}

bool FIndexerGetTokenSuppliesMapTest::RunTest(const FString& Parameters)
{
    const int64 PolygonNetworkId = IndexerEndToEndTestsCommon::PolygonNetworkId;
    UIndexerRequestsTestData* IndexerRequestsTestData = UIndexerRequestsTestData::Make(1);

    const FString UsdcAddress = IndexerEndToEndTestsCommon::TestContractAddress_USDC;
    const FString WmaticAddress = IndexerEndToEndTestsCommon::TestContractAddress_WMATIC;
    const FString SkyweaverAddress = IndexerEndToEndTestsCommon::TestContractAddress_Skyweaver;
    const FString SkyweaverTokenId1 = "68657";
    const FString SkyweaverTokenId2 = "66669";
    const FString SkyweaverTokenId3 = "66668";

    const TSuccessCallback<FSeqGetTokenSuppliesMapReturn> GenericSuccess = [this, IndexerRequestsTestData, UsdcAddress, WmaticAddress, SkyweaverAddress, SkyweaverTokenId1, SkyweaverTokenId2, SkyweaverTokenId3](const FSeqGetTokenSuppliesMapReturn& SuppliesMap)
    {
        TestNotNull(TEXT("SuppliesMap"), &SuppliesMap);
        TestNotNull(TEXT("SuppliesMap.supplies"), &SuppliesMap.supplies);
        TestEqual(TEXT("SuppliesMap.supplies count"), SuppliesMap.supplies.Num(), 3);
        TestTrue(TEXT("SuppliesMap contains USDC"), SuppliesMap.supplies.Contains(UsdcAddress));
        TestTrue(TEXT("SuppliesMap contains WMATIC"), SuppliesMap.supplies.Contains(WmaticAddress));
        TestTrue(TEXT("SuppliesMap contains Skyweaver"), SuppliesMap.supplies.Contains(SkyweaverAddress));

        const FSeqTokenSupplyList* UsdcSupplies = SuppliesMap.supplies.Find(UsdcAddress);
        const FSeqTokenSupplyList* WmaticSupplies = SuppliesMap.supplies.Find(WmaticAddress);
        const FSeqTokenSupplyList* SkyweaverSupplies = SuppliesMap.supplies.Find(SkyweaverAddress);

        TestNotNull(TEXT("USDC Supplies"), UsdcSupplies);
        TestNotNull(TEXT("WMATIC Supplies"), WmaticSupplies);
        TestNotNull(TEXT("Skyweaver Supplies"), SkyweaverSupplies);

        TestEqual(TEXT("USDC Supplies count"), UsdcSupplies->token_supply_list.Num(), 1);
        TestEqual(TEXT("WMATIC Supplies count"), WmaticSupplies->token_supply_list.Num(), 1);
        TestEqual(TEXT("Skyweaver Supplies count"), SkyweaverSupplies->token_supply_list.Num(), 3);

        TestTrue(TEXT("USDC supply length > 0"), UsdcSupplies->token_supply_list[0].supply.Len() > 0);
        TestTrue(TEXT("WMATIC supply length > 0"), WmaticSupplies->token_supply_list[0].supply.Len() > 0);
        TestTrue(TEXT("Skyweaver supply 1 length > 0"), SkyweaverSupplies->token_supply_list[0].supply.Len() > 0);
        TestTrue(TEXT("Skyweaver supply 2 length > 0"), SkyweaverSupplies->token_supply_list[1].supply.Len() > 0);
        TestTrue(TEXT("Skyweaver supply 3 length > 0"), SkyweaverSupplies->token_supply_list[2].supply.Len() > 0);

        const FString Message = "GetTokenSuppliesMap request succeeded";
        AddInfo(FString::Printf(TEXT("%s. Remaining Requests: %d"), *Message, IndexerRequestsTestData->DecrementPendingRequests()));
    };

    const FFailureCallback GenericFailure = [this, IndexerRequestsTestData](const FSequenceError& Error)
    {
        const FString Message = "GetTokenSuppliesMap Request Failure";
        AddError(FString::Printf(TEXT("%s: %s. Remaining requests: %d"), *Message, *Error.Message, IndexerRequestsTestData->DecrementPendingRequests()));
        IndexerRequestsTestData->RequestFailed();
    };

    AddInfo(TEXT("Starting GetTokenSuppliesMap request"));

    FSeqGetTokenSuppliesMapArgs Args;
    Args.tokenMap.Add(UsdcAddress, FSeqTokenList());
    Args.tokenMap.Add(WmaticAddress, FSeqTokenList("0"));
    Args.tokenMap.Add(SkyweaverAddress, FSeqTokenList(SkyweaverTokenId1, SkyweaverTokenId2, SkyweaverTokenId3));

    IndexerRequestsTestData->GetIndexer()->GetTokenSuppliesMap(PolygonNetworkId, Args, GenericSuccess, GenericFailure);
    
    ADD_LATENT_AUTOMATION_COMMAND(FIsDoneGetTokenSuppliesMap(IndexerRequestsTestData, this));
    return true;
}