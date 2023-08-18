// Fill out your copyright notice in the Description page of Project Settings.


#include "General_Testing.h"
#include "IndexerTests.h"
#include "Misc/AES.h"
#include "Containers/UnrealString.h"
#include "HexUtility.h"
#include "IndexerSupport.h"
#include "Auth.h"
#include "SequenceEncryptor.h"
#include "Sequence/SequenceAPI.h"
#include "tests/ContractTest.h"
#include "Tests/TestSequenceAPI.h"

// Sets default values
AGeneral_Testing::AGeneral_Testing()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AGeneral_Testing::BeginPlay()
{
	Super::BeginPlay();
}

void AGeneral_Testing::test_provider()
{
	TFunction<void (FString)> OnSuccess = [this](FString State)
	{
		callback_passed(State);
	};

	TFunction<void (FString, SequenceError)> OnFailure = [this](FString data, SequenceError Err)
	{
		Callback_Failed(data, Err);
	};

	//ContractTest::RunTest(OnSuccess, OnFailure);
	SequenceAPITest::RunTest(OnSuccess, OnFailure);
}

void AGeneral_Testing::test_indexer()
{
	TFunction<void(FString)> OnSuccess = [this](FString State)
	{
		callback_passed(State);
	};

	TFunction<void(FString, SequenceError)> OnFailure = [this](FString data, SequenceError Err)
	{
		Callback_Failed(data, Err);
	};

	IndexerTest(OnSuccess, OnFailure);
}

void AGeneral_Testing::testMisc()
{//used for testing various things in the engine to verify behaviour
	UAuth * auth = NewObject<UAuth>();
	FStoredAuthState_BE testingStruct;

	FString preEncrypt = UIndexerSupport::structToSimpleString<FStoredAuthState_BE>(testingStruct);

	auth->setNewSecureStorableAuth(testingStruct);
	FSecureKey duringEncryptStruct = auth->getSecureStorableAuth();

	FString encryptedData = UIndexerSupport::structToSimpleString<FSecureKey>(duringEncryptStruct);

	auth->setSecureStorableAuth(duringEncryptStruct);

	FString decryptedData = UIndexerSupport::structToSimpleString<FStoredAuthState_BE>(auth->auth);

	UE_LOG(LogTemp, Display, TEXT("Pre Encrypt: %s"), *preEncrypt);
	UE_LOG(LogTemp, Display, TEXT("Encrypted: %s"), *encryptedData);
	UE_LOG(LogTemp, Display, TEXT("Post Encrypt: %s"), *decryptedData);
}

void AGeneral_Testing::testSequence()
{
	TFunction<void(FString)> OnSuccess = [this](FString State)
	{
		callback_passed(State);
	};

	TFunction<void(FString, SequenceError)> OnFailure = [this](FString data, SequenceError Err)
	{
		Callback_Failed(data, Err);
	};

	TestSequenceData(OnSuccess, OnFailure);
}

// Called every frame
void AGeneral_Testing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGeneral_Testing::callback_passed(FString state_data)
{
	UE_LOG(LogTemp, Display, TEXT("========================================================================="));
	UE_LOG(LogTemp, Display, TEXT("[Callback Passed!]\nAdditional State: [%s]"), *state_data);
	UE_LOG(LogTemp, Display, TEXT("========================================================================="));
}

void AGeneral_Testing::Callback_Failed(const FString state_data, SequenceError error) const
{
	UE_LOG(LogTemp, Display, TEXT("========================================================================="));
	UE_LOG(LogTemp, Error, TEXT("[Callback Failed!]\nAdditional State: [%s]"), *state_data);
	UE_LOG(LogTemp, Error, TEXT("[Error Message]:\n[%s]"),*error.Message);
	UE_LOG(LogTemp, Error, TEXT("[Error Type]: [%s]"),*Error_To_String(error.Type));
	UE_LOG(LogTemp, Display, TEXT("========================================================================="));
}

FString AGeneral_Testing::Error_To_String(ErrorType error)
{
	switch (error) {
	case NotFound:
		return "NotFound";
	case ResponseParseError:
		return "ResponseParseError";
	case EmptyResponse:
		return "EmptyResponse";
	case UnsupportedMethodOnChain:
		return "UnsupportedMethodOnChain";
	case RequestFail:
		return "RequestFail";
	case RequestTimeExceeded:
		return "RequestTimeExceeded";
	case TestFail:
		return "TestFail";
	default:
		return "SequenceError";
	}
}

