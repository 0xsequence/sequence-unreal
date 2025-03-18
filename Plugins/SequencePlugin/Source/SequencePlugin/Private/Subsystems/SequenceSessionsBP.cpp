// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "Subsystems/SequenceSessionsBP.h"

#include "Errors.h"
#include "Errors.h"
#include "PlayFabResponseIntent.h"
#include "PlayFabSendIntent.h"
#include "RequestHandler.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Native/NativeOAuth.h"
#include "Util/Log.h"
#include "Util/SequenceSupport.h"

USequenceSessionsBP::USequenceSessionsBP()
{
	this->RPCManager = USequenceRPCManager::Make(false);
	this->Authenticator = NewObject<USequenceAuthenticator>();
	this->CredentialsStorage = NewObject<UCredentialsStorage>();
}

bool USequenceSessionsBP::CheckExistingSession() const
{
	const FStoredCredentials_BE Credentials = this->CredentialsStorage->GetStoredCredentials();
	return Credentials.GetValid();
}

void USequenceSessionsBP::GetGoogleTokenId(FOnBrowserRequired BrowserRequired)
{
#if PLATFORM_ANDROID || PLATFORM_IOS
	this->Authenticator->SignInWithGoogleMobile(this);
#else
	const FString SignInUrl = this->Authenticator->GetSigninURL(ESocialSigninType::Google);
	BrowserRequired.ExecuteIfBound(SignInUrl);
#endif
}

void USequenceSessionsBP::GetAppleTokenId(FOnBrowserRequired BrowserRequired)
{
#if PLATFORM_ANDROID || PLATFORM_IOS
	this->Authenticator->SignInWithAppleMobile(this);
#else
	const FString SignInUrl = this->Authenticator->GetSigninURL(ESocialSigninType::Apple);
	BrowserRequired.ExecuteIfBound(SignInUrl);
#endif
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

	this->RPCManager->InitEmailAuth(Email.ToLower(),OnApiSuccess,OnApiFailure);
}

