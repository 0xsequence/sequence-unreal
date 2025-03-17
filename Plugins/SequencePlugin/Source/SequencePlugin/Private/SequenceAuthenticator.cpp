// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "SequenceAuthenticator.h"
#include "Misc/Guid.h"
#include "Misc/Base64.h"
#include "StorableCredentials.h"
#include "Kismet/GameplayStatics.h"
#include "Util/SequenceSupport.h"
#include "SequenceEncryptor.h"
#include "IWebBrowserCookieManager.h"
#include "IWebBrowserSingleton.h"
#include "PlayFabSendIntent.h"
#include "RequestHandler.h"
#include "WebBrowserModule.h"
#include "SequenceRPCManager.h"
#include "Native/NativeOAuth.h"
#include "NativeEncryptors/AppleEncryptor.h"
#include "NativeEncryptors/AndroidEncryptor.h"
#include "NativeEncryptors/WindowsEncryptor.h"
#include "PlayFabResponseIntent.h"
#include "Sequence/SequenceAPI.h"
#include "Util/Log.h"

USequenceAuthenticator::USequenceAuthenticator()
{
	this->StateToken = FGuid::NewGuid().ToString();
	this->SequenceRPCManager = USequenceRPCManager::Make(false);
	this->Validator = SequenceRPCManager->Validator;

	if constexpr (PLATFORM_ANDROID)
	{
		this->Encryptor = NewObject<UAndroidEncryptor>();
	}
	else if constexpr (PLATFORM_MAC)
	{
		this->Encryptor = NewObject<UAppleEncryptor>();
	}
	else if constexpr (PLATFORM_WINDOWS)
	{
		this->Encryptor = NewObject<UWindowsEncryptor>();
	}
	else if constexpr (PLATFORM_IOS)
	{
		this->Encryptor = NewObject<UAppleEncryptor>();
	}
}

void USequenceAuthenticator::InitiateMobileSSO_Internal(const ESocialSigninType& Type)
{
	switch (Type)
	{
		case ESocialSigninType::Apple:
			this->SignInWithAppleMobile(this);
			break;
		case ESocialSigninType::Google:
			this->SignInWithGoogleMobile(this);
			break;
		case ESocialSigninType::FaceBook:
			SEQ_LOG(Error, TEXT("Facebook Login is not supported yet."))
			break;
		case ESocialSigninType::Discord:
			SEQ_LOG(Error, TEXT("Discord Login is not supported yet."))
			break;
	}
}

void USequenceAuthenticator::SignInWithGoogleMobile(INativeAuthCallback* CallbackHandler)
{
#if PLATFORM_ANDROID
	NativeOAuth::SignInWithGoogle(UConfigFetcher::GetConfigVar(UConfigFetcher::GoogleClientID), CallbackHandler);
#elif PLATFORM_IOS
	NativeOAuth::SignInWithGoogle_IOS(this->GetSigninURL(ESocialSigninType::Google),UrlScheme,CallbackHandler);
#endif
}

void USequenceAuthenticator::SignInWithAppleMobile(INativeAuthCallback* CallbackHandler)
{
#if PLATFORM_ANDROID
	const FString RequestUrl = GenerateSigninURL(ESocialSigninType::Apple);
	const FString RedirectUrl = GenerateRedirectURL(ESocialSigninType::Apple);
	NativeOAuth::RequestAuthWebView(RequestUrl, RedirectUrl, CallbackHandler);
#elif PLATFORM_IOS
	const FString ClientId = UrlScheme + "---" + this->StateToken + UEnum::GetValueAsString(ESocialSigninType::Apple) + "&client_id=" + UConfigFetcher::GetConfigVar(UConfigFetcher::AppleClientID);
	NativeOAuth::SignInWithApple(ClientId, CallbackHandler);
#endif
}

void USequenceAuthenticator::HandleNativeIdToken(const FString& IdToken)
{
	this->UpdateMobileLogin_IdToken(IdToken);
}

void USequenceAuthenticator::SetIsForcing(const bool IsForcingIn)
{
	this->IsForcing = IsForcingIn;
}

void USequenceAuthenticator::SetIsFederating(const bool IsFederatingIn)
{
	this->IsFederating = IsFederatingIn;
}

