// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "SequenceBackendManager.h"
#include "Provider.h"
#include "TimerManager.h"
#include "SystemDataBuilder.h"
#include "Engine/World.h"

FUserDetails ASequenceBackendManager::GetUserDetails()
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

void ASequenceBackendManager::CallShowAuthFailureScreen(const FString& ErrorIn)
{
	if (this->ShowAuthFailureDelegate.IsBound())
		this->ShowAuthFailureDelegate.Broadcast(ErrorIn);
	else
		UE_LOG(LogTemp, Error, TEXT("[Nothing bound to: ShowAuthFailureDelegate]"));
}

void ASequenceBackendManager::CallShowAuthSuccessScreen()
{
	if (this->ShowAuthSuccessDelegate.IsBound())
		this->ShowAuthSuccessDelegate.Broadcast();
	else
		UE_LOG(LogTemp, Error, TEXT("[Nothing bound to: ShowAuthSuccessDelegate]"));
}

void ASequenceBackendManager::CallShowFederationSuccess()
{
	if (this->ShowFederateSuccessDelegate.IsBound())
	{
		this->ShowFederateSuccessDelegate.Broadcast();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[Nothing bound to: ShowFederateSuccessDelegate]"));
	}
}

void ASequenceBackendManager::CallShowFederationFailure(const FString& Error)
{
	if (this->ShowFederationFailureDelegate.IsBound())
	{
		this->ShowFederationFailureDelegate.Broadcast(Error);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[Nothing bound to: ShowFederationFailureDelegate]"));
	}
}

void ASequenceBackendManager::CallShowFederateOrForce(const FFederationSupportData& FederationData)
{
	if (this->ShowFederateOrForceDelegate.IsBound())
	{
		this->ShowFederateOrForceDelegate.Broadcast(FederationData);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[Nothing bound to: ShowFederateOrForceDelegate]"));
	}
}

// Sets default values
ASequenceBackendManager::ASequenceBackendManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	this->Authenticator = nullptr;
}

// Called when the game starts or when spawned
void ASequenceBackendManager::BeginPlay()
{
	Super::BeginPlay();
	this->Authenticator = NewObject<USequenceAuthenticator>();

	//setup up delegate bindings
	this->Authenticator->AuthSuccess.AddDynamic(this, &ASequenceBackendManager::CallShowAuthSuccessScreen);
	this->Authenticator->AuthRequiresCode.AddDynamic(this, &ASequenceBackendManager::CallReadyToReceiveCode);
	this->Authenticator->AuthFailure.AddDynamic(this, &ASequenceBackendManager::CallShowAuthFailureScreen);
	this->Authenticator->FederateSuccess.AddDynamic(this, &ASequenceBackendManager::CallShowFederationSuccess);
	this->Authenticator->FederateFailure.AddDynamic(this, &ASequenceBackendManager::CallShowFederationFailure);
	this->Authenticator->FederateOrForce.AddDynamic(this, &ASequenceBackendManager::CallShowFederateOrForce);
}

//SYNC FUNCTIONAL CALLS// [THESE ARE BLOCKING CALLS AND WILL RETURN DATA IMMEDIATELY]

void ASequenceBackendManager::SetupCustomEncryptor(UGenericNativeEncryptor * EncryptorIn)
{
	if (this->Authenticator)
	{
		this->Authenticator->SetCustomEncryptor(EncryptorIn);
	}
}

/*
	Used to copy data to the systems clipboard!
*/
void ASequenceBackendManager::CopyToClipboard(FString Data)
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

void ASequenceBackendManager::InitiateMobileSSO(const ESocialSigninType& Type, const bool ForceCreateAccountIn)
{
	if (this->Authenticator)
	{
		this->Authenticator->InitiateMobileSSO(Type, ForceCreateAccountIn);
	}
}

FString ASequenceBackendManager::GetLoginURL(const ESocialSigninType& Type)
{
	if (this->Authenticator)
	{
		return this->Authenticator->GetSigninURL(Type);
	}
	else
	{
		return "invalid";
	}
}

