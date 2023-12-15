// Fill out your copyright notice in the Description page of Project Settings.

#include "SequenceBackendManager.h"
#include "Provider.h"
#include "Util/HexUtility.h"
#include "Misc/AutomationTest.h"
#include "GenericPlatform/GenericPlatformApplicationMisc.h"
#include "Bitcoin-Cryptography-Library/cpp/Keccak256.hpp"
#include "Bitcoin-Cryptography-Library/cpp/Ecdsa.hpp"
#include "Indexer/Indexer.h"
#include "Eth/Crypto.h"
#include "SystemDataBuilder.h"

FUserDetails ASequenceBackendManager::getUserDetails()
{
	FUserDetails ret;
	ret.accountID = this->accountID;
	ret.email = this->userEmail;
	ret.emailService = this->emailService;
	ret.username = this->username;
	return ret;
}


void ASequenceBackendManager::CallReadyToReceiveCode()
{
	this->ReadyToReceiveCodeDelegate.Broadcast();
}

void ASequenceBackendManager::CallShowLoadingScreen()
{
	this->ShowLoadingScreenDelegate.Broadcast();
}

void ASequenceBackendManager::CallShowLoginScreen()
{
	this->ShowLoginScreenDelegate.Broadcast();
}

// Sets default values
ASequenceBackendManager::ASequenceBackendManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	this->sequenceWallet = new SequenceAPI::FSequenceWallet();
	this->Indexer = NewObject<UIndexer>();//for handling indexer data
	this->auth = NewObject<UAuth>();
	this->authenticator = NewObject<UAuthenticator>();
	//setup up delegate bindings
	this->authenticator->AuthRequiresCode.AddDynamic(this,&ASequenceBackendManager::CallReadyToReceiveCode);
}

ASequenceBackendManager::~ASequenceBackendManager()
{
	if (this->sequenceWallet)
	{
		delete this->sequenceWallet;//free memory only if it's valid!
	}
}

// Called when the game starts or when spawned
void ASequenceBackendManager::BeginPlay()
{
	Super::BeginPlay();
}

//SYNC FUNCTIONAL CALLS// [THESE ARE BLOCKING CALLS AND WILL RETURN DATA IMMEDIATELY]

/*
	Used to copy data to the systems clipboard!
*/
void ASequenceBackendManager::CopyToClipboard(FString data)
{
	//FGenericPlatformMisc::ClipboardCopy(*data);
}

FString ASequenceBackendManager::GetFromClipboard()
{
	FString retData;
	//gets data from clipboard but it comes back invalid? this will be broken until we move up engine versions (hopefully epic actually implements a real solution)
	//FGenericPlatformMisc::ClipboardPaste(retData);
	return retData;
}

FString ASequenceBackendManager::GetTransactionHash(FTransaction_FE Transaction)
{
	return Transaction.ID();
}

FSecureKey ASequenceBackendManager::GetSecureStorableAuth()
{
	return this->auth->GetSecureStorableAuth();//get the stored auth data ready for storage!
}

FString ASequenceBackendManager::GetLoginURL()
{
	return this->authenticator->GetSigninURL();
}

FString ASequenceBackendManager::GetRedirectURL()
{
	return this->authenticator->GetRedirectURL();
}

void ASequenceBackendManager::SetSocialLoginType(ESocialSigninType Type)
{
	this->authenticator->SetSocialLoginType(Type);
}

void ASequenceBackendManager::SocialLogin(const FString& IDTokenIn)
{
	this->authenticator->SocialLogin(IDTokenIn);
}

void ASequenceBackendManager::EmailLogin(const FString& EmailIn)
{
	this->authenticator->EmailLogin(EmailIn);
}

void ASequenceBackendManager::EmailCode(const FString& CodeIn)
{
	this->authenticator->EmailLoginCode(CodeIn);
}

//SYNC FUNCTIONAL CALLS// [THESE ARE BLOCKING CALLS AND WILL RETURN DATA IMMEDIATELY]

//ASYNC FUNCTIONAL CALLS// [THESE ARE NON BLOCKING CALLS AND WILL USE A MATCHING UPDATE...FUNC TO RETURN DATA]

void ASequenceBackendManager::RandomReceive()
{
	int32 Dist = FMath::RandRange(1, 100);
	int32 AcceptRange = 10;
	bool bGenNew = (Dist <= AcceptRange);//acceptRange % of the time we will generate something new!

	if (FMath::RandBool())
	{//coin
		FCoin_BE RCoin = this->testCoins[FMath::RandRange(0, this->testCoins.Num() - 1)];
		RCoin.Coin_Amount = FMath::FRandRange(0.001, 100);
		if (bGenNew)
		{
			RCoin.Coin_Long_Name = "NewRandomCoin:";
			RCoin.Coin_Long_Name.AppendInt(FMath::RandRange(0,1000));
		}
		this->ReceiveCoin(RCoin);
	}
	else
	{//nft
		FNFT_Master_BE RNft = this->testNFTs[FMath::RandRange(0, this->testNFTs.Num() - 1)];
		
		int32 RCount = FMath::RandRange(1, 100);
		for (int32 i = 0; i < RCount; i++)
		{
			FNFT_UData_BE uData;
			RNft.nft_data.Add(uData);
		}

		if (bGenNew)
		{
			RNft.NFT_Name = "NewRandomNFT";
			RNft.NFT_Name.AppendInt(FMath::RandRange(0, 1000));
			RNft.Collection_Long_Name = "Testing_Collection";
			RNft.Collection_Short_Name = "Tst";
		}

		this->ReceiveNFT(RNft);
	}
	FTimerHandle TimerTestReceive;
	FTimerDelegate Delegate; // Delegate to bind function with parameters
	Delegate.BindUFunction(this, "randomReceive"); // Character is the parameter we wish to pass with the function.
	GetWorld()->GetTimerManager().SetTimer(TimerTestReceive, Delegate,FMath::FRandRange(1,15), false);
}

