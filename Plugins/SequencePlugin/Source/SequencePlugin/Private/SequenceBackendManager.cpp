// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "SequenceBackendManager.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Util/Log.h"

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

void ASequenceBackendManager::PlayfabAuthenticateWithSessionTicket(const FString& SessionTicket)
{
	if (this->Authenticator)
	{
		this->Authenticator->PlayFabAuthenticateWithSessionTicket(SessionTicket);
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