void USequenceAuthenticator::SetIsFederatingSessionInUse()
{
	this->IsFederatingSessionInUse = true;
}

bool USequenceAuthenticator::ReadAndResetIsForcing()
{
	const bool Cached_Force_State = this->IsForcing;
	this->IsForcing = false;
	return Cached_Force_State;
}

bool USequenceAuthenticator::ReadAndResetIsFederating()
{
	const bool Cached_Federate_State = this->IsFederating;
	this->IsFederating = false;
	return Cached_Federate_State;
}

bool USequenceAuthenticator::ReadAndResetIsFederatingSessionInUse()
{
	const bool Cached_FederateSessionInUse_State = this->IsFederatingSessionInUse;
	this->IsFederatingSessionInUse = false;
	return Cached_FederateSessionInUse_State;
}

void USequenceAuthenticator::CheckAndFederateSessionInUse()
{
	if (this->ReadAndResetIsFederatingSessionInUse())
	{
		FStoredCredentials_BE StoredCredentials = this->GetStoredCredentials();

		if (StoredCredentials.GetValid())
		{
			const TFunction<void()> OnSuccess = [this]()
			{
				this->CallFederateSuccess();
			};

			const FFailureCallback OnFailure = [this](const FSequenceError& Error)
			{
				this->CallFederateFailure(Error.Message);
			};

			
			this->SequenceRPCManager->FederateSessionInUse(StoredCredentials.GetCredentials().GetWalletAddress(), OnSuccess, OnFailure);
		}
		else
		{
			this->CallFederateFailure(TEXT("Failed to Federate Session in use"));
		}
	}
}

void USequenceAuthenticator::AuthenticateUsingPlayfabSessionTicket(const FString& SessionTicket, const bool ForceCreateAccountIn)
{
	const TSuccessCallback<FCredentials_BE> OnOpenSuccess = [this](const FCredentials_BE& Credentials)
	{
		this->InitializeSequence(Credentials);
		this->CheckAndFederateSessionInUse();
	};

	const FFailureCallback OnOpenFailure = [this](const FSequenceError& Error)
	{
		UE_LOG(LogTemp, Warning, TEXT("Error: %s"), *Error.Message);
		this->ResetFederateSessionInUse();
		this->CallAuthFailure(Error.Message);
	};

	const TFunction<void (FFederationSupportData)> OnFederationRequired = [this](const FFederationSupportData& FederationData)
	{
		UE_LOG(LogTemp, Warning, TEXT("Account Force Create Or Federation Required"));
		this->SetIsFederatingSessionInUse();
		this->CallFederateOrForce(FederationData);
	};
		
	this->SequenceRPCManager->OpenPlayFabSession(SessionTicket,ForceCreateAccountIn, OnOpenSuccess, OnOpenFailure, OnFederationRequired);
}

void USequenceAuthenticator::ResetFederateSessionInUse()
{
	this->IsFederatingSessionInUse = false;
}

void USequenceAuthenticator::SetCustomEncryptor(UGenericNativeEncryptor * EncryptorIn)
{
	this->Encryptor = EncryptorIn;
	if (this->Encryptor)
	{
		const FString EncryptorName = this->Encryptor->GetClass()->GetName();
		UE_LOG(LogTemp,Display,TEXT("Setting custom encryptor to: %s"),*EncryptorName);
	}
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("Received null instead of a pointer to an Encryptor Object using fallback encryptor"));	
	}
}

void USequenceAuthenticator::ClearStoredCredentials() const
{
	const FCredentials_BE BlankCredentials;
	this->StoreCredentials(BlankCredentials);
}

void USequenceAuthenticator::StoreCredentials(const FCredentials_BE& Credentials) const
{
	if (UStorableCredentials* StorableCredentials = Cast<UStorableCredentials>(UGameplayStatics::CreateSaveGameObject(UStorableCredentials::StaticClass())))
	{
		const FString CTS_Json = USequenceSupport::StructToString<FCredentials_BE>(Credentials);
		const int32 CTS_Json_Length = CTS_Json.Len();

		if (Encryptor)
		{//Use set encryptor
			StorableCredentials->EK = this->Encryptor->Encrypt(CTS_Json);
			StorableCredentials->KL = CTS_Json_Length;
		}
		else
		{//Use the fallback
			StorableCredentials->EK = USequenceEncryptor::Encrypt(CTS_Json);
			StorableCredentials->KL = CTS_Json_Length;
		}

		if (UGameplayStatics::SaveGameToSlot(StorableCredentials, this->SaveSlot, this->UserIndex))
		{
			UE_LOG(LogTemp, Display, TEXT("Data Saved Correctly!"));
		}
	}
}

