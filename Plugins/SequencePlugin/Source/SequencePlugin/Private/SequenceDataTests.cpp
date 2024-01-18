
#include "SequenceDataTests.h"
#include "Indexer/IndexerTests.h"
#include "Sequence/SequenceAPI.h"
#include "Util/Structs/BE_Structs.h"

//No longer in V1
//void TestGetFriends(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
//{
//	const TSuccessCallback<TArray<FContact_BE>> GenericSuccess = [OnSuccess](const TArray<FContact_BE> contactList)
//	{
//		OnSuccess("Get Friend Success");
//	};
//
//	const FFailureCallback GenericFailure = [OnFailure](const FSequenceError Error)
//	{
//		OnFailure("Get Friend Failure", Error);
//	};
//
//	//because of the way this code was structured we can't rely on unreal GC for this
//	SequenceAPI::FSequenceWallet* lclWallet = new SequenceAPI::FSequenceWallet();
//
//	//lclWallet->getFriends(testingAddress, GenericSuccess, GenericFailure);
//
//	delete lclWallet;
//}
//
//void testGetCoins(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
//{
//	const TSuccessCallback<TArray<FItemPrice_BE>> GenericSuccess = [OnSuccess](const TArray<FItemPrice_BE> contactList)
//	{
//		OnSuccess("Get Coin Success");
//	};
//
//	const FFailureCallback GenericFailure = [OnFailure](const FSequenceError Error)
//	{
//		OnFailure("Get Get Coin Failure", Error);
//	};
//	FID_BE args;
//	args.chainID = 137;
//	args.contractAddress = "0xeac222c7f2456b393621fa3f2eff9e410832e8f0";
//
//	//because of the way this code was structured we can't rely on unreal GC for this
//	SequenceAPI::FSequenceWallet* lclWallet = new SequenceAPI::FSequenceWallet();
//
//	//lclWallet->getUpdatedCoinPrice(args, GenericSuccess, GenericFailure);
//
//	delete lclWallet;
//}

void TestSequenceData(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
{	
	//TestGetFriends(OnSuccess, OnFailure);
	//testGetCoins(OnSuccess, OnFailure);
}