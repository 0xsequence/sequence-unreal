// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "Integrators/SequenceSessionsBP.h"

#include "PlayFabResponseIntent.h"
#include "PlayFabSendIntent.h"
#include "RequestHandler.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Util/Log.h"
#include "Util/SequenceSupport.h"

USequenceSessionsBP* USequenceSessionsBP::GetSubSystem()
{
	if (GEngine)
	{
		const TIndirectArray<FWorldContext> Contexts = GEngine->GetWorldContexts();
		for (FWorldContext Context : Contexts)
		{
			if (const UWorld* World = Context.World())
			{
				if (const UGameInstance* GI = UGameplayStatics::GetGameInstance(World))
				{
					if (USequenceSessionsBP* Subsystem = GI->GetSubsystem<USequenceSessionsBP>())
					{
						return Subsystem;
					}
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Error Accessing GEngine"));
	}
	
	UE_LOG(LogTemp, Error,TEXT("Error Accessing USequenceWallet GameInstanceSubSystem"));
	return nullptr;
}

USequenceSessionsBP::USequenceSessionsBP()
{
	this->RPCManager = USequenceRPCManager::Make(false);
	this->Authenticator = NewObject<USequenceAuthenticator>();
}

void USequenceSessionsBP::StartEmailLoginAsync(const FString& Email)
{
	const TFunction<void()> OnSuccess = [this]
	{
		this->CallEmailLoginRequiresCode();
	};

	const FFailureCallback OnFailure = [this](const FSequenceError& Error)
	{
		UE_LOG(LogTemp, Error, TEXT("Email Auth Error: %s"), *Error.Message);
		this->CallSessionCreationFailure(Error.Message);
	};
	
	this->RPCManager->InitEmailAuth(Email.ToLower(),OnSuccess,OnFailure);
}

void USequenceSessionsBP::ConfirmEmailLoginWithCodeAsync(const FString& Code)
{
	const TSuccessCallback<FCredentials_BE> OnSuccess = [this](const FCredentials_BE& Credentials)
	{
		this->StartSession(Credentials);
	};

	const FFailureCallback OnFailure = [this](const FSequenceError& Error)
	{
		UE_LOG(LogTemp, Error, TEXT("Email Auth Error: %s"), *Error.Message);
		this->CallSessionCreationFailure(Error.Message);
	};

	const TFunction<void (FFederationSupportData)> OnFederationRequired = [this](const FFederationSupportData& FederationData)
	{
		UE_LOG(LogTemp, Warning, TEXT("Account Force Create Or Federation Required"));
	};
	
	this->RPCManager->OpenEmailSession(Code, true, OnSuccess, OnFailure, OnFederationRequired);
}

void USequenceSessionsBP::GetGoogleTokenIdAsync()
{
	
}

void USequenceSessionsBP::GetAppleTokenIdAsync()
{
	
}

void USequenceSessionsBP::StartOidcSessionAsync(const FString& IdToken)
{
	const TSuccessCallback<FCredentials_BE> OnSuccess = [this](const FCredentials_BE& Credentials)
	{
		this->StartSession(Credentials);
	};

	const FFailureCallback OnFailure = [this](const FSequenceError& Error)
	{
		UE_LOG(LogTemp, Error, TEXT("OIDC Auth Error: %s"), *Error.Message);
		this->CallSessionCreationFailure(Error.Message);
	};

	const TFunction<void (FFederationSupportData)> OnFederationRequired = [this](const FFederationSupportData& FederationData)
	{
		UE_LOG(LogTemp, Warning, TEXT("Account Force Create Or Federation Required"));
	};
	
	this->RPCManager->OpenOIDCSession(IdToken, true, OnSuccess, OnFailure, OnFederationRequired);
}

void USequenceSessionsBP::PlayFabRegistrationAsync(const FString& UsernameIn, const FString& EmailIn, const FString& PasswordIn)
{
	const TSuccessCallback<FString> OnSuccess = [this](const FString& SessionTicket)
	{
		const TSuccessCallback<FCredentials_BE> OnOpenSuccess = [this](const FCredentials_BE& Credentials)
		{
			this->StartSession(Credentials);
		};

		const FFailureCallback OnOpenFailure = [this](const FSequenceError& Error)
		{
			UE_LOG(LogTemp, Warning, TEXT("Error: %s"), *Error.Message);
			this->CallSessionCreationFailure(Error.Message);
		};

		const TFunction<void (FFederationSupportData)> OnFederationRequired = [this](const FFederationSupportData& FederationData)
		{
			UE_LOG(LogTemp, Warning, TEXT("Account Force Create Or Federation Required"));
		};
		
		this->RPCManager->OpenPlayFabSession(SessionTicket, true, OnOpenSuccess, OnOpenFailure, OnFederationRequired);
	};

	const FFailureCallback OnFailure = [this](const FSequenceError& Error)
	{
		UE_LOG(LogTemp, Warning, TEXT("Error Response: %s"), *Error.Message);
		this->CallSessionCreationFailure(Error.Message);
	};
	
	this->PlayFabNewAccountLoginRpcAsync(UsernameIn, EmailIn, PasswordIn, OnSuccess, OnFailure);
}

void USequenceSessionsBP::PlayFabLoginAsync(const FString& UsernameIn, const FString& PasswordIn)
{
	const TSuccessCallback<FString> OnSuccess = [this](const FString& SessionTicket)
	{
		const TSuccessCallback<FCredentials_BE> OnOpenSuccess = [this](const FCredentials_BE& Credentials)
		{
			this->StartSession(Credentials);
		};

		const FFailureCallback OnOpenFailure = [this](const FSequenceError& Error)
		{
			UE_LOG(LogTemp, Warning, TEXT("Error: %s"), *Error.Message);
			this->CallSessionCreationFailure(Error.Message);
		};

		const TFunction<void (FFederationSupportData)> OnFederationRequired = [this](const FFederationSupportData& FederationData)
		{
			UE_LOG(LogTemp, Warning, TEXT("Account Force Create Or Federation Required"));
		};
		
		this->RPCManager->OpenPlayFabSession(SessionTicket, true, OnOpenSuccess, OnOpenFailure, OnFederationRequired);
	};

	const FFailureCallback OnFailure = [this](const FSequenceError& Error)
	{
		UE_LOG(LogTemp, Warning, TEXT("Error Response: %s"), *Error.Message);
		this->CallSessionCreationFailure(Error.Message);
	};

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

	this->PlayFabLoginRpcAsync(UsernameIn, PasswordIn, OnSuccess, OnFailure);
}

void USequenceSessionsBP::StartGuestSessionAsync()
{
	const TSuccessCallback<FCredentials_BE> OnSuccess = [this](const FCredentials_BE& Credentials)
	{
		this->StartSession(Credentials);
	};

	const FFailureCallback OnFailure = [this](const FSequenceError& Error)
	{
		UE_LOG(LogTemp, Error, TEXT("Guest Auth Error: %s"), *Error.Message);
		this->CallSessionCreationFailure(Error.Message);
	};
	
	this->RPCManager->OpenGuestSession(true, OnSuccess, OnFailure);
}

void USequenceSessionsBP::ClearSession()
{
	this->Authenticator->ClearStoredCredentials();
}

void USequenceSessionsBP::StartSession(const FCredentials_BE& Credentials) const
{
	this->Authenticator->StoreCredentials(Credentials);
	this->CallSessionEstablished();
}

void USequenceSessionsBP::PlayFabNewAccountLoginRpcAsync(const FString& UsernameIn, const FString& EmailIn, const FString& PasswordIn, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure)
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
	
	this->PlayFabRpcAsync(Url, RequestBody, OnSuccessResponse, OnFailure);
}

void USequenceSessionsBP::PlayFabLoginRpcAsync(const FString& UsernameIn, const FString& PasswordIn, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure)
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
	
	this->PlayFabRpcAsync(Url, RequestBody, OnSuccessResponse, OnFailure);
}

void USequenceSessionsBP::PlayFabRpcAsync(const FString& Url, const FString& Content, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure)
{
	NewObject<URequestHandler>()
		->PrepareRequest()
		->WithUrl(Url)
		->WithHeader("Content-type", "application/json")
		->WithVerb("POST")
		->WithContentAsString(Content)
		->ProcessAndThen(OnSuccess, OnFailure);
}

void USequenceSessionsBP::CallEmailLoginRequiresCode() const
{
	if (this->EmailLoginRequiresCode.IsBound())
		this->EmailLoginRequiresCode.Broadcast();
	else
		SEQ_LOG(Error, TEXT("Nothing bound to delegate: EmailLoginRequiresCode"));
}

void USequenceSessionsBP::CallSessionEstablished() const
{
	if (this->SessionEstablished.IsBound())
		this->SessionEstablished.Broadcast();
	else
		SEQ_LOG(Error, TEXT("Nothing bound to delegate: SessionEstablished"));
}

void USequenceSessionsBP::CallSessionCreationFailure(const FString& Error) const
{
	if (this->SessionCreationFailure.IsBound())
		this->SessionCreationFailure.Broadcast(Error);
	else
		SEQ_LOG(Error, TEXT("Nothing bound to delegate: SessionCreationFailure"));
}
