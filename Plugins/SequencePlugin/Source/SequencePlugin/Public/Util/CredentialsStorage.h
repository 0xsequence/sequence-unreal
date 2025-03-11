#pragma once
#include "Credentials.h"
#include "Encryptors/GenericNativeEncryptor.h"

class SEQUENCEPLUGIN_API UCredentialsStorage : public UObject
{
public:
	UCredentialsStorage();

	void SetEncryptor(UGenericNativeEncryptor* Encryptor);
	void StoreCredentials(const FCredentials_BE& Credentials) const;
	bool GetStoredCredentials(FCredentials_BE* Credentials) const;
	FStoredCredentials_BE GetStoredCredentials() const;
	void ClearStoredCredentials() const;

private:
	UGenericNativeEncryptor* Encryptor = nullptr;
	const FString SaveSlot = "Cr";
	const uint32 UserIndex = 0;
};
