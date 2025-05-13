#include "Subsystems/SequenceSessionsBP.h"
#include "SequenceRPCManager.h"
#include "Errors.h"
#include "RequestHandler.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Util/Log.h"

USequenceSessionsBP::USequenceSessionsBP()
{
	this->Sessions = NewObject<USequenceSessions>();
}

bool USequenceSessionsBP::CheckExistingSession()
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

void USequenceSessionsBP::SignInWithGoogle(const FString& IdToken, FOnSuccess OnSuccess, FOnFailure OnFailure)
{
	this->StartOidcSession(IdToken, OnSuccess, OnFailure);
}

void USequenceSessionsBP::SignInWithApple(const FString& IdToken, FOnSuccess OnSuccess, FOnFailure OnFailure)
{
	this->StartOidcSession(IdToken, OnSuccess, OnFailure);
}

void USequenceSessionsBP::SignInWithEpic(const FString& IdToken, FOnSuccess OnSuccess, FOnFailure OnFailure)
{
	this->StartOidcSession(IdToken, OnSuccess, OnFailure);
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

void USequenceSessionsBP::ConfirmEmailLoginWithCode(const FString& Code, FOnSuccess OnSuccess, FOnFailure OnFailure)
{
	const TFunction<void()> OnApiSuccess = [this, OnSuccess]()
	{
		this->CallOnSessionCreated();
		OnSuccess.ExecuteIfBound();
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Email Auth Error: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};

	const TFunction<void (FFederationSupportData)> OnApiFederationRequired = [this](const FFederationSupportData& FederationData)
	{
		this->CallFederationRequired(FederationData);
	};

	this->Sessions->ConfirmEmailLoginWithCode(Code, OnApiSuccess, OnApiFailure, OnApiFederationRequired);
}

void USequenceSessionsBP::StartOidcSession(const FString& IdToken, FOnSuccess OnSuccess, FOnFailure OnFailure)
{
	const TFunction<void()> OnApiSuccess = [this, OnSuccess]()
	{
		this->CallOnSessionCreated();
		OnSuccess.ExecuteIfBound();
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("OIDC Auth Error: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};

	const TFunction<void (FFederationSupportData)> OnApiFederationRequired = [this](const FFederationSupportData& FederationData)
	{
		this->CallFederationRequired(FederationData);
	};

	this->Sessions->StartOidcSession(IdToken, OnApiSuccess, OnApiFailure, OnApiFederationRequired);
}

void USequenceSessionsBP::PlayFabRegistration(const FString& UsernameIn, const FString& EmailIn, const FString& PasswordIn, FOnSuccess OnSuccess, FOnFailure OnFailure)
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

	const TFunction<void (FFederationSupportData)> OnApiFederationRequired = [this](const FFederationSupportData& FederationData)
	{
		this->CallFederationRequired(FederationData);
	};

	this->Sessions->PlayFabRegistration(UsernameIn, EmailIn, PasswordIn, OnApiSuccess, OnApiFailure, OnApiFederationRequired);
}

void USequenceSessionsBP::PlayFabLogin(const FString& UsernameIn, const FString& PasswordIn, FOnSuccess OnSuccess, FOnFailure OnFailure)
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

	const TFunction<void (FFederationSupportData)> OnApiFederationRequired = [this](const FFederationSupportData& FederationData)
	{
		this->CallFederationRequired(FederationData);
	};

	this->Sessions->PlayFabLogin(UsernameIn, PasswordIn, OnApiSuccess, OnApiFailure, OnApiFederationRequired);
}

void USequenceSessionsBP::PlayFabAuthenticateWithSessionTicket(const FString& SessionTicket, FOnSuccess OnSuccess, FOnFailure OnFailure)
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

	const TFunction<void (FFederationSupportData)> OnApiFederationRequired = [this](const FFederationSupportData& FederationData)
	{
		this->CallFederationRequired(FederationData);
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

void USequenceSessionsBP::SetForceCreateAccount(const bool NewForceCreateAccount)
{
	this->Sessions->SetForceCreateAccount(NewForceCreateAccount);
}

void USequenceSessionsBP::HandleNativeIdToken(const FString& IdToken)
{
	if (this->IdTokenReceived.IsBound())
		this->IdTokenReceived.Broadcast(IdToken);
	else
		SEQ_LOG(Warning, TEXT("Id Token received but event listener was found for IdTokenReceived"));
}

void USequenceSessionsBP::CallOnSessionCreated() const
{
	if (this->OnSessionCreated.IsBound())
		this->OnSessionCreated.Broadcast();
	else
		SEQ_LOG(Warning, TEXT("Session was created but no event listener was found for OnSessionCreated"));
}

void USequenceSessionsBP::CallFederationRequired(const FFederationSupportData& FederationData) const
{
	SEQ_LOG(Display, TEXT("Federation is required. Please login first and link this to the existing account."));
	
	if (this->FederationRequired.IsBound())
		this->FederationRequired.Broadcast(FederationData);
	else
		SEQ_LOG(Warning, TEXT("Federation is required but no event listener was found for FederationRequired"));
}

USequenceSessions* USequenceSessionsBP::GetSequenceSessions() const
{
	return this->Sessions;
}
