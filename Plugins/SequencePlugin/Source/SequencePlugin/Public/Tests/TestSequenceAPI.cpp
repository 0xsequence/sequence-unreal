#include "TestSequenceAPI.h"

#include "Sequence/SequenceAPI.h"
#include "Authenticator.h"

void SequenceAPITest::RegisterSession(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
{
	const UAuthenticator * Auth = NewObject<UAuthenticator>();
	USequenceWallet * Api = USequenceWallet::Make(Auth->GetStoredCredentials().GetCredentials());

	const TFunction<void(FString)> OnResponse = [OnSuccess](FString Response)
	{
		OnSuccess("RegisterSession Test Passed");	
	};
	
	const FFailureCallback GenericFailure = [OnFailure](const FSequenceError& Error)
	{
		OnFailure("Test Failed", Error);
	};
	
	UE_LOG(LogTemp,Display,TEXT("========================[Running Sequence API RegisterSession Test]========================"));

	Api->RegisterSession(OnSuccess,GenericFailure);
}

void SequenceAPITest::SignMessage(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
{
	const UAuthenticator * Auth = NewObject<UAuthenticator>();
	USequenceWallet * Api = USequenceWallet::Make(Auth->GetStoredCredentials().GetCredentials());

	const TSuccessCallback<FSignedMessage> OnResponse = [OnSuccess] (FSignedMessage response)
	{
		OnSuccess("Sign Message Test Passed");
	};
	
	const FFailureCallback GenericFailure = [OnFailure](const FSequenceError& Error)
	{
		OnFailure("Sign Message Test Failed", Error);
	};
	
	UE_LOG(LogTemp,Display,TEXT("========================[Running Sequence API SignMessage Test]========================"));

	Api->SignMessage("hi",OnResponse,GenericFailure);
}

void SequenceAPITest::ListSessions(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
{
	const UAuthenticator * Auth = NewObject<UAuthenticator>();
	USequenceWallet * Api = USequenceWallet::Make(Auth->GetStoredCredentials().GetCredentials());

	const TSuccessCallback<TArray<FSession>> OnResponse = [OnSuccess](TArray<FSession> response)
	{
		OnSuccess("List Sessions Test Passed");
	};
	
	const FFailureCallback GenericFailure = [OnFailure](const FSequenceError& Error)
	{
		OnFailure("Test Failed", Error);
	};
	
	UE_LOG(LogTemp,Display,TEXT("========================[Running Sequence API ListSessions Test]========================"));

	Api->ListSessions(OnResponse,GenericFailure);
}

void SequenceAPITest::SendTransaction(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
{
	const UAuthenticator * Auth = NewObject<UAuthenticator>();
	USequenceWallet * Api = USequenceWallet::Make(Auth->GetStoredCredentials().GetCredentials());

	const FFailureCallback GenericFailure = [OnFailure](const FSequenceError& Error)
	{
		OnFailure("Test Failed", Error);
	};
	
	UE_LOG(LogTemp,Display,TEXT("========================[Running Sequence API SendTransaction Test]========================"));
	
	//TArray<FRawTransaction> Txn;
	//Api->SendTransaction(OnSuccess,GenericFailure);
}

void SequenceAPITest::CloseSession(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
{
	const UAuthenticator * Auth = NewObject<UAuthenticator>();
	USequenceWallet * Api = USequenceWallet::Make(Auth->GetStoredCredentials().GetCredentials());

	const TFunction<void(void)> OnResponse = [OnSuccess]()
	{
		OnSuccess("CloseSession Test Passed");
	};
	
	const FFailureCallback GenericFailure = [OnFailure](const FSequenceError& Error)
	{
		OnFailure("Test Failed", Error);
	};
	
	UE_LOG(LogTemp,Display,TEXT("========================[Running Sequence API CloseSession Test]========================"));

	Api->CloseSession(OnResponse,GenericFailure);
}