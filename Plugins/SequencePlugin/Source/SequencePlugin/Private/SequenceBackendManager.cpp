// Fill out your copyright notice in the Description page of Project Settings.

#include "SequenceBackendManager.h"
#include "Provider.h"
#include "TimerManager.h"
#include "SystemDataBuilder.h"

FUserDetails ASequenceBackendManager::getUserDetails()
{
	FUserDetails ret;
	return ret;
}


void ASequenceBackendManager::CallReadyToReceiveCode()
{
	if (this->ReadyToReceiveCodeDelegate.IsBound())
		this->ReadyToReceiveCodeDelegate.Broadcast();
	else
		UE_LOG(LogTemp, Error, TEXT("[Nothing bound to: ReadyToReceiveCodeDelegate]"));
}

void ASequenceBackendManager::CallShowAuthFailureScreen()
{
	if (this->ShowAuthFailureDelegate.IsBound())
		this->ShowAuthFailureDelegate.Broadcast();
	else
		UE_LOG(LogTemp, Error, TEXT("[Nothing bound to: ShowAuthFailureDelegate]"));
}

void ASequenceBackendManager::CallShowAuthSuccessScreen(const FCredentials_BE& CredentialsIn)
{
	this->Credentials = CredentialsIn;
	if (this->ShowAuthSuccessDelegate.IsBound())
		this->ShowAuthSuccessDelegate.Broadcast(Credentials);
	else
		UE_LOG(LogTemp, Error, TEXT("[Nothing bound to: ShowAuthSuccessDelegate]"));
}

// Sets default values
ASequenceBackendManager::ASequenceBackendManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	this->authenticator = nullptr;
}

// Called when the game starts or when spawned
void ASequenceBackendManager::BeginPlay()
{
	Super::BeginPlay();
	this->authenticator = NewObject<UAuthenticator>();

	//setup up delegate bindings
	FScriptDelegate del;
	del.BindUFunction(this, "CallShowAuthSuccessScreen");
	this->authenticator->AuthSuccess.Add(del);
	this->authenticator->AuthRequiresCode.AddDynamic(this, &ASequenceBackendManager::CallReadyToReceiveCode);
	this->authenticator->AuthFailure.AddDynamic(this, &ASequenceBackendManager::CallShowAuthFailureScreen);
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

FString ASequenceBackendManager::GetLoginURL(const ESocialSigninType& Type)
{
	if (this->authenticator)
	{
		return this->authenticator->GetSigninURL(Type);
	}
	else
	{
		UE_LOG(LogTemp,Display,TEXT("Authenticator invalid!"));
		return "invalid";
	}
}

FString ASequenceBackendManager::GetRedirectURL()
{
	if (this->authenticator)
	{
		return this->authenticator->GetRedirectURL();
	}
	else
	{
		UE_LOG(LogTemp,Display,TEXT("Authenticator invalid!"));
		return "invalid";
	}
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

bool ASequenceBackendManager::StoredCredentialsValid()
{
	bool valid = false;
	FStoredCredentials_BE PCred = this->authenticator->GetStoredCredentials();
	valid = PCred.GetValid();
	if (valid)
	{
		this->Credentials = PCred.GetCredentials();
	}
	return valid;
}

//SYNC FUNCTIONAL CALLS// [THESE ARE BLOCKING CALLS AND WILL RETURN DATA IMMEDIATELY]

//ASYNC FUNCTIONAL CALLS// [THESE ARE NON BLOCKING CALLS AND WILL USE A MATCHING UPDATE...FUNC TO RETURN DATA]


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

	const TFunction<void (FString, FSequenceError)> SendFailure = [this](const TransactionID ID, const FSequenceError Error)
	{
		UE_LOG(LogTemp, Display, TEXT("[Error With Transaction] [%s]"),*Error.Message);
		FTxnCallback_BE Callback;
		Callback.good_txn = false;
		Callback.txn_hash_id = ID;
		this->UpdateTxn(Callback);
	};

	//this->sequenceWallet->SendTransactionWithCallback(TransactionData,SendSuccess,SendFailure);
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

	//this->sequenceWallet->SendTransactionWithCallback(TransactionData, SendSuccess, SendFailure);
}

void ASequenceBackendManager::InitGetUpdatedCoinData(TArray<FID_BE> CoinsToUpdate)
{	
	UE_LOG(LogTemp, Display, TEXT("[Update Coin Fetch INITIATED]"));
	
	const TSuccessCallback<TArray<FItemPrice_BE>> GenericSuccess = [this](const TArray<FItemPrice_BE> updatedCoinData)
	{
	};

	const FFailureCallback GenericFailure = [=](const FSequenceError Error)
	{
		UE_LOG(LogTemp, Error, TEXT("[Error in fetching updated coin data]"));
		TArray<FItemPrice_BE> bList;
	};

	//this->sequenceWallet->getUpdatedCoinPrices(CoinsToUpdate,GenericSuccess,GenericFailure);
}


void ASequenceBackendManager::InitGetUpdateTokenData(TArray<FID_BE> TokensToUpdate)
{
	UE_LOG(LogTemp, Display, TEXT("[Update NFT Fetch INITIATED]"));
	const TSuccessCallback<TArray<FItemPrice_BE>> GenericSuccess = [this](const TArray<FItemPrice_BE> updatedTokenData)
	{
	};

	const FFailureCallback GenericFailure = [=](const FSequenceError Error)
	{
		UE_LOG(LogTemp, Error, TEXT("[Error in fetching updated token data]"));
		TArray<FItemPrice_BE> bList;
	};

	//this->sequenceWallet->getUpdatedCollectiblePrices(TokensToUpdate, GenericSuccess, GenericFailure);
}

//ASYNC FUNCTIONAL CALLS// [THESE ARE NON BLOCKING CALLS AND WILL USE A MATCHING UPDATE...FUNC TO RETURN DATA]