FString USequenceAuthenticator::BuildRedirectPrefix() const
{
	const FString Redirect = UConfigFetcher::GetConfigVar(UConfigFetcher::RedirectUrl);
	if (Redirect.EndsWith(TEXT("/"),ESearchCase::IgnoreCase))
		return Redirect + this->RedirectPrefixTrailer;
	else
		return Redirect + "/" + this->RedirectPrefixTrailer;
}

bool USequenceAuthenticator::GetStoredCredentials(FCredentials_BE* Credentials) const
{
	bool ret = false;

	//This line crashes the engine if Cr.sav is modified externally in anyway
	
	const USaveGame * SaveGame = UGameplayStatics::LoadGameFromSlot(this->SaveSlot, this->UserIndex);

	if (SaveGame != nullptr)
	{
		if (const UStorableCredentials* LoadedCredentials = Cast<UStorableCredentials>(SaveGame))
		{
			FString CTR_Json = "";

			if (Encryptor)
			{//Use set encryptor
				CTR_Json = Encryptor->Decrypt(LoadedCredentials->EK);
			}
			else
			{//Use the fallback
				CTR_Json = USequenceEncryptor::Decrypt(LoadedCredentials->EK, LoadedCredentials->KL);
			}

			ret = USequenceSupport::JSONStringToStruct<FCredentials_BE>(CTR_Json, Credentials);
			ret &= Credentials->RegisteredValid();

			if (ret == false)
			{
				// Assumed that there is an issue with the save file, therefore we'll just delete the save file
				UE_LOG(LogTemp, Error, TEXT("[System Failure: Unable to read save file or file is corrupted]"));
				UGameplayStatics::DeleteGameInSlot(this->SaveSlot, this->UserIndex);
			}
		}
	}
	return ret;
}

void USequenceAuthenticator::CallAuthRequiresCode() const
{
	if (this->AuthRequiresCode.IsBound())
		this->AuthRequiresCode.Broadcast();
	else
		UE_LOG(LogTemp, Error, TEXT("[System Failure: nothing bound to delegate: AuthRequiresCode]"));
}

void USequenceAuthenticator::CallAuthFailure(const FString& ErrorMessageIn) const
{
	if (this->AuthFailure.IsBound())
		this->AuthFailure.Broadcast(ErrorMessageIn);
	else
		UE_LOG(LogTemp, Error, TEXT("[System Error: nothing bound to delegate: AuthFailure]"));
}

void USequenceAuthenticator::CallAuthSuccess() const
{
	if (this->AuthSuccess.IsBound())
		this->AuthSuccess.Broadcast();
	else
		UE_LOG(LogTemp, Error, TEXT("[System Error: nothing bound to delegate: AuthSuccess]"));
}

void USequenceAuthenticator::CallFederateSuccess() const
{
	if (this->FederateSuccess.IsBound())
	{
		this->FederateSuccess.Broadcast();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[System Error: nothing bound to delegate: FederateSuccess]"));
	}
}

void USequenceAuthenticator::CallFederateFailure(const FString& ErrorMessageIn) const
{
	if (this->FederateFailure.IsBound())
	{
		this->FederateFailure.Broadcast(ErrorMessageIn);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[System Error: nothing bound to delegate: FederateFailure], Captured Error Message: %s"), *ErrorMessageIn);
	}
}

void USequenceAuthenticator::CallFederateOrForce(const FFederationSupportData& FederationData) const
{
	if (this->FederateOrForce.IsBound())
	{
		this->FederateOrForce.Broadcast(FederationData);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[System Error: nothing bound to delegate: FederateOrForce]"));
	}
}

void USequenceAuthenticator::UpdateMobileLogin(const FString& TokenizedUrl)
{
	const FString IdToken = NativeOAuth::GetIdTokenFromTokenizedUrl(TokenizedUrl);
	UpdateMobileLogin_IdToken(IdToken);
}

