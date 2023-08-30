// Fill out your copyright notice in the Description page of Project Settings.


#include "General_Testing.h"
#include "Indexer/IndexerTests.h"
#include "ObjectHandler.h"
#include "Misc/AES.h"
#include "Containers/UnrealString.h"
#include "Util/HexUtility.h"
#include "Indexer/IndexerSupport.h"
#include "Auth.h"
#include "SequenceEncryptor.h"
#include "SystemDataBuilder.h"
#include "Sequence/SequenceAPI.h"
#include "tests/ContractTest.h"
#include "Tests/TestSequenceAPI.h"

// Sets default values
AGeneral_Testing::AGeneral_Testing()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	testingURLs.Add("https://assets.coingecko.com/coins/images/6319/large/USD_Coin_icon.png?1547042389");
	testingURLs.Add("https://www.circle.com/hubfs/share-USDC.png#keepProtocol");
	testingURLs.Add("https://assets.skyweaver.net/i7FuksL3/webapp/cards/full-cards/4x/0-silver.png");
	testingURLs.Add("https://skyweaver.net/images/skyweavercover.jpg");
}

// Called when the game starts or when spawned
void AGeneral_Testing::BeginPlay()
{
	Super::BeginPlay();
}

void AGeneral_Testing::TestProvider() const
{
	const TFunction<void (FString)> OnSuccess = [this](FString State)
	{
		CallbackPassed(State);
	};

	const TFunction<void (FString, FSequenceError)> OnFailure = [this](FString Data, FSequenceError Err)
	{
		CallbackFailed(Data, Err);
	};

	ContractTest::RunTest(OnSuccess, OnFailure);
	SequenceAPITest::RunTest(OnSuccess, OnFailure);
}

void AGeneral_Testing::TestIndexer()
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

void AGeneral_Testing::TestEncryption() const
{
	UAuth* Auth = NewObject<UAuth>();
	const FStoredAuthState_BE TestingStruct;

	const FString PreEncrypt = UIndexerSupport::structToSimpleString<FStoredAuthState_BE>(TestingStruct);

	Auth->SetNewSecureStorableAuth(TestingStruct);
	const FSecureKey DuringEncryptStruct = Auth->GetSecureStorableAuth();

	const FString EncryptedData = UIndexerSupport::structToSimpleString<FSecureKey>(DuringEncryptStruct);

	Auth->SetSecureStorableAuth(DuringEncryptStruct);

	const FString DecryptedData = UIndexerSupport::structToSimpleString<FStoredAuthState_BE>(Auth->auth);

	UE_LOG(LogTemp, Display, TEXT("Pre Encrypt: %s"), *PreEncrypt);
	UE_LOG(LogTemp, Display, TEXT("Encrypted: %s"), *EncryptedData);
	UE_LOG(LogTemp, Display, TEXT("Post Encrypt: %s"), *DecryptedData);
}

//dedicated encryption test!

void AGeneral_Testing::TestMisc()
{//used for testing various things in the engine to verify behaviour
	imgHandler = NewObject<UObjectHandler>();
	imgHandler->Setup(true);//we want to test caching!
	imgHandler->FOnDoneImageProcessingDelegate.BindUFunction(this, "OnDoneImageProcessing");
	imgHandler->requestImages(this->testingURLs);
}

void AGeneral_Testing::OnDoneImageProcessing()
{//forward this to the front as we will be able to view all the images from there!
	this->testMiscForwarder(this->imgHandler->getProcessedImages());
}

void AGeneral_Testing::TestSequence() const
{
	TFunction<void(FString)> OnSuccess = [this](FString State)
	{
		CallbackPassed(State);
	};

	TFunction<void(FString, FSequenceError)> OnFailure = [this](FString data, FSequenceError Err)
	{
		CallbackFailed(data, Err);
	};

	TestSequenceData(OnSuccess, OnFailure);
}

void AGeneral_Testing::testSystemDataBuilder()
{//testing system data builder
	USystemDataBuilder* sysBuilder = NewObject<USystemDataBuilder>();
	UIndexer* indexer = NewObject<UIndexer>();
	SequenceAPI::FSequenceWallet* wallet = new SequenceAPI::FSequenceWallet();
	sysBuilder->testGOTokenData(indexer,wallet,137, "0x0E0f9d1c4BeF9f0B8a2D9D4c09529F260C7758A2");
}

// Called every frame
void AGeneral_Testing::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGeneral_Testing::CallbackPassed(FString StateData) const
{
	UE_LOG(LogTemp, Display, TEXT("========================================================================="));
	UE_LOG(LogTemp, Display, TEXT("[Callback Passed!]\nAdditional State: [%s]"), *StateData);
	UE_LOG(LogTemp, Display, TEXT("========================================================================="));
}

void AGeneral_Testing::CallbackFailed(const FString StateData, FSequenceError Error) const
{
	UE_LOG(LogTemp, Display, TEXT("========================================================================="));
	UE_LOG(LogTemp, Error, TEXT("[Callback Failed!]\nAdditional State: [%s]"), *StateData);
	UE_LOG(LogTemp, Error, TEXT("[Error Message]:\n[%s]"),*Error.Message);
	UE_LOG(LogTemp, Error, TEXT("[Error Type]: [%s]"),*ErrorToString(Error.Type));
	UE_LOG(LogTemp, Display, TEXT("========================================================================="));
}

FString AGeneral_Testing::ErrorToString(EErrorType Error)
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

