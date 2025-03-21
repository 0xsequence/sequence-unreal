// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "Subsystems/SequenceSessionsBP.h"

#include "Errors.h"
#include "PlayFabResponseIntent.h"
#include "RequestHandler.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Util/Log.h"

USequenceSessionsBP::USequenceSessionsBP()
{
	this->Sessions = NewObject<USequenceSessions>();
}

bool USequenceSessionsBP::CheckExistingSession() const
{
	return this->Sessions->CheckExistingSession();
}

void USequenceSessionsBP::GetGoogleTokenId(FOnBrowserRequired BrowserRequired)
{
	if (this->Sessions->IsBrowserRequired)
	{
		const FString Url = this->Sessions->GetGoogleSignInUrl();
		BrowserRequired.ExecuteIfBound(Url);
	}
	else
	{
		this->Sessions->GetGoogleTokenId(this);
	}
}

void USequenceSessionsBP::GetAppleTokenId(FOnBrowserRequired BrowserRequired)
{
	if (this->Sessions->IsBrowserRequired)
	{
		const FString Url = this->Sessions->GetAppleSignInUrl();
		BrowserRequired.ExecuteIfBound(Url);
	}
	else
	{
		this->Sessions->GetAppleTokenId(this);
	}
}

void USequenceSessionsBP::StartEmailLogin(const FString& Email, FOnSuccess RequiresCode, FOnFailure OnFailure)
{
	const TFunction<void()> OnApiSuccess = [this, RequiresCode]
	{
		RequiresCode.ExecuteIfBound();
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Email Auth Error: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};

	this->Sessions->StartEmailLogin(Email, OnApiSuccess, OnApiFailure);
}

void USequenceSessionsBP::ConfirmEmailLoginWithCode(const FString& Code, FOnSuccess OnSuccess, FOnFailure OnFailure, FOnFederationRequired OnFederationRequired)
{
	const TFunction<void()> OnApiSuccess = [this, OnSuccess]()
	{
		OnSuccess.ExecuteIfBound();
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Email Auth Error: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};

	const TFunction<void (FFederationSupportData)> OnApiFederationRequired = [this, OnFederationRequired](const FFederationSupportData& FederationData)
	{
		SEQ_LOG(Error, TEXT("Account Force Create Or Federation Required"));
		OnFederationRequired.ExecuteIfBound(FederationData);
	};

	this->Sessions->ConfirmEmailLoginWithCode(Code, OnApiSuccess, OnApiFailure, OnApiFederationRequired);
}

void USequenceSessionsBP::HandleNativeIdToken(const FString& IdToken)
{
	if (this->IdTokenReceived.IsBound())
		this->IdTokenReceived.Broadcast(IdToken);
	else
		SEQ_LOG(Error, TEXT("Nothing bound to delegate: IdTokenReceived"));
}

