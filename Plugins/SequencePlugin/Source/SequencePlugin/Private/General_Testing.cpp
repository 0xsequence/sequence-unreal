// Fill out your copyright notice in the Description page of Project Settings.


#include "General_Testing.h"
#include "IndexerTests.h"
#include "Misc/AES.h"
#include "Containers/UnrealString.h"
#include "HexUtility.h"
#include "SequenceEncryptor.h"
#include "tests/ContractTest.h"

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

	ContractTest::RunTest(OnSuccess, OnFailure);
	
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
	
	const FString key = "abababababababababababababababab";
	FString name = "calvin";

	int32 payloadLength = name.Len();//store this ahead of time unless it's hardcoded!

	FString encryptedData = USequenceEncryptor::encrypt(name, key);

	FString decryptedData = USequenceEncryptor::decrypt(encryptedData, payloadLength, key);

	UE_LOG(LogTemp, Display, TEXT("Pre Encrypt: %s"), *name);
	UE_LOG(LogTemp, Display, TEXT("Encrypted: %s"), *encryptedData);
	UE_LOG(LogTemp, Display, TEXT("Post Encrypt: %s"), *decryptedData);

	//UE_LOG(LogTemp, Display, TEXT("Pre Encrypt: %s"), *name);
	//UE_LOG(LogTemp, Display, TEXT("Key Size: %d"), key.Len());

	//uint32 keySize = 32;//hard code this!
	//uint8* keyBlob = new uint8[keySize];

	//StringToBytes(key,keyBlob, keySize);
	//FString ret = BytesToString(keyBlob, keySize);

	//uint8* blob; //we declare uint8 pointer
	//uint32 size; //for size calculation
	//
	////encrypting
	////sizing
	//size = name.Len();
	//size = size + (FAES::AESBlockSize - (size % FAES::AESBlockSize));
	//blob = new uint8[size];

	//StringToBytes(name,blob, name.Len());
	//FAES::EncryptData(blob, size,keyBlob,keySize); //We encrypt the data, don't know how you want to input key
	//FString encryptedData = BytesToString(blob,size);
	//UE_LOG(LogTemp, Display, TEXT("During Encrypt: %s"), *encryptedData);
	//delete blob; //deleting allocation for safety

	////decrypting
	////sizing
	//
	//size = encryptedData.Len();
	//size = size + (FAES::AESBlockSize - (size % FAES::AESBlockSize));//align size with block size!
	//blob = new uint8[size];
	//
	////FString::ToHexBlob(encryptedData, blob, encryptedData.Len());
	//StringToBytes(encryptedData, blob,encryptedData.Len());
	//FAES::DecryptData(blob, size,keyBlob,keySize);
	//FString decryptedData = BytesToString(blob, size);
	//decryptedData = decryptedData.Left(name.Len());
	//UE_LOG(LogTemp, Display, TEXT("post Encrypt: %s"), *decryptedData);

	//delete blob;
	//delete keyBlob;
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