void ASequenceBackendManager::SocialLogin(const FString& IDTokenIn, const bool ForceCreateAccountIn)
{
	if (this->Authenticator)
	{
		this->Authenticator->SocialLogin(IDTokenIn, ForceCreateAccountIn);
	}
}

void ASequenceBackendManager::EmailLogin(const FString& EmailIn, const bool ForceCreateAccountIn)
{
	if (this->Authenticator)
	{
		this->Authenticator->EmailLogin(EmailIn, ForceCreateAccountIn);
	}
}

void ASequenceBackendManager::EmailCode(const FString& CodeIn)
{
	if (this->Authenticator)
	{
		this->Authenticator->EmailLoginCode(CodeIn);
	}
}

void ASequenceBackendManager::GuestLogin(const bool ForceCreateAccountIn)
{
	if (this->Authenticator)
	{
		this->Authenticator->GuestLogin(ForceCreateAccountIn);
	}
}

void ASequenceBackendManager::PlayFabLogin(const FString& UsernameIn, const FString& PasswordIn, const bool ForceCreateAccountIn)
{
	if (this->Authenticator)
	{
		this->Authenticator->PlayFabLogin(UsernameIn, PasswordIn, ForceCreateAccountIn);
	}
}

void ASequenceBackendManager::PlayFabRegisterAndLogin(const FString& UsernameIn, const FString& EmailIn, const FString& PasswordIn, const bool ForceCreateAccountIn)
{
	if (this->Authenticator)
	{
		this->Authenticator->PlayFabRegisterAndLogin(UsernameIn, EmailIn, PasswordIn, ForceCreateAccountIn);
	}
}

void ASequenceBackendManager::FederateEmail(const FString& EmailIn) const
{
	if (this->Authenticator)
	{
		this->Authenticator->FederateEmail(EmailIn);
	}
}

void ASequenceBackendManager::FederateOIDCIdToken(const FString& IdTokenIn) const
{
	if (this->Authenticator)
	{
		this->Authenticator->FederateOIDCIdToken(IdTokenIn);
	}
}

void ASequenceBackendManager::InitiateMobileFederateOIDC(const ESocialSigninType& TypeIn)
{
	if (this->Authenticator)
	{
		this->Authenticator->InitiateMobileFederateOIDC(TypeIn);
	}
}

void ASequenceBackendManager::FederatePlayFabNewAccount(const FString& UsernameIn, const FString& EmailIn, const FString& PasswordIn) const
{
	if (this->Authenticator)
	{
		this->Authenticator->FederatePlayFabNewAccount(UsernameIn, EmailIn, PasswordIn);
	}
}

void ASequenceBackendManager::FederatePlayFabLogin(const FString& UsernameIn, const FString& PasswordIn) const
{
	if (this->Authenticator)
	{
		this->Authenticator->FederatePlayFabLogin(UsernameIn, PasswordIn);
	}
}

FCredentials_BE ASequenceBackendManager::GetStoredCredentials()
{
	FStoredCredentials_BE PCred = this->Authenticator->GetStoredCredentials();
	if (PCred.GetValid())
	{
		return PCred.GetCredentials();
	}
	return FCredentials_BE();
}

void ASequenceBackendManager::ForceOpenLastOpenSessionAttempt()
{
	if (this->Authenticator)
	{
		this->Authenticator->ForceOpenLastOpenSessionAttempt();
	}
}

bool ASequenceBackendManager::StoredCredentialsValid()
{
	const FStoredCredentials_BE PCred = this->Authenticator->GetStoredCredentials();
	return PCred.GetValid();
}

//SYNC FUNCTIONAL CALLS// [THESE ARE BLOCKING CALLS AND WILL RETURN DATA IMMEDIATELY]

//ASYNC FUNCTIONAL CALLS// [THESE ARE NON BLOCKING CALLS AND WILL USE A MATCHING UPDATE...FUNC TO RETURN DATA]


void ASequenceBackendManager::UpdateSystemTestableData(const FSystemData_BE& systemData)
{
	this->TestCoins = systemData.user_data.coins;
	this->TestNfTs = systemData.user_data.nfts;

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
	//USystemDataBuilder * builder = NewObject<USystemDataBuilder>();
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