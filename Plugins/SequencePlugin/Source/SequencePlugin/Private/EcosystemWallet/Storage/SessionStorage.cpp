#include "SessionStorage.h"

#include "StorableCredentials.h"
#include "Encryptors/AndroidEncryptor.h"
#include "Encryptors/AppleEncryptor.h"
#include "Encryptors/SequenceEncryptor.h"
#include "Encryptors/WindowsEncryptor.h"
#include "Kismet/GameplayStatics.h"
#include "Util/Log.h"
#include "Util/SequenceSupport.h"

USessionStorage::USessionStorage()
{
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
	else
	{
		SEQ_LOG(Warning, TEXT("You are running on an unsupported platform. If you continue, make sure to define a 'FallbackEncryptionKey' value in your SequenceConfig.ini file to encrypt/decrypt stored credentials."))
	}
}

FWalletSessions USessionStorage::GetStoredSessions()
{
	TArray<FSessionCredentials> Sessions;
	if (!this->GetStoredSessions(Sessions))
	{
		FWalletSessions EmptySessions;
		EmptySessions.Sessions.Empty();
		return EmptySessions;
	}

	return {
		Sessions
	};
}

bool USessionStorage::GetStoredSessions(TArray<FSessionCredentials>& Sessions)
{
	const USaveGame * SaveGame = UGameplayStatics::LoadGameFromSlot(this->SaveSlot, this->UserIndex);

	if (SaveGame != nullptr)
	{
		if (const UStorableCredentials* LoadedCredentials = Cast<UStorableCredentials>(SaveGame))
		{
			FString CtrJson = "";

			if (Encryptor)
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
				UE_LOG(LogTemp, Display, TEXT("JsonObject"));
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

void USessionStorage::StoreSessions(const TArray<FSessionCredentials>& Sessions)
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

		if (Encryptor)
		{
			StorableCredentials->EK = this->Encryptor->Encrypt(CtsJson);
			StorableCredentials->KL = CtsJsonLength;
		}
		else
		{
			StorableCredentials->EK = USequenceEncryptor::Encrypt(CtsJson);
			StorableCredentials->KL = CtsJsonLength;
		}

		if (UGameplayStatics::SaveGameToSlot(StorableCredentials, this->SaveSlot, this->UserIndex))
		{
			SEQ_LOG(Display, TEXT("Session stored successfully."));
		}
	}
}

void USessionStorage::AddSession(const FSessionCredentials& Credentials)
{
	TArray<FSessionCredentials> Sessions;
	if (!this->GetStoredSessions(Sessions))
	{
		UE_LOG(LogTemp, Error, TEXT("USessionStorage::AddSession, error getting stored credentials."))
		return;
	}
	
	Sessions.Add(Credentials);
	this->StoreSessions(Sessions);
}

void USessionStorage::ClearSessions()
{
	FWalletSessions EmptySessions;
	EmptySessions.Sessions.Empty();
	this->StoreSessions(TArray<FSessionCredentials>());
}

