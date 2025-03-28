#include "Util/CredentialsStorage.h"
#include "StorableCredentials.h"
#include "Encryptors/AndroidEncryptor.h"
#include "Encryptors/AppleEncryptor.h"
#include "Encryptors/SequenceEncryptor.h"
#include "Encryptors/WindowsEncryptor.h"
#include "Kismet/GameplayStatics.h"
#include "Util/SequenceSupport.h"

UCredentialsStorage::UCredentialsStorage()
{
	if constexpr (PLATFORM_ANDROID)
	{
		this->SetEncryptor(NewObject<UAndroidEncryptor>());
	}
	else if constexpr (PLATFORM_MAC)
	{
		this->SetEncryptor(NewObject<UAppleEncryptor>());
	}
	else if constexpr (PLATFORM_WINDOWS)
	{
		this->SetEncryptor(NewObject<UWindowsEncryptor>());
	}
	else if constexpr (PLATFORM_IOS)
	{
		this->SetEncryptor(NewObject<UAppleEncryptor>());
	}
}

void UCredentialsStorage::SetEncryptor(UGenericNativeEncryptor* NewEncryptor)
{
	this->Encryptor = NewEncryptor;	
}

void UCredentialsStorage::StoreCredentials(const FCredentials_BE& Credentials) const
{
	if (UStorableCredentials* StorableCredentials = Cast<UStorableCredentials>(UGameplayStatics::CreateSaveGameObject(UStorableCredentials::StaticClass())))
	{
		const FString CTS_Json = USequenceSupport::StructToString<FCredentials_BE>(Credentials);
		const int32 CTS_Json_Length = CTS_Json.Len();

		if (Encryptor)
		{
			StorableCredentials->EK = this->Encryptor->Encrypt(CTS_Json);
			StorableCredentials->KL = CTS_Json_Length;
		}
		else
		{
			StorableCredentials->EK = USequenceEncryptor::Encrypt(CTS_Json);
			StorableCredentials->KL = CTS_Json_Length;
		}

		if (UGameplayStatics::SaveGameToSlot(StorableCredentials, this->SaveSlot, this->UserIndex))
		{
			SEQ_LOG(Display, TEXT("Data Saved Correctly!"));
		}
	}
}

bool UCredentialsStorage::GetStoredCredentials(FCredentials_BE* Credentials) const
{
	bool ret = false;
	const USaveGame * SaveGame = UGameplayStatics::LoadGameFromSlot(this->SaveSlot, this->UserIndex);

	if (SaveGame != nullptr)
	{
		if (const UStorableCredentials* LoadedCredentials = Cast<UStorableCredentials>(SaveGame))
		{
			FString CTR_Json = "";

			if (Encryptor)
			{
				CTR_Json = Encryptor->Decrypt(LoadedCredentials->EK);
			}
			else
			{
				CTR_Json = USequenceEncryptor::Decrypt(LoadedCredentials->EK, LoadedCredentials->KL);
			}

			ret = USequenceSupport::JSONStringToStruct<FCredentials_BE>(CTR_Json, Credentials);
			ret &= Credentials->RegisteredValid();

			if (ret == false)
			{
				SEQ_LOG(Error, TEXT("[System Failure: Unable to read save file or file is corrupted]"));
				UGameplayStatics::DeleteGameInSlot(this->SaveSlot, this->UserIndex);
			}
		}
	}
	return ret;
}

FStoredCredentials_BE UCredentialsStorage::GetStoredCredentials() const
{	
	FCredentials_BE CredData;
	FCredentials_BE* Credentials = &CredData;
	const bool IsValid = this->GetStoredCredentials(Credentials);
	return FStoredCredentials_BE(IsValid, *Credentials);
}

void UCredentialsStorage::ClearStoredCredentials() const
{
	const FCredentials_BE BlankCredentials;
	this->StoreCredentials(BlankCredentials);	
}
