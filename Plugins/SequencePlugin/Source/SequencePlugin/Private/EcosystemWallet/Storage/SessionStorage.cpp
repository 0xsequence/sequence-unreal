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

bool USessionStorage::GetStoredSessions(FWalletSessions* Sessions)
{
	bool ret = false;
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

			UE_LOG(LogTemp, Display, TEXT("Sessions Json %s"), *CtrJson);
			ret = USequenceSupport::JSONStringToStruct<FWalletSessions>(CtrJson, Sessions);
			ret &= Sessions->HasSessions();

			if (ret == false)
			{
				UGameplayStatics::DeleteGameInSlot(this->SaveSlot, this->UserIndex);
			}
		}
	}
	
	return ret;
}

FWalletSessions USessionStorage::GetStoredSessions()
{
	FWalletSessions CredData;
	FWalletSessions* Credentials = &CredData;
	
	if (this->GetStoredSessions(Credentials))
	{
		return *Credentials;
	}

	FWalletSessions EmptySessions;
	EmptySessions.Sessions.Empty();
	return EmptySessions;
}

void USessionStorage::StoreSessions(const FWalletSessions& WalletSessions)
{
	if (UStorableCredentials* StorableCredentials = Cast<UStorableCredentials>(UGameplayStatics::CreateSaveGameObject(UStorableCredentials::StaticClass())))
	{
		const FString CtsJson = USequenceSupport::StructToString<FWalletSessions>(WalletSessions);
		const int32 CtsJsonLength = CtsJson.Len();

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

void USessionStorage::AddSession(const FSessionCredentials& Session)
{
	FWalletSessions WalletSessions = this->GetStoredSessions();
	WalletSessions.Sessions.Add(Session);
	this->StoreSessions(WalletSessions);
}

void USessionStorage::ClearSessions()
{
	FWalletSessions EmptySessions;
	EmptySessions.Sessions.Empty();
	this->StoreSessions(EmptySessions);
}