void USequenceSessionsBP::StartOidcSession(const FString& IdToken, FOnSuccess OnSuccess, FOnFailure OnFailure, FOnFederationRequired OnFederationRequired)
{
	const TFunction<void()> OnApiSuccess = [this, OnSuccess]()
	{
		OnSuccess.ExecuteIfBound();
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("OIDC Auth Error: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};

	const TFunction<void (FFederationSupportData)> OnApiFederationRequired = [this, OnFederationRequired](const FFederationSupportData& FederationData)
	{
		SEQ_LOG(Error, TEXT("Account Force Create Or Federation Required"));
		OnFederationRequired.ExecuteIfBound(FederationData);
	};

	this->Sessions->StartOidcSession(IdToken, OnApiSuccess, OnApiFailure, OnApiFederationRequired);
}

void USequenceSessionsBP::PlayFabRegistration(const FString& UsernameIn, const FString& EmailIn, const FString& PasswordIn, FOnSuccess OnSuccess, FOnFailure OnFailure, FOnFederationRequired OnFederationRequired)
{
	const TFunction<void()> OnApiSuccess = [this, OnSuccess]()
	{
		this->CallOnSessionCreated();
		OnSuccess.ExecuteIfBound();
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error Response: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};

	const TFunction<void (FFederationSupportData)> OnApiFederationRequired = [this, OnFederationRequired](const FFederationSupportData& FederationData)
	{
		SEQ_LOG(Error, TEXT("Account Force Create Or Federation Required"));
		OnFederationRequired.ExecuteIfBound(FederationData);
	};

	this->Sessions->PlayFabRegistration(UsernameIn, EmailIn, PasswordIn, OnApiSuccess, OnApiFailure, OnApiFederationRequired);
}

void USequenceSessionsBP::PlayFabLogin(const FString& UsernameIn, const FString& PasswordIn, FOnSuccess OnSuccess, FOnFailure OnFailure, FOnFederationRequired OnFederationRequired)
{
	const TFunction<void()> OnApiSuccess = [this, OnSuccess]()
	{
		OnSuccess.ExecuteIfBound();
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error Response: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};

	const TFunction<void (FFederationSupportData)> OnApiFederationRequired = [this, OnFederationRequired](const FFederationSupportData& FederationData)
	{
		SEQ_LOG(Error, TEXT("Account Force Create Or Federation Required"));
		OnFederationRequired.ExecuteIfBound(FederationData);
	};

	this->Sessions->PlayFabLogin(UsernameIn, PasswordIn, OnApiSuccess, OnApiFailure, OnApiFederationRequired);
}

void USequenceSessionsBP::PlayFabAuthenticateWithSessionTicket(const FString& SessionTicket, FOnSuccess OnSuccess, FOnFailure OnFailure, FOnFederationRequired OnFederationRequired)
{
	const TFunction<void()> OnOpenSuccess = [this, OnSuccess]()
	{
		this->CallOnSessionCreated();
		OnSuccess.ExecuteIfBound();
	};

	const FFailureCallback OnOpenFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};

	const TFunction<void (FFederationSupportData)> OnApiFederationRequired = [this, OnFederationRequired](const FFederationSupportData& FederationData)
	{
		SEQ_LOG(Error, TEXT("Account Force Create Or Federation Required"));
		OnFederationRequired.ExecuteIfBound(FederationData);
	};

	this->Sessions->PlayFabAuthenticateWithSessionTicket(SessionTicket, OnOpenSuccess, OnOpenFailure, OnApiFederationRequired);
}

void USequenceSessionsBP::StartGuestSession(FOnSuccess OnSuccess, FOnFailure OnFailure)
{
	const TFunction<void()> OnApiSuccess = [this, OnSuccess]()
	{
		this->CallOnSessionCreated();
		OnSuccess.ExecuteIfBound();
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Guest Auth Error: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};

	this->Sessions->StartGuestSession(OnApiSuccess, OnApiFailure);
}

void USequenceSessionsBP::FederateEmail(const FString& Email, FOnSuccess OnSuccess, FOnFailure OnFailure)
{
	const TFunction<void()> OnApiSuccess = [this, OnSuccess]
	{
		OnSuccess.ExecuteIfBound();
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Email Auth Error: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};

	this->Sessions->FederateEmail(Email, OnApiSuccess, OnApiFailure);
}

void USequenceSessionsBP::ConfirmEmailFederationWithCode(const FString& Code, FOnSuccess OnSuccess, FOnFailure OnFailure)
{
	const TFunction<void()> OnApiSuccess = [this, OnSuccess]()
	{
		OnSuccess.ExecuteIfBound();
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Email Auth Error: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};
		
	this->Sessions->ConfirmEmailFederationWithCode(Code, OnApiSuccess, OnApiFailure);
}

void USequenceSessionsBP::FederateOidcToken(const FString& IdTokenIn, FOnSuccess OnSuccess, FOnFailure OnFailure)
{
	const TFunction<void()> OnApiSuccess = [this, OnSuccess]()
	{
		OnSuccess.ExecuteIfBound();
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Email Auth Error: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};
		
	this->Sessions->FederateOidcToken(IdTokenIn, OnApiSuccess, OnApiFailure);
}

void USequenceSessionsBP::FederatePlayFabRegistration(const FString& UsernameIn, const FString& EmailIn, const FString& PasswordIn, FOnSuccess OnSuccess, FOnFailure OnFailure)
{
	const TFunction<void()> OnApiSuccess = [this, OnSuccess]()
	{
		OnSuccess.ExecuteIfBound();
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Warning, TEXT("Error Federating PlayFab Account: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};

	this->Sessions->FederatePlayFabRegistration(UsernameIn, EmailIn, PasswordIn, OnApiSuccess, OnApiFailure);
}

void USequenceSessionsBP::FederatePlayFabLogin(const FString& UsernameIn, const FString& PasswordIn, FOnSuccess OnSuccess, FOnFailure OnFailure)
{
	const TFunction<void()> OnApiSuccess = [this, OnSuccess]()
	{
		OnSuccess.ExecuteIfBound();
	};

	const FFailureCallback OnApiFailure = [OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Warning, TEXT("Error Federating PlayFab Account: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};

	this->Sessions->FederatePlayFabLogin(UsernameIn, PasswordIn, OnApiSuccess, OnApiFailure);
}

void USequenceSessionsBP::SetForceCreateAccount(const bool NewForceCreateAccount)
{
	this->Sessions->SetForceCreateAccount(NewForceCreateAccount);
}


void USequenceSessionsBP::CallOnSessionCreated() const
{
	if (this->OnSessionCreated.IsBound())
	{
		this->OnSessionCreated.Broadcast();
	}
}
