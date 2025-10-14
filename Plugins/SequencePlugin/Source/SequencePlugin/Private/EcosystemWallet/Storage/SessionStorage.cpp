#include "SessionStorage.h"
#include "StorableCredentials.h"
#include "Encryptors/AndroidEncryptor.h"
#include "Encryptors/AppleEncryptor.h"
#include "Encryptors/SequenceEncryptor.h"
#include "Encryptors/WindowsEncryptor.h"
#include "Kismet/GameplayStatics.h"
#include "Util/Log.h"
#include "Util/SequenceSupport.h"

TArray<FSessionSigner> FSessionStorage::GetStoredSigners()
{
	const FWalletSessions Sessions = GetStoredSessions();
	
	TArray<FSessionSigner> Signers;
	for (FSessionCredentials Session : Sessions.Sessions)
	{
		Signers.Add(FSessionSigner(Session));
	}

	return Signers;
}

FWalletSessions FSessionStorage::GetStoredSessions()
{
	TArray<FSessionCredentials> Sessions;
	if (!GetStoredSessions(Sessions))
	{
		FWalletSessions EmptySessions;
		EmptySessions.Sessions.Empty();
		return EmptySessions;
	}

	return {
		Sessions
	};
}

bool FSessionStorage::GetStoredSessions(TArray<FSessionCredentials>& Sessions)
{
	const USaveGame * SaveGame = UGameplayStatics::LoadGameFromSlot(SaveSlot, UserIndex);

	if (SaveGame != nullptr)
	{
		if (const UStorableCredentials* LoadedCredentials = Cast<UStorableCredentials>(SaveGame))
		{
			FString CtrJson = "";

			if (UGenericNativeEncryptor* Encryptor = GetEncryptor())
			{
				CtrJson = Encryptor->Decrypt(LoadedCredentials->EK);
			}
			else
			{
				CtrJson = USequenceEncryptor::Decrypt(LoadedCredentials->EK, LoadedCredentials->KL);
			}

			UE_LOG(LogTemp, Display, TEXT("Stored sessions Json %s"), *CtrJson);

			TArray<TSharedPtr<FJsonValue>> SessionsJson;
			USequenceSupport::ParseJsonStringToObjectArray(CtrJson, SessionsJson);

			for (TSharedPtr<FJsonValue> JsonObject : SessionsJson)
			{
				Sessions.Add(FSessionCredentials::FromJson(JsonObject->AsObject()));
			}

			/*if (ret == false)
			{
				UGameplayStatics::DeleteGameInSlot(this->SaveSlot, this->UserIndex);
			}*/
		}
	}
	
	return true;
}

void FSessionStorage::StoreSessions(const TArray<FSessionCredentials>& Sessions)
{
	TArray<TSharedPtr<FJsonObject>> SessionsJson;
	for (FSessionCredentials Session : Sessions)
	{
		SessionsJson.Add(Session.ToJson());
	}
	
	if (UStorableCredentials* StorableCredentials = Cast<UStorableCredentials>(UGameplayStatics::CreateSaveGameObject(UStorableCredentials::StaticClass())))
	{
		const FString CtsJson = USequenceSupport::JsonObjListToParsableString(SessionsJson);
		const int32 CtsJsonLength = CtsJson.Len();

		UE_LOG(LogTemp, Display, TEXT("Storing sessions Json %s"), *CtsJson);

		if (UGenericNativeEncryptor* Encryptor = GetEncryptor())
		{
			StorableCredentials->EK = Encryptor->Encrypt(CtsJson);
			StorableCredentials->KL = CtsJsonLength;
		}
		else
		{
			StorableCredentials->EK = USequenceEncryptor::Encrypt(CtsJson);
			StorableCredentials->KL = CtsJsonLength;
		}

		if (UGameplayStatics::SaveGameToSlot(StorableCredentials, SaveSlot, UserIndex))
		{
			SEQ_LOG(Display, TEXT("Session stored successfully."));
		}
	}
}

void FSessionStorage::AddSession(const FSessionCredentials& Credentials)
{
	TArray<FSessionCredentials> Sessions;
	if (!GetStoredSessions(Sessions))
	{
		UE_LOG(LogTemp, Error, TEXT("FSessionStorage::AddSession, error getting stored credentials."))
		return;
	}
	
	Sessions.Add(Credentials);
	StoreSessions(Sessions);
}

void FSessionStorage::ClearSessions()
{
	FWalletSessions EmptySessions;
	EmptySessions.Sessions.Empty();
	StoreSessions(TArray<FSessionCredentials>());
}

UGenericNativeEncryptor* FSessionStorage::GetEncryptor()
{
	if constexpr (PLATFORM_ANDROID)
	{
		return NewObject<UAndroidEncryptor>();
	}
	else if constexpr (PLATFORM_MAC)
	{
		return NewObject<UAppleEncryptor>();
	}
	else if constexpr (PLATFORM_WINDOWS)
	{
		return NewObject<UWindowsEncryptor>();
	}
	else if constexpr (PLATFORM_IOS)
	{
		return NewObject<UAppleEncryptor>();
	}
	else
	{
		SEQ_LOG(Warning, TEXT("You are running on an unsupported platform. If you continue, make sure to define a 'FallbackEncryptionKey' value in your SequenceConfig.ini file to encrypt/decrypt stored credentials."))
		return nullptr;
	}
}

