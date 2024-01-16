// Fill out your copyright notice in the Description page of Project Settings.


#include "GeneralTesting.h"
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
#include "Authenticator.h"
#include "Types/Wallet.h"

// Sets default values
AGeneralTesting::AGeneralTesting()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//TODO SWAP TO STATIC TARRAY FOR THIS//
	testingURLs.Add("https://assets.coingecko.com/coins/images/6319/large/USD_Coin_icon.png?1547042389");
	testingURLs.Add("https://www.circle.com/hubfs/share-USDC.png#keepProtocol");
	testingURLs.Add("https://assets.skyweaver.net/i7FuksL3/webapp/cards/full-cards/4x/0-silver.png");
	testingURLs.Add("https://skyweaver.net/images/skyweavercover.jpg");
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

	ContractTest::RunTest(OnSuccess, OnFailure);
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
	//UAuth* Auth = NewObject<UAuth>();
	//const FStoredAuthState_BE TestingStruct;

	//const FString PreEncrypt = UIndexerSupport::structToSimpleString<FStoredAuthState_BE>(TestingStruct);

	//Auth->SetNewSecureStorableAuth(TestingStruct);
	//const FSecureKey DuringEncryptStruct = Auth->GetSecureStorableAuth();

	//const FString EncryptedData = UIndexerSupport::structToSimpleString<FSecureKey>(DuringEncryptStruct);

	//Auth->SetSecureStorableAuth(DuringEncryptStruct);

	//const FString DecryptedData = UIndexerSupport::structToSimpleString<FStoredAuthState_BE>(Auth->auth);

	//UAuth* Auth = NewObject<UAuth>();
	//const FString PreEncrypt = "testing text";
	//const FString EncryptedData = USequenceEncryptor::Encrypt(PreEncrypt);
	//const FString DecryptedData = USequenceEncryptor::Decrypt(EncryptedData,PreEncrypt.Len());

	//UE_LOG(LogTemp, Display, TEXT("Pre Encrypt: %s"), *PreEncrypt);
	//UE_LOG(LogTemp, Display, TEXT("Encrypted: %s"), *EncryptedData);
	//UE_LOG(LogTemp, Display, TEXT("Post Encrypt: %s"), *DecryptedData);

	//TODO need to setup PKCS7 padding
	//AES_ctx ctx;
	//struct AES_ctx * PtrCtx = &ctx;

	//const int32 keySize = 32;
	//uint8_t key[keySize];
	//uint8_t* PtrKey = &key[0];

	//const int32 IVSize = 16;
	//uint8_t iv[IVSize];
	//uint8_t* PtrIV = &iv[0];
	//

	////setup the key
	//for (int i = 0; i < keySize; i++)
	//{
	//	key[i] = i % 16;
	//}

	//for (int i = 0; i < IVSize; i++)
	//{
	//	iv[i] = i;
	//}
	//
	//FString testData = "abcdabcdabcdabcd";
	//int32 cachedLen = testData.Len();
	//uint8_t* PtrString;
	//const int32 buffSize = 32;
	//uint8_t buff[buffSize];
	//PtrString = &buff[0];

	//UE_LOG(LogTemp, Display, TEXT("Pre Encrypted Data: %s"), *testData);

	//StringToBytes(testData,PtrString, buffSize);

	//AES_init_ctx_iv(PtrCtx, PtrKey,PtrIV);//init then use
	//AES_CBC_encrypt_buffer(PtrCtx, PtrString, buffSize);

	//FString DuringEncrypt = BytesToString(PtrString, buffSize);
	//UE_LOG(LogTemp, Display, TEXT("Encrypted Data: %s"), *DuringEncrypt);

	//AES_init_ctx_iv(PtrCtx, PtrKey, PtrIV);//init then use, need to re init this prior to decryption
	//AES_CBC_decrypt_buffer(PtrCtx, PtrString, buffSize);

	//testData = BytesToString(PtrString, buffSize);

	//testData = testData.Left(cachedLen);

	//UE_LOG(LogTemp, Display, TEXT("Pst Encrypted Data: %s"), *testData);

	FString test = "0123456789012345678901234567890123456789012345678901234567890123";

	FWallet* TestWallet = new FWallet(test);
	FString PrivateKey = TestWallet->GetWalletPrivateKey().ToHex().ToLower();
	FString PublicKey = TestWallet->GetWalletPublicKey().ToHex().ToLower();
	FString Address = TestWallet->GetWalletAddress().ToHex().ToLower();



	UE_LOG(LogTemp, Display, TEXT("PrivateKey: %s , %d"), *PrivateKey , PrivateKey.Len());
	UE_LOG(LogTemp, Display, TEXT("PublicKey: %s"), *PublicKey);
	UE_LOG(LogTemp, Display, TEXT("Address: %s"), *Address);

	FString UnixIssueString = "120";
	FString UnixExpireString = "123";

	FString CachedWalletAddress = "0x" + Address;
	FString Token = "";// "eyJhbGciOiJSUzI1NiIsImtpZCI6IjFmNDBmMGE4ZWYzZDg4MDk3OGRjODJmMjVjM2VjMzE3YzZhNWI3ODEiLCJ0eXAiOiJKV1QifQ.eyJpc3MiOiJhY2NvdW50cy5nb29nbGUuY29tIiwiYXpwIjoiOTcwOTg3NzU2NjYwLTM1YTZ0YzQ4aHZpOGNldjljbmtucDBpdWd2OXBvYTIzLmFwcHMuZ29vZ2xldXNlcmNvbnRlbnQuY29tIiwiYXVkIjoiOTcwOTg3NzU2NjYwLTM1YTZ0YzQ4aHZpOGNldjljbmtucDBpdWd2OXBvYTIzLmFwcHMuZ29vZ2xldXNlcmNvbnRlbnQuY29tIiwic3ViIjoiMTA0OTkyNTExOTA0MzcxMTk2MDYxIiwiaGQiOiJ6ZW1pbmQuY2EiLCJlbWFpbCI6ImNhbHZpbi5rb3JrQHplbWluZC5jYSIsImVtYWlsX3ZlcmlmaWVkIjp0cnVlLCJub25jZSI6IjFGMTdGQkQzNDFCREQ1NzQ4NEM0QUZCNkFCQzIzNTJEIiwibmJmIjoxNzA1MzMwMjgyLCJpYXQiOjE3MDUzMzA1ODIsImV4cCI6MTcwNTMzNDE4MiwianRpIjoiOTA0NGZiZTA5N2IxYjIzYzgyMDI1ODk4YjczZmQyNTE4YTU0MTFiYyJ9.isa5HXnkRpbg8kscxLsTT3b - WA8DRAddIMf5_I5XlkDLgNMLeNRI2yOzEPdFpmoLCgyYfHtOq2ODetDgkbreytRslbHOIq0ptAVesZXRb - Wyb7tJgwmuypNvMxZNMPHyXbM4TDzrucTrSKhLeR20pSE8dcncreNlK4BTfkPrn5tPIlgFP9HzvAkpFa99MGPM06jfCO695qgk2TZ - ntXVRmp327GRlmosCKFqdkVIRL0POSLlZa0prGZtl_xVEYmZPxArldkG7M1jBdFTyN1S0jOQI2sS57sWOM5fLBJqSCEuMlFpJakiZGr4TFhaSg9XOHiNcK2aRJfr8j3Szv1Rsg";
	FString Intent = "{\\\"version\\\":\\\"1.0.0\\\",\\\"packet\\\":{\\\"code\\\":\\\"openSession\\\",\\\"expires\\\":" + UnixExpireString + ",\\\"issued\\\":" + UnixIssueString + ",\\\"session\\\":\\\"" + CachedWalletAddress + "\\\",\\\"proof\\\":{\\\"idToken\\\":\\\""+Token+"\\\"}}}";
	FString Message = "{\"projectId\":124,\"idToken\":\""+Token+"\",\"sessionAddress\":\"" + CachedWalletAddress + "\",\"friendlyName\":\"FRIENDLY SESSION WALLET\",\"intentJSON\":\"" + Intent + "\"}";

	TArray<uint8> Sig = TestWallet->SignMessage(Message);

	FString SigString = BytesToHex(Sig.GetData(),Sig.Num());

	UE_LOG(LogTemp, Display, TEXT("Message: %s"), *Message);
	UE_LOG(LogTemp, Display, TEXT("Signature: %s"), *SigString);
}

//dedicated encryption test!

void AGeneralTesting::TestMisc()
{//used for testing various things in the engine to verify behaviour
	imgHandler = NewObject<UObjectHandler>();
	imgHandler->setup(true);//we want to test caching!
	imgHandler->FOnDoneImageProcessingDelegate.BindUFunction(this, "OnDoneImageProcessing");
	imgHandler->requestImages(this->testingURLs);
}

void AGeneralTesting::OnDoneImageProcessing()
{//forward this to the front as we will be able to view all the images from there!
	this->testMiscForwarder(this->imgHandler->getProcessedImages());
}

void AGeneralTesting::TestSequence() const
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

void AGeneralTesting::testSystemDataBuilder()
{//testing system data builder
	USystemDataBuilder* sysBuilder = NewObject<USystemDataBuilder>();
	SequenceAPI::FSequenceWallet* wallet = new SequenceAPI::FSequenceWallet();
	sysBuilder->testGOTokenData(wallet,137, "0x0E0f9d1c4BeF9f0B8a2D9D4c09529F260C7758A2");
}

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