void USequenceSessionsBP::ConfirmEmailLoginWithCode(const FString& Code, FOnSuccess OnSuccess, FOnFailure OnFailure, FOnFederationRequired OnFederationRequired)
{
	const TSuccessCallback<FCredentials_BE> OnApiSuccess = [this, OnSuccess](const FCredentials_BE& Credentials)
	{
		this->CallOnSessionCreated(Credentials);
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

	this->RPCManager->OpenEmailSession(Code, true, OnApiSuccess, OnApiFailure, OnApiFederationRequired);
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
	const TSuccessCallback<FCredentials_BE> OnApiSuccess = [this, OnSuccess](const FCredentials_BE& Credentials)
	{
		this->CallOnSessionCreated(Credentials);
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

	this->RPCManager->OpenOIDCSession(IdToken, true, OnApiSuccess, OnApiFailure, OnApiFederationRequired);
}

void USequenceSessionsBP::PlayFabRegistration(const FString& UsernameIn, const FString& EmailIn, const FString& PasswordIn, FOnSuccess OnSuccess, FOnFailure OnFailure, FOnFederationRequired OnFederationRequired)
{
	const TSuccessCallback<FString> OnApiSuccess = [this, OnSuccess, OnFailure, OnFederationRequired](const FString& SessionTicket)
	{
		const TSuccessCallback<FCredentials_BE> OnOpenSuccess = [this, OnSuccess](const FCredentials_BE& Credentials)
		{
			this->CallOnSessionCreated(Credentials);
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
		
		this->RPCManager->OpenPlayFabSession(SessionTicket, true, OnOpenSuccess, OnOpenFailure, OnApiFederationRequired);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error Response: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};

	this->PlayFabNewAccountLoginRpc(UsernameIn, EmailIn, PasswordIn, OnApiSuccess, OnApiFailure);
}

void USequenceSessionsBP::PlayFabLogin(const FString& UsernameIn, const FString& PasswordIn, FOnSuccess OnSuccess, FOnFailure OnFailure, FOnFederationRequired OnFederationRequired)
{
	const TSuccessCallback<FString> OnApiSuccess = [this, OnSuccess, OnFailure, OnFederationRequired](const FString& SessionTicket)
	{
		PlayFabAuthenticateWithSessionTicket(SessionTicket, OnSuccess, OnFailure, OnFederationRequired);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error Response: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};

	const TFunction<void(FString)> OnSuccessResponse = [OnApiSuccess, OnApiFailure](const FString& Response)
	{
		if (const FPlayFabLoginUserResponse ParsedResponse = USequenceSupport::JSONStringToStruct<FPlayFabLoginUserResponse>(Response); ParsedResponse.IsValid())
		{
			OnApiSuccess(ParsedResponse.Data.SessionTicket);
		}
		else
		{
			OnApiFailure(FSequenceError(ResponseParseError,Response));
		}
	};

	this->PlayFabLoginRpc(UsernameIn, PasswordIn, OnApiSuccess, OnApiFailure);
}

void USequenceSessionsBP::PlayFabAuthenticateWithSessionTicket(const FString& SessionTicket, FOnSuccess OnSuccess, FOnFailure OnFailure, FOnFederationRequired OnFederationRequired)
{
	const TSuccessCallback<FCredentials_BE> OnOpenSuccess = [this, OnSuccess](const FCredentials_BE& Credentials)
	{
		this->CallOnSessionCreated(Credentials);
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

	this->RPCManager->OpenPlayFabSession(SessionTicket, true, OnOpenSuccess, OnOpenFailure, OnApiFederationRequired);
}

void USequenceSessionsBP::StartGuestSession(FOnSuccess OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FCredentials_BE> OnApiSuccess = [this, OnSuccess](const FCredentials_BE& Credentials)
	{
		this->CallOnSessionCreated(Credentials);
		OnSuccess.ExecuteIfBound();
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Guest Auth Error: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};

	this->RPCManager->OpenGuestSession(true, OnApiSuccess, OnApiFailure);
}

void USequenceSessionsBP::FederateEmail(const FString& EmailIn, FOnSuccess OnSuccess, FOnFailure OnFailure)
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

	this->RPCManager->InitEmailFederation(EmailIn.ToLower(),OnApiSuccess,OnApiFailure);
}

void USequenceSessionsBP::ConfirmEmailFederationWithCode(const FString& Code, FOnSuccess OnSuccess, FOnFailure OnFailure)
{
	if (FStoredCredentials_BE StoredCredentials = this->CredentialsStorage->GetStoredCredentials(); StoredCredentials.GetValid())
	{
		const TFunction<void()> OnApiSuccess = [this, OnSuccess]()
		{
			OnSuccess.ExecuteIfBound();
		};

		const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
		{
			OnFailure.ExecuteIfBound(Error.Message);
		};
		
		this->RPCManager->FederateEmailSession(StoredCredentials.GetCredentials().GetWalletAddress(), Code, OnApiSuccess, OnApiFailure);
	}
	else
	{
		SEQ_LOG(Error, TEXT("StoredCredentials are invalid, please login"));
		OnFailure.ExecuteIfBound("StoredCredentials are invalid, please login");
	}
}

void USequenceSessionsBP::FederateOidcToken(const FString& IdTokenIn, FOnSuccess OnSuccess, FOnFailure OnFailure)
{
	if (FStoredCredentials_BE StoredCredentials = this->CredentialsStorage->GetStoredCredentials(); StoredCredentials.GetValid())
	{
		const TFunction<void()> OnApiSuccess = [this, OnSuccess]()
		{
			OnSuccess.ExecuteIfBound();
		};

		const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
		{
			OnFailure.ExecuteIfBound(Error.Message);
		};
		
		this->RPCManager->FederateOIDCSession(StoredCredentials.GetCredentials().GetWalletAddress(),IdTokenIn, OnApiSuccess, OnApiFailure);
	}
	else
	{
		SEQ_LOG(Error, TEXT("StoredCredentials are invalid, please login"));
		OnFailure.ExecuteIfBound("StoredCredentials are invalid, please login");
	}
}

void USequenceSessionsBP::FederatePlayFabRegistration(const FString& UsernameIn, const FString& EmailIn, const FString& PasswordIn, FOnSuccess OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FString> OnApiSuccess = [this, OnSuccess, OnFailure](const FString& SessionTicket)
	{
		const TFunction<void()> OnFederateSuccess = [OnSuccess]()
		{
			OnSuccess.ExecuteIfBound();
		};

		const FFailureCallback OnFederateFailure = [OnFailure](const FSequenceError& Error)
		{
			SEQ_LOG(Error, TEXT("Error Federating PlayFab Account: %s"), *Error.Message);
			OnFailure.ExecuteIfBound(Error.Message);
		};

		if (FStoredCredentials_BE StoredCredentials = this->CredentialsStorage->GetStoredCredentials(); StoredCredentials.GetValid())
		{
			this->RPCManager->FederatePlayFabSession(StoredCredentials.GetCredentials().GetWalletAddress(), SessionTicket, OnFederateSuccess, OnFederateFailure);
		}
		else
		{
			const FString ErrorMessage = TEXT("StoredCredentials are invalid, please login");
			SEQ_LOG(Warning, TEXT("Error: %s"), *ErrorMessage);
			OnFailure.ExecuteIfBound(ErrorMessage);
		}
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Warning, TEXT("Error Federating PlayFab Account: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};

	this->PlayFabNewAccountLoginRpc(UsernameIn, EmailIn, PasswordIn, OnApiSuccess, OnApiFailure);
}

void USequenceSessionsBP::FederatePlayFabLogin(const FString& UsernameIn, const FString& PasswordIn, FOnSuccess OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FString> OnApiSuccess = [this, OnSuccess, OnFailure](const FString& SessionTicket)
	{
		const TFunction<void()> OnFederateSuccess = [OnSuccess]()
		{
			OnSuccess.ExecuteIfBound();
		};

		const FFailureCallback OnFederateFailure = [OnFailure](const FSequenceError& Error)
		{
			SEQ_LOG(Warning, TEXT("Error Federating PlayFab Account: %s"), *Error.Message);
			OnFailure.ExecuteIfBound(Error.Message);
		};
		
		if (FStoredCredentials_BE StoredCredentials = this->CredentialsStorage->GetStoredCredentials(); StoredCredentials.GetValid())
		{
			this->RPCManager->FederatePlayFabSession(StoredCredentials.GetCredentials().GetWalletAddress(), SessionTicket, OnFederateSuccess, OnFederateFailure);
		}
		else
		{
			const FString ErrorMessage = TEXT("StoredCredentials are invalid, please login");
			SEQ_LOG(Warning, TEXT("Error: %s"), *ErrorMessage);
			OnFailure.ExecuteIfBound(ErrorMessage);
		}
	};

	const FFailureCallback OnApiFailure = [OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Warning, TEXT("Error Federating PlayFab Account: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};

	this->PlayFabLoginRpc(UsernameIn, PasswordIn, OnApiSuccess, OnApiFailure);
}

void USequenceSessionsBP::PlayFabNewAccountLoginRpc(const FString& UsernameIn, const FString& EmailIn, const FString& PasswordIn, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure)
{
	const TFunction<void(FString)> OnSuccessResponse = [OnSuccess, OnFailure](const FString& Response)
	{
		if (const FPlayFabRegisterUserResponse ParsedResponse = USequenceSupport::JSONStringToStruct<FPlayFabRegisterUserResponse>(Response); ParsedResponse.IsValid())
		{
			OnSuccess(ParsedResponse.Data.SessionTicket);
		}
		else
		{
			OnFailure(FSequenceError(ResponseParseError,Response));
		}
	};

	const FString TitleId = UConfigFetcher::GetConfigVar(UConfigFetcher::PlayFabTitleID);
	const FPlayFabRegisterUser RegisterUser(TitleId, EmailIn, PasswordIn, UsernameIn);
	const FString RequestBody = USequenceSupport::StructToPartialSimpleString(RegisterUser);
	const FString Url = "https://" + TitleId + ".playfabapi.com/Client/RegisterPlayFabUser";
	
	this->PlayFabRpc(Url, RequestBody, OnSuccessResponse, OnFailure);
}

void USequenceSessionsBP::PlayFabLoginRpc(const FString& UsernameIn, const FString& PasswordIn, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure)
{
	const TFunction<void(FString)> OnSuccessResponse = [OnSuccess, OnFailure](const FString& Response)
	{
		if (const FPlayFabLoginUserResponse ParsedResponse = USequenceSupport::JSONStringToStruct<FPlayFabLoginUserResponse>(Response); ParsedResponse.IsValid())
		{
			OnSuccess(ParsedResponse.Data.SessionTicket);
		}
		else
		{
			OnFailure(FSequenceError(ResponseParseError,Response));
		}
	};

	const FString TitleId = UConfigFetcher::GetConfigVar(UConfigFetcher::PlayFabTitleID);
	const FPlayFabLoginUser LoginUser(PasswordIn,TitleId,UsernameIn);
	const FString RequestBody = USequenceSupport::StructToPartialSimpleString(LoginUser);
	const FString Url = "https://" + TitleId + ".playfabapi.com/Client/LoginWithPlayFab";
	
	this->PlayFabRpc(Url, RequestBody, OnSuccessResponse, OnFailure);
}

void USequenceSessionsBP::PlayFabRpc(const FString& Url, const FString& Content, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure)
{
	NewObject<URequestHandler>()
		->PrepareRequest()
		->WithUrl(Url)
		->WithHeader("Content-type", "application/json")
		->WithVerb("POST")
		->WithContentAsString(Content)
		->ProcessAndThen(OnSuccess, OnFailure);
}

void USequenceSessionsBP::CallOnSessionCreated(const FCredentials_BE& Credentials) const
{
	this->CredentialsStorage->StoreCredentials(Credentials);
	if (this->OnSessionCreated.IsBound())
	{
		this->OnSessionCreated.Broadcast();
	}
}
