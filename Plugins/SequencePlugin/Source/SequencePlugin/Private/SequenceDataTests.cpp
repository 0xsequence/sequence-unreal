
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

void TestSequenceData(TFunction<void(FString)> OnSuccess, TFunction<void(FString, SequenceError)> OnFailure)
{
	USequenceData* sequence = NewObject<USequenceData>();
	
	TestGetFriends(sequence, OnSuccess, OnFailure);
}