void USequenceAuthenticator::UpdateMobileLogin_IdToken(const FString& IdTokenIn)
{
	if (this->ReadAndResetIsFederating())
	{
		FederateOIDCIdToken(IdTokenIn);
	}
	else
	{
		SocialLogin(IdTokenIn, this->ReadAndResetIsForcing());
	}
}

void USequenceAuthenticator::InitiateMobileSSO(const ESocialSigninType& Type, const bool ForceCreateAccountIn)
{
	this->SetIsForcing(ForceCreateAccountIn);
	this->SetIsFederating(false);
	this->InitiateMobileSSO_Internal(Type);
}

FString USequenceAuthenticator::GetSigninURL(const ESocialSigninType& Type) const
{
	FString SigninURL = "";
	
	if (this->SSOProviderMap.Contains(Type))
	{
		SigninURL = this->GenerateSigninURL(Type);
	}
	else
	{
		const FString SSOType = UEnum::GetValueAsString(Type);
		UE_LOG(LogTemp, Error, TEXT("No Entry for SSO type: [%s] in SSOProviderMap"),*SSOType);
	}

	//clear web cache here so signin will be clean each time!
	if (this->PurgeCache)
	{
		if (const IWebBrowserSingleton* WebBrowserSingleton = IWebBrowserModule::Get().GetSingleton())
		{
			const TSharedPtr<IWebBrowserCookieManager> CookieManager = WebBrowserSingleton->GetCookieManager();
			if (CookieManager.IsValid())
			{
				CookieManager->DeleteCookies();
			}
		}
	}
	return SigninURL;
}

void USequenceAuthenticator::SocialLogin(const FString& IDTokenIn, const bool ForceCreateAccountIn)
{
	if (ForceCreateAccountIn)
	{
		this->ResetFederateSessionInUse();
	}
	
	const TSuccessCallback<FCredentials_BE> OnSuccess = [this](const FCredentials_BE& Credentials)
	{
		this->InitializeSequence(Credentials);
		this->CheckAndFederateSessionInUse();
	};

	const FFailureCallback OnFailure = [this](const FSequenceError& Error)
	{
		UE_LOG(LogTemp, Error, TEXT("OIDC Auth Error: %s"), *Error.Message);
		this->ResetFederateSessionInUse();
		this->CallAuthFailure(Error.Message);
	};

	const TFunction<void (FFederationSupportData)> OnFederationRequired = [this](const FFederationSupportData& FederationData)
	{
		UE_LOG(LogTemp, Warning, TEXT("Account Force Create Or Federation Required"));
		this->SetIsFederatingSessionInUse();
		this->CallFederateOrForce(FederationData);
	};
	
	this->SequenceRPCManager->OpenOIDCSession(IDTokenIn, ForceCreateAccountIn, OnSuccess, OnFailure, OnFederationRequired);
}

void USequenceAuthenticator::EmailLogin(const FString& EmailIn, const bool ForceCreateAccountIn)
{
	this->SetIsForcing(ForceCreateAccountIn);
	this->SetIsFederating(false);

	if (ForceCreateAccountIn)
	{
		this->ResetFederateSessionInUse();
	}
	
	const TFunction<void()> OnSuccess = [this]
	{
		this->CallAuthRequiresCode();
	};

	const FFailureCallback OnFailure = [this](const FSequenceError& Error)
	{
		UE_LOG(LogTemp, Error, TEXT("Email Auth Error: %s"), *Error.Message);
		this->CallAuthFailure(Error.Message);
	};
	
	this->SequenceRPCManager->InitEmailAuth(EmailIn.ToLower(),OnSuccess,OnFailure);
}

void USequenceAuthenticator::GuestLogin(const bool ForceCreateAccountIn)
{
	if (ForceCreateAccountIn)
	{
		this->ResetFederateSessionInUse();
	}
	
	const TSuccessCallback<FCredentials_BE> OnSuccess = [this](const FCredentials_BE& Credentials)
	{
		this->InitializeSequence(Credentials);
	};

	const FFailureCallback OnFailure = [this](const FSequenceError& Error)
	{
		UE_LOG(LogTemp, Error, TEXT("Guest Auth Error: %s"), *Error.Message);
		this->CallAuthFailure(Error.Message);
	};
	
	this->SequenceRPCManager->OpenGuestSession(ForceCreateAccountIn,OnSuccess,OnFailure);
}

