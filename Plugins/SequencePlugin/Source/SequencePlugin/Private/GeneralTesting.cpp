// Fill out your copyright notice in the Description page of Project Settings.

#include "GeneralTesting.h"
#include "Indexer/IndexerTests.h"
#include "ObjectHandler.h"
#include "Misc/AES.h"
#include "Containers/UnrealString.h"
#include "Util/HexUtility.h"
#include "Indexer/IndexerSupport.h"
#include "SequenceEncryptor.h"
#include "SystemDataBuilder.h"
#include "Sequence/SequenceAPI.h"
#include "tests/ContractTest.h"
#include "Tests/TestSequenceAPI.h"
#include "Authenticator.h"
#include "Types/Wallet.h"
#include "Indexer/Indexer_Enums.h"

// Sets default values
AGeneralTesting::AGeneralTesting()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AGeneralTesting::BeginPlay()
{
	Super::BeginPlay();
}

void AGeneralTesting::TestProvider() const
{
	const TFunction<void (FString)> OnSuccess = [this](FString State)
	{
		CallbackPassed(State);
	};

	const TFunction<void (FString, FSequenceError)> OnFailure = [this](FString Data, FSequenceError Err)
	{
		CallbackFailed(Data, Err);
	};

	//ContractTest::RunTest(OnSuccess, OnFailure);
	SequenceAPITest::RunTest(OnSuccess, OnFailure);
}

void AGeneralTesting::TestIndexer()
{
	TFunction<void(FString)> OnSuccess = [this](FString State)
	{
		CallbackPassed(State);
	};

	TFunction<void(FString, FSequenceError)> OnFailure = [this](FString data, FSequenceError Err)
	{
		CallbackFailed(data, Err);
	};

	IndexerTest(OnSuccess, OnFailure);
}

void AGeneralTesting::TestEncryption() const
{
	const FString PreEncrypt = "testing text";
	const FString EncryptedData = USequenceEncryptor::Encrypt(PreEncrypt);
	const FString DecryptedData = USequenceEncryptor::Decrypt(EncryptedData,PreEncrypt.Len());

	UE_LOG(LogTemp, Display, TEXT("Pre Encrypt: %s"), *PreEncrypt);
	UE_LOG(LogTemp, Display, TEXT("Encrypted: %s"), *EncryptedData);
	UE_LOG(LogTemp, Display, TEXT("Post Encrypt: %s"), *DecryptedData);
	FString TestState = (PreEncrypt.Compare(DecryptedData) == 0) ? "Passed" : "Falied";
	UE_LOG(LogTemp, Display, TEXT("Test: [%s]"), *TestState);
}

void AGeneralTesting::TestMisc()
{//used for testing various things in the engine to verify behaviour
	ESortOrder TestOrder = ESortOrder::ASC;
	FString TestResult = UEnum::GetValueAsString(TestOrder);
	UE_LOG(LogTemp, Display, TEXT("Printed UEnum: %s"), *TestResult);
}

void AGeneralTesting::OnDoneImageProcessing()
{//forward this to the front as we will be able to view all the images from there!
	this->testMiscForwarder(this->imgHandler->getProcessedImages());
}

//Not being used in V1
//void AGeneralTesting::TestSequence() const
//{
//	TFunction<void(FString)> OnSuccess = [this](FString State)
//	{
//		CallbackPassed(State);
//	};
//
//	TFunction<void(FString, FSequenceError)> OnFailure = [this](FString data, FSequenceError Err)
//	{
//		CallbackFailed(data, Err);
//	};
//
//	TestSequenceData(OnSuccess, OnFailure);
//}

//Not being used in V1
//void AGeneralTesting::testSystemDataBuilder()
//{//testing system data builder
//	USystemDataBuilder* sysBuilder = NewObject<USystemDataBuilder>();
//	SequenceAPI::FSequenceWallet* wallet = new SequenceAPI::FSequenceWallet();
//	sysBuilder->testGOTokenData(wallet,137, "0x0E0f9d1c4BeF9f0B8a2D9D4c09529F260C7758A2");
//}

// Called every frame
void AGeneralTesting::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGeneralTesting::CallbackPassed(FString StateData) const
{
	UE_LOG(LogTemp, Display, TEXT("========================================================================="));
	UE_LOG(LogTemp, Display, TEXT("[Callback Passed!]\nAdditional State: [%s]"), *StateData);
	UE_LOG(LogTemp, Display, TEXT("========================================================================="));
}

void AGeneralTesting::CallbackFailed(const FString StateData, FSequenceError Error) const
{
	UE_LOG(LogTemp, Display, TEXT("========================================================================="));
	UE_LOG(LogTemp, Error, TEXT("[Callback Failed!]\nAdditional State: [%s]"), *StateData);
	UE_LOG(LogTemp, Error, TEXT("[Error Message]:\n[%s]"),*Error.Message);
	UE_LOG(LogTemp, Error, TEXT("[Error Type]: [%s]"),*ErrorToString(Error.Type));
	UE_LOG(LogTemp, Display, TEXT("========================================================================="));
}

FString AGeneralTesting::ErrorToString(EErrorType Error)
{
	switch (Error) {
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