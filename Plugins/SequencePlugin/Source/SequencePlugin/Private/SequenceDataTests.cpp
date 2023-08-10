
#include "SequenceDataTests.h"

void TestGetFriends(USequenceData* sequence, TFunction<void(FString)> OnSuccess, TFunction<void(FString, SequenceError)> OnFailure)
{
	const TSuccessCallback<TArray<FContact_BE>> GenericSuccess = [OnSuccess](const TArray<FContact_BE> contactList)
	{
		OnSuccess("Get Friend Success");
	};

	const FFailureCallback GenericFailure = [OnFailure](const SequenceError Error)
	{
		OnFailure("Get Friend Failure", Error);
	};
	sequence->getFriends(testingAddress,GenericSuccess,GenericFailure);
}

void testGetCoins(USequenceData* sequence, TFunction<void(FString)> OnSuccess, TFunction<void(FString, SequenceError)> OnFailure)
{
	const TSuccessCallback<TArray<FItemPrice_BE>> GenericSuccess = [OnSuccess](const TArray<FItemPrice_BE> contactList)
	{
		OnSuccess("Get Coin Success");
	};

	const FFailureCallback GenericFailure = [OnFailure](const SequenceError Error)
	{
		OnFailure("Get Get Coin Failure", Error);
	};
	FID_BE args;
	args.chainID = 137;
	args.contractAddress = "0xeac222c7f2456b393621fa3f2eff9e410832e8f0";
	sequence->getUpdatedItemPrice(args, GenericSuccess, GenericFailure);
}

void TestSequenceData(TFunction<void(FString)> OnSuccess, TFunction<void(FString, SequenceError)> OnFailure)
{
	USequenceData* sequence = NewObject<USequenceData>();
	
	TestGetFriends(sequence, OnSuccess, OnFailure);
	testGetCoins(sequence, OnSuccess, OnFailure);
}