void USequenceAuthenticator::PlayFabRegisterAndLogin(const FString& UsernameIn, const FString& EmailIn, const FString& PasswordIn, const bool ForceCreateAccountIn)
{
	if (ForceCreateAccountIn)
	{
		this->ResetFederateSessionInUse();
	}
	
	const TSuccessCallback<FString> OnSuccess = [this, ForceCreateAccountIn](const FString& SessionTicket)
	{
		const TSuccessCallback<FCredentials_BE> OnOpenSuccess = [this](const FCredentials_BE& Credentials)
		{
			this->InitializeSequence(Credentials);
			this->CheckAndFederateSessionInUse();
		};

		const FFailureCallback OnOpenFailure = [this](const FSequenceError& Error)
		{
			UE_LOG(LogTemp, Warning, TEXT("Error: %s"), *Error.Message);
			this->ResetFederateSessionInUse();
			this->CallAuthFailure(Error.Message);
		};

		const TFunction<void (FFederationSupportData)> OnFederationRequired = [this](const FFederationSupportData& FederationData)
		{
			UE_LOG(LogTemp, Warning, TEXT("Account Force Create Or Federation Required"));
			this->SetIsFederatingSessionInUse();
			this->CallFederateOrForce(FederationData);
		};
		
		this->SequenceRPCManager->OpenPlayFabSession(SessionTicket,ForceCreateAccountIn, OnOpenSuccess, OnOpenFailure, OnFederationRequired);
	};

	const FFailureCallback OnFailure = [this](const FSequenceError& Error)
	{
		UE_LOG(LogTemp, Warning, TEXT("Error Response: %s"), *Error.Message);
		this->CallAuthFailure(Error.Message);
	};
	
	this->PlayFabNewAccountLoginRPC(UsernameIn, EmailIn, PasswordIn, OnSuccess, OnFailure);
}

void USequenceAuthenticator::PlayFabLogin(const FString& UsernameIn, const FString& PasswordIn, const bool ForceCreateAccountIn)
{
	if (ForceCreateAccountIn)
	{
		this->ResetFederateSessionInUse();
	}
	
	const TSuccessCallback<FString> OnSuccess = [this, ForceCreateAccountIn](const FString& SessionTicket)
	{
		this->AuthenticateUsingPlayfabSessionTicket(SessionTicket, ForceCreateAccountIn);
	};

	const FFailureCallback OnFailure = [this](const FSequenceError& Error)
	{
		UE_LOG(LogTemp, Warning, TEXT("Error Response: %s"), *Error.Message);
		this->CallAuthFailure(Error.Message);
	};
	
	this->PlayFabLoginRPC(UsernameIn, PasswordIn, OnSuccess, OnFailure);
}

void USequenceAuthenticator::PlayFabAuthenticateWithSessionTicket(const FString& SessionTicket)
{
	AuthenticateUsingPlayfabSessionTicket(SessionTicket, false);
}

FString USequenceAuthenticator::GenerateRedirectURL(const ESocialSigninType& Type) const
{
	FString RedirectUrl = this->BuildRedirectPrefix() + "&scope=openid+email&state=" + UrlScheme + "---" + this->StateToken + UEnum::GetValueAsString(Type);
	switch (Type)
	{
	case ESocialSigninType::Google:
		break;
	case ESocialSigninType::Apple:
		RedirectUrl = this->BuildRedirectPrefix();
		break;
	case ESocialSigninType::FaceBook:
		break;
	case ESocialSigninType::Discord:
		break;
	}
	return RedirectUrl;
}