void ASequenceBackendManager::UpdateSystemTestableData(const FSystemData_BE& systemData)
{
	this->testCoins = systemData.user_data.coins;
	this->testNFTs = systemData.user_data.nfts;

	//need to setup a special timer function delegate!

	FTimerHandle TimerTestReceive;
	FTimerDelegate Delegate; // Delegate to bind function with parameters
	Delegate.BindUFunction(this, "randomReceive"); // Character is the parameter we wish to pass with the function.
	GetWorld()->GetTimerManager().SetTimer(TimerTestReceive, Delegate, 60, false);

	this->UpdateSystemData(systemData);
}

void ASequenceBackendManager::InitSystemData()
{
	UE_LOG(LogTemp, Display, TEXT("[System Data Fetch INITIATED]"));
	USystemDataBuilder * builder = NewObject<USystemDataBuilder>();
	//Note we still need Auth prior to this but the idea is all of this is already setup and ready to go for this call
	//builder->initBuildSystemData(this->sequenceWallet, this->chainID, this->publicAddress, this);
}

void ASequenceBackendManager::InitCoinSendTxn(FTransaction_FE TransactionData)
{
	UE_LOG(LogTemp, Display, TEXT("[Coin Txn Request Initiated]"));//first chunk simulates signin request code gen

	const TSuccessCallback<FString> SendSuccess = [this](const FString ID)
	{
		FTxnCallback_BE Callback;
		Callback.good_txn = true;
		Callback.txn_hash_id = ID;
		this->UpdateTxn(Callback);
	};

	const TFunction<void (FString, FSequenceError)> SendFailure = [this](const SequenceAPI::TransactionID ID, const FSequenceError Error)
	{
		UE_LOG(LogTemp, Display, TEXT("[Error With Transaction] [%s]"),*Error.Message);
		FTxnCallback_BE Callback;
		Callback.good_txn = false;
		Callback.txn_hash_id = ID;
		this->UpdateTxn(Callback);
	};

	this->sequenceWallet->SendTransactionWithCallback(TransactionData,SendSuccess,SendFailure);
}

void ASequenceBackendManager::InitNFTSendTxn(FTransaction_FE TransactionData)
{
	UE_LOG(LogTemp, Display, TEXT("[NFT Txn Request Initiated]"));
	const TSuccessCallback<FString> SendSuccess = [this](const FString ID)
	{
		FTxnCallback_BE Callback;
		Callback.good_txn = true;
		Callback.txn_hash_id = ID;
		this->UpdateTxn(Callback);
	};

	const TFunction<void (FString, FSequenceError)> SendFailure = [this](const FString ID, const FSequenceError Error)
	{
		UE_LOG(LogTemp, Display, TEXT("[Error With Transaction] [%s]"), *Error.Message);
		FTxnCallback_BE callback;
		callback.good_txn = false;
		callback.txn_hash_id = ID;
		this->UpdateTxn(callback);
	};

	this->sequenceWallet->SendTransactionWithCallback(TransactionData, SendSuccess, SendFailure);
}

//update this to be the encrypted json string
void ASequenceBackendManager::InitAuthentication(FSecureKey storedAuthData)
{
	UE_LOG(LogTemp, Display, TEXT("[AUTH INITIATED]"));
	FTimerHandle TH_auth_delay;
	FTimerDelegate Delegate; // Delegate to bind function with parameters
	Delegate.BindUFunction(this, "UpdateAuthentication", this->auth->SetSecureStorableAuth(storedAuthData));
	GetWorld()->GetTimerManager().SetTimer(TH_auth_delay, Delegate, FMath::RandRange(1,4), false);
}

void ASequenceBackendManager::InitGetUpdatedCoinData(TArray<FID_BE> CoinsToUpdate)
{	
	UE_LOG(LogTemp, Display, TEXT("[Update Coin Fetch INITIATED]"));
	
	const TSuccessCallback<TArray<FItemPrice_BE>> GenericSuccess = [this](const TArray<FItemPrice_BE> updatedCoinData)
	{
		this->UpdateCoinData(updatedCoinData);
	};

	const FFailureCallback GenericFailure = [=](const FSequenceError Error)
	{
		UE_LOG(LogTemp, Error, TEXT("[Error in fetching updated coin data]"));
		TArray<FItemPrice_BE> bList;
		this->UpdateTokenData(bList);//we need to continue if possible otherwise the frontend hangs
	};

	this->sequenceWallet->getUpdatedCoinPrices(CoinsToUpdate,GenericSuccess,GenericFailure);
}


void ASequenceBackendManager::InitGetUpdateTokenData(TArray<FID_BE> TokensToUpdate)
{
	UE_LOG(LogTemp, Display, TEXT("[Update NFT Fetch INITIATED]"));
	const TSuccessCallback<TArray<FItemPrice_BE>> GenericSuccess = [this](const TArray<FItemPrice_BE> updatedTokenData)
	{
		this->UpdateTokenData(updatedTokenData);
	};

	const FFailureCallback GenericFailure = [=](const FSequenceError Error)
	{
		UE_LOG(LogTemp, Error, TEXT("[Error in fetching updated token data]"));
		TArray<FItemPrice_BE> bList;
		this->UpdateTokenData(bList);//we need to continue if possible otherwise the frontend hangs
	};

	this->sequenceWallet->getUpdatedCollectiblePrices(TokensToUpdate, GenericSuccess, GenericFailure);
}

//ASYNC FUNCTIONAL CALLS// [THESE ARE NON BLOCKING CALLS AND WILL USE A MATCHING UPDATE...FUNC TO RETURN DATA]