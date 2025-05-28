#include "EOSManager.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/Paths.h"

struct FEosLoginContext
{
	UEOSManager* Self;
	TFunction<void()> OnSuccess;
	TFunction<void()> OnFailure;
};

UEOSManager* UEOSManager::MakeEosManager()
{
	return NewObject<UEOSManager>();
}

// EOS SDK

void UEOSManager::Tick()
{
	if (this->EosPlatform)
	{
		EOS_Platform_Tick(this->EosPlatform);	
	}
}

void UEOSManager::Initialize()
{
	const FString CacheDirectory = FPaths::ConvertRelativePathToFull(FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("EOSCache")));

	EOS_Platform_Options PlatformOptions = {};
	PlatformOptions.ApiVersion = EOS_PLATFORM_OPTIONS_API_LATEST;
	PlatformOptions.ProductId = GetConfigValue("ProductId");
	PlatformOptions.SandboxId = GetConfigValue("SandboxId");
	PlatformOptions.DeploymentId = GetConfigValue("DeploymentId");
	PlatformOptions.ClientCredentials.ClientId = GetConfigValue("ClientId");
	PlatformOptions.ClientCredentials.ClientSecret = GetConfigValue("ClientSecret");
	PlatformOptions.EncryptionKey = GetConfigValue("EncryptionKey");
	PlatformOptions.TickBudgetInMilliseconds = 0;
	PlatformOptions.OverrideCountryCode = nullptr;
	PlatformOptions.OverrideLocaleCode = nullptr;
	PlatformOptions.Flags = 0;

	this->EosPlatform = EOS_Platform_Create(&PlatformOptions);
	if (!this->EosPlatform)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create EOS platform."));
		return;
	}

	UE_LOG(LogTemp, Display, TEXT("EOS Platform initialized successfully."));
}

// EOS Epic Account Services

void UEOSManager::LoginWithEpicAccountServices(FOnCallback OnSuccess, FOnCallback OnFailure)
{
	if (!this->EosPlatform)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to login. EOS Platform is null, please Initialize."));
		OnFailure.ExecuteIfBound();
		return;
	}
	
	this->EosAuth = EOS_Platform_GetAuthInterface(this->EosPlatform);
	
	EOS_Auth_Credentials Credentials = {};
	Credentials.ApiVersion = EOS_AUTH_CREDENTIALS_API_LATEST;
	Credentials.Type = EOS_ELoginCredentialType::EOS_LCT_AccountPortal;

	EOS_Auth_LoginOptions LoginOptions = {};
	LoginOptions.ApiVersion = EOS_AUTH_LOGIN_API_LATEST;
	LoginOptions.Credentials = &Credentials;
	LoginOptions.ScopeFlags = EOS_EAuthScopeFlags::EOS_AS_NoFlags;

	FEosLoginContext* ClientData = new FEosLoginContext;
	ClientData->Self = this;
	ClientData->OnSuccess = [OnSuccess]() { OnSuccess.ExecuteIfBound(); };
	ClientData->OnFailure = [OnFailure]() { OnFailure.ExecuteIfBound(); };
	
	EOS_Auth_Login(this->EosAuth, &LoginOptions, ClientData,[](const EOS_Auth_LoginCallbackInfo* Data)
	{
		const FEosLoginContext* Context = static_cast<FEosLoginContext*>(Data->ClientData);
		if (Data->ResultCode == EOS_EResult::EOS_Success)
		{
			const EOS_EpicAccountId EpicId = Data->LocalUserId;
			Context->Self->EpicAccountId = EpicId;
			Context->OnSuccess();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Epic Account login failed: %s"), *FString(EOS_EResult_ToString(Data->ResultCode)));
			Context->OnFailure();
		}
	});	
}

FString UEOSManager::CopyEpicAccountAccessToken()
{
	if (!this->EpicAccountId)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get Access Token. Epic Account Id is null."));
		return TEXT("");
	}
	
	EOS_Auth_CopyUserAuthTokenOptions TokenOptions
	{
		EOS_AUTH_COPYUSERAUTHTOKEN_API_LATEST
	};
	
	EOS_Auth_Token* AuthToken = nullptr;
	const EOS_EResult Result = EOS_Auth_CopyUserAuthToken(this->EosAuth, &TokenOptions, this->EpicAccountId, &AuthToken);

	if (Result == EOS_EResult::EOS_Success && AuthToken)
	{
		const FString AccessToken = FString(AuthToken->AccessToken);
		UE_LOG(LogTemp, Log, TEXT("Epic Account Services Access Token: %s"), *AccessToken);
		
		EOS_Auth_Token_Release(AuthToken);
		return AccessToken;
	}
	
	UE_LOG(LogTemp, Error, TEXT("Failed to copy user auth token: %s"), *FString(EOS_EResult_ToString(Result)));
	return TEXT("");
}

// EOS Connect Services

void UEOSManager::LoginWithConnectServices(const FString& ExternalToken, FOnCallback OnSuccess, FOnCallback OnFailure)
{
	this->EosConnect = EOS_Platform_GetConnectInterface(this->EosPlatform);

	const FTCHARToUTF8 Converted(*ExternalToken);
	const char* Token = Converted.Get();
	
	const EOS_Connect_Credentials Credentials
	{
		EOS_CONNECT_CREDENTIALS_API_LATEST,
		Token,
		EOS_EExternalCredentialType::EOS_ECT_EPIC
	};

	const EOS_Connect_LoginOptions LoginOptions
	{
		EOS_CONNECT_LOGIN_API_LATEST,
		&Credentials
	};

	FEosLoginContext* ClientData = new FEosLoginContext;
	ClientData->Self = this;
	ClientData->OnSuccess = [OnSuccess]() { OnSuccess.ExecuteIfBound(); };
	ClientData->OnFailure = [OnFailure]() { OnFailure.ExecuteIfBound(); };

	EOS_Connect_Login(this->EosConnect, &LoginOptions, ClientData, [](const EOS_Connect_LoginCallbackInfo* Data)
	{
		const FEosLoginContext* Context = static_cast<FEosLoginContext*>(Data->ClientData);
		
		UE_LOG(LogTemp, Display, TEXT("EOS Connect login done, checking result"));
		if (Data->ResultCode == EOS_EResult::EOS_Success)
		{
			Context->Self->ProductUserId = Data->LocalUserId;
			UE_LOG(LogTemp, Display, TEXT("EOS Connect login successful"));
			Context->OnSuccess();
		}
		else if (Data->ResultCode == EOS_EResult::EOS_InvalidUser)
		{
			UE_LOG(LogTemp, Display, TEXT("User not found — must create new user."));
			
			const EOS_Connect_CreateUserOptions CreateUserOptions
			{
				EOS_CONNECT_CREATEUSER_API_LATEST,
				Data->ContinuanceToken
			};

			EOS_Connect_CreateUser(Context->Self->EosConnect, &CreateUserOptions, Data->ClientData, [](const EOS_Connect_CreateUserCallbackInfo* Data)
			{
				const FEosLoginContext* Context = static_cast<FEosLoginContext*>(Data->ClientData);
				
				if (Data->ResultCode == EOS_EResult::EOS_Success)
				{
					UE_LOG(LogTemp, Display, TEXT("EOS Connect user created successfully!"));
					Context->Self->ProductUserId = Data->LocalUserId;
					Context->OnSuccess();
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Failed to create EOS Connect user: %s"), *FString(EOS_EResult_ToString(Data->ResultCode)));
					Context->OnFailure();
				}
			});
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("EOS Connect login failed %s"), *FString(EOS_EResult_ToString(Data->ResultCode)));
			Context->OnFailure();
		}
	});
}

FString UEOSManager::CopyConnectIdToken()
{
	EOS_Connect_CopyIdTokenOptions Options = {};
	Options.ApiVersion = EOS_CONNECT_COPYIDTOKEN_API_LATEST;
	Options.LocalUserId = this->ProductUserId;

	EOS_Connect_IdToken* IdToken = nullptr;
	const EOS_EResult Result = EOS_Connect_CopyIdToken(this->EosConnect, &Options, &IdToken);

	if (Result == EOS_EResult::EOS_Success && IdToken != nullptr)
	{
		const FString Jwt = UTF8_TO_TCHAR(IdToken->JsonWebToken);
		UE_LOG(LogTemp, Log, TEXT("Connect Services Id Token: %s"), *Jwt);
		
		EOS_Connect_IdToken_Release(IdToken);
		return Jwt;
	}
	
	UE_LOG(LogTemp, Error, TEXT("Failed to copy ID token: %s"), *FString(EOS_EResult_ToString(Result)));
	return TEXT("");
}

char* UEOSManager::GetConfigValue(const FString& Key)
{
	if(!GConfig)
	{
		UE_LOG(LogTemp, Error, TEXT("GConfig error"));
		return const_cast<char*>("");
	}
 
	FString Value;
	const FString Section = "/Script/EosConfig.Config";
	const FString Filename = FConfigCacheIni::NormalizeConfigIniPath(FPaths::ProjectConfigDir() + TEXT("/EosConfig.ini"));
	
	GConfig->Flush(true, Filename);
	if (GConfig->GetString(GetData(Section), GetData(Key),Value,GetData(Filename)))
	{
		const FTCHARToUTF8 Converter(*Value);
		const int32 Length = Converter.Length();

		char* CharBuffer = new char[Length + 1];
		FMemory::Memcpy(CharBuffer, Converter.Get(), Length);
		CharBuffer[Length] = '\0';
		
		return CharBuffer;
	}
	
	UE_LOG(LogTemp, Error, TEXT("Error reading config file, ensure EosConfig.ini is setup correctly."));
	return const_cast<char*>("");
}