FString USequenceAuthenticator::GenerateSigninURL(const ESocialSigninType& Type) const
{
	const FString AuthClientId = SSOProviderMap[Type].ClientID;
	const FString AuthUrl = SSOProviderMap[Type].URL;
	FString SigninUrl = AuthUrl +"?response_type=code+id_token&client_id="+ AuthClientId +"&redirect_uri="+ this->BuildRedirectPrefix() + "&scope=openid+email&state=" + UrlScheme + "---" + this->StateToken + UEnum::GetValueAsString(Type);
	switch (Type)
	{
	case ESocialSigninType::Google:
		break;
	case ESocialSigninType::Apple://For apple we have no scope, as well as the trailing response_mode
		SigninUrl = AuthUrl +"?response_type=code+id_token&client_id="+ AuthClientId +"&redirect_uri="+ this->BuildRedirectPrefix() + "&state=" + UrlScheme + "---" + this->StateToken + UEnum::GetValueAsString(Type);
		SigninUrl += "&response_mode=fragment";
		break;
	case ESocialSigninType::FaceBook:
		break;
	case ESocialSigninType::Discord:
		break;
	}
	return SigninUrl;
}

void USequenceAuthenticator::InitializeSequence(const FCredentials_BE& Credentials) const
{
	if (const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Credentials); WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		this->StoreCredentials(Credentials);
		this->CallAuthSuccess();
	}
	else
	{
		this->CallAuthFailure(TEXT("Failed to Initialize SequenceWallet"));
	}
}

void USequenceAuthenticator::PlayFabLoginRPC(const FString& UsernameIn, const FString& PasswordIn, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure)
{
	if (!USequenceAuthenticator::ValidateUsername(UsernameIn).IsEmpty())
	{
		OnFailure(FSequenceError(InvalidArgument, USequenceAuthenticator::ValidateUsername(UsernameIn)));
		return;
	}
	if (!USequenceAuthenticator::ValidatePassword(PasswordIn).IsEmpty())
	{
		OnFailure(FSequenceError(InvalidArgument, USequenceAuthenticator::ValidatePassword(PasswordIn)));
		return;
	}
	
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
	
	PlayFabRPC(GeneratePlayFabUrl(), RequestBody, OnSuccessResponse, OnFailure);
}

void USequenceAuthenticator::PlayFabNewAccountLoginRPC(const FString& UsernameIn, const FString& EmailIn, const FString& PasswordIn, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure)
{
	if (!USequenceAuthenticator::ValidateUsername(UsernameIn).IsEmpty())
	{
		OnFailure(FSequenceError(InvalidArgument, USequenceAuthenticator::ValidateUsername(UsernameIn)));
		return;
	}
	if (!USequenceAuthenticator::ValidatePassword(PasswordIn).IsEmpty())
	{
		OnFailure(FSequenceError(InvalidArgument, USequenceAuthenticator::ValidatePassword(PasswordIn)));
		return;
	}
	if (!USequenceAuthenticator::ValidateEmail(EmailIn).IsEmpty())
	{
		OnFailure(FSequenceError(InvalidArgument, USequenceAuthenticator::ValidateEmail(EmailIn)));
		return;
	}
	
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
	
	PlayFabRPC(GeneratePlayFabRegisterUrl(), RequestBody, OnSuccessResponse, OnFailure);
}

FString USequenceAuthenticator::GeneratePlayFabUrl()
{
	const FString TitleId = UConfigFetcher::GetConfigVar(UConfigFetcher::PlayFabTitleID);
	return "https://" + TitleId + ".playfabapi.com/Client/LoginWithPlayFab";
}

FString USequenceAuthenticator::GeneratePlayFabRegisterUrl()
{
	const FString TitleId = UConfigFetcher::GetConfigVar(UConfigFetcher::PlayFabTitleID);
	return "https://" + TitleId + ".playfabapi.com/Client/RegisterPlayFabUser";
}

void USequenceAuthenticator::PlayFabRPC(const FString& Url, const FString& Content, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure)
{
	URequestHandler* RequestHandler = NewObject<URequestHandler>();

	RequestHandler->PrepareRequest()
		->WithUrl(Url)
		->WithHeader("Content-type", "application/json")
		->WithVerb("POST")
		->WithContentAsString(Content);

	RequestHandler->ProcessAndThen(*Validator,OnSuccess, OnFailure, false);
}

FString USequenceAuthenticator::ValidateUsername(const FString& Username)
{
	if (Username.IsEmpty())
	{
		return "Username cannot be empty";
	}
	return "";
}

FString USequenceAuthenticator::ValidateEmail(const FString& Email)
{
	if (Email.IsEmpty())
	{
		return "Email cannot be empty";
	}

	int32 AtIndex;

	if (!Email.FindChar('@', AtIndex) || AtIndex == 0)
	{
		return TEXT("Email is invalid, given " + Email);
	}

	if (!Email.FindChar('.', AtIndex) || AtIndex == 0)
	{
		return TEXT("Email is invalid, given " + Email);
	}

	
	return "";
}

FString USequenceAuthenticator::ValidatePassword(const FString& Password)
{
	if (Password.IsEmpty())
	{
		return "Password cannot be empty";
	}
	if (Password.Len() < 8)
	{
		return "Password must be at least 8 characters long";
	}
	return "";
}

void USequenceAuthenticator::EmailLoginCode(const FString& CodeIn)
{	
	if (this->ReadAndResetIsFederating())
	{
		const TFunction<void()> OnSuccess = [this]()
		{
			this->CallFederateSuccess();
		};

		const FFailureCallback OnFailure = [this](const FSequenceError& Error)
		{
			this->CallFederateFailure(Error.Message);
		};
		
		if (FStoredCredentials_BE StoredCredentials = this->GetStoredCredentials(); StoredCredentials.GetValid())
		{
			this->SequenceRPCManager->FederateEmailSession(StoredCredentials.GetCredentials().GetWalletAddress(), CodeIn, OnSuccess, OnFailure);
		}
		else
		{
			const FString ErrorMessage = TEXT("StoredCredentials are invalid, please login");
			UE_LOG(LogTemp, Warning, TEXT("Error: %s"), *ErrorMessage);
			this->CallFederateFailure(ErrorMessage);
		}
	}
	else
	{
		const TSuccessCallback<FCredentials_BE> OnSuccess = [this](const FCredentials_BE& Credentials)
		{
			this->InitializeSequence(Credentials);
			this->CheckAndFederateSessionInUse();
		};

		const FFailureCallback OnFailure = [this](const FSequenceError& Error)
		{
			UE_LOG(LogTemp, Error, TEXT("Email Auth Error: %s"), *Error.Message);
			this->ResetFederateSessionInUse();
			this->CallAuthFailure(Error.Message);
		};

		const TFunction<void (FFederationSupportData)> OnFederationRequired = [this](const FFederationSupportData& FederationData)
		{
			UE_LOG(LogTemp, Warning, TEXT("Account Force Create Or Federation Required"));
			this->SetIsFederatingSessionInUse();
			this->CallFederateOrForce(FederationData);
		};
		
		this->SequenceRPCManager->OpenEmailSession(CodeIn, this->ReadAndResetIsForcing(), OnSuccess, OnFailure, OnFederationRequired);
	}
}

void USequenceAuthenticator::FederateEmail(const FString& EmailIn)
{
	this->SetIsFederating(true);

	const TFunction<void()> OnSuccess = [this]
	{
		this->CallAuthRequiresCode();
	};

	const FFailureCallback OnFailure = [this](const FSequenceError& Error)
	{
		UE_LOG(LogTemp, Error, TEXT("Email Auth Error: %s"), *Error.Message);
		this->CallFederateFailure(Error.Message);
	};

	this->SequenceRPCManager->InitEmailFederation(EmailIn.ToLower(),OnSuccess,OnFailure);
}

void USequenceAuthenticator::FederateOIDCIdToken(const FString& IdTokenIn)
{
	const TFunction<void()> OnSuccess = [this]()
	{
		this->CallFederateSuccess();
	};

	const FFailureCallback OnFailure = [this](const FSequenceError& Error)
	{
		this->CallFederateFailure(Error.Message);
	};

	if (FStoredCredentials_BE StoredCredentials = this->GetStoredCredentials(); StoredCredentials.GetValid())
	{		
		this->SequenceRPCManager->FederateOIDCSession(StoredCredentials.GetCredentials().GetWalletAddress(),IdTokenIn, OnSuccess, OnFailure);
	}
	else
	{
		const FString ErrorMessage = TEXT("StoredCredentials are invalid, please login");
		UE_LOG(LogTemp, Warning, TEXT("Error: %s"), *ErrorMessage);
		this->CallFederateFailure(ErrorMessage);
	}
}

void USequenceAuthenticator::InitiateMobileFederateOIDC(const ESocialSigninType& Type)
{
	this->SetIsFederating(true);
	this->InitiateMobileSSO_Internal(Type);
}

void USequenceAuthenticator::FederatePlayFabNewAccount(const FString& UsernameIn, const FString& EmailIn, const FString& PasswordIn) 
{
	const TSuccessCallback<FString> OnSuccess = [this](const FString& SessionTicket)
	{
		const TFunction<void()> OnFederateSuccess = [this]()
		{
			this->CallFederateSuccess();
		};

		const FFailureCallback OnFederateFailure = [this](const FSequenceError& Error)
		{
			UE_LOG(LogTemp, Error, TEXT("Error Federating PlayFab Account: %s"), *Error.Message);
			this->CallFederateFailure(Error.Message);
		};

		//Load on disk credentials and get the wallet current address//

		if (FStoredCredentials_BE StoredCredentials = this->GetStoredCredentials(); StoredCredentials.GetValid())
		{
			this->SequenceRPCManager->FederatePlayFabSession(StoredCredentials.GetCredentials().GetWalletAddress(), SessionTicket, OnFederateSuccess, OnFederateFailure);
		}
		else
		{
			const FString ErrorMessage = TEXT("StoredCredentials are invalid, please login");
			UE_LOG(LogTemp, Warning, TEXT("Error: %s"), *ErrorMessage);
			this->CallFederateFailure(ErrorMessage);
		}
	};

	const FFailureCallback OnFailure = [this](const FSequenceError& Error)
	{
		UE_LOG(LogTemp, Warning, TEXT("Error Federating PlayFab Account: %s"), *Error.Message);
		this->CallFederateFailure(Error.Message);
	};

	this->PlayFabNewAccountLoginRPC(UsernameIn, EmailIn, PasswordIn, OnSuccess, OnFailure);
}

void USequenceAuthenticator::FederatePlayFabLogin(const FString& UsernameIn, const FString& PasswordIn) 
{
	const TSuccessCallback<FString> OnSuccess = [this](const FString& SessionTicket)
	{
		const TFunction<void()> OnFederateSuccess = [this]()
		{
			this->CallFederateSuccess();
		};

		const FFailureCallback OnFederateFailure = [this](const FSequenceError& Error)
		{
			UE_LOG(LogTemp, Warning, TEXT("Error Federating PlayFab Account: %s"), *Error.Message);
			this->CallFederateFailure(Error.Message);
		};
		
		//Load on disk credentials and get the wallet current address//

		if (FStoredCredentials_BE StoredCredentials = this->GetStoredCredentials(); StoredCredentials.GetValid())
		{
			this->SequenceRPCManager->FederatePlayFabSession(StoredCredentials.GetCredentials().GetWalletAddress(), SessionTicket, OnFederateSuccess, OnFederateFailure);
		}
		else
		{
			const FString ErrorMessage = TEXT("StoredCredentials are invalid, please login");
			UE_LOG(LogTemp, Warning, TEXT("Error: %s"), *ErrorMessage);
			this->CallFederateFailure(ErrorMessage);
		}
	};

	const FFailureCallback OnFailure = [this](const FSequenceError& Error)
	{
		UE_LOG(LogTemp, Warning, TEXT("Error Federating PlayFab Account: %s"), *Error.Message);
		this->CallFederateFailure(Error.Message);
	};

	this->PlayFabLoginRPC(UsernameIn, PasswordIn, OnSuccess, OnFailure);
}

void USequenceAuthenticator::ForceOpenLastOpenSessionAttempt()
{
	const TSuccessCallback<FCredentials_BE> OnSuccess = [this](const FCredentials_BE& Credentials)
	{
		this->InitializeSequence(Credentials);
	};

	const FFailureCallback OnFailure = [this](const FSequenceError& Error)
	{
		UE_LOG(LogTemp, Error, TEXT("Error Force Opening Session: %s"), *Error.Message);
		this->CallAuthFailure(Error.Message);
	};

	this->ResetFederateSessionInUse();
	this->SequenceRPCManager->ForceOpenSessionInUse(OnSuccess, OnFailure);
}

FStoredCredentials_BE USequenceAuthenticator::GetStoredCredentials() const
{	
	FCredentials_BE CredData;
	FCredentials_BE* Credentials = &CredData;
	const bool IsValid = this->GetStoredCredentials(Credentials);
	return FStoredCredentials_BE(IsValid, *Credentials);
}