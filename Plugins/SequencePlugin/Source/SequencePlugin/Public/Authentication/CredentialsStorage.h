// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "Credentials.h"
#include "Encryptors/GenericNativeEncryptor.h"
#include "CredentialsStorage.generated.h"

UCLASS()
class SEQUENCEPLUGIN_API UCredentialsStorage : public UObject
{
	GENERATED_BODY()
	
public:
	UCredentialsStorage();

	void SetEncryptor(UGenericNativeEncryptor* NewEncryptor);
	void StoreCredentials(const FCredentials_BE& Credentials) const;
	bool GetStoredCredentials(FCredentials_BE* Credentials) const;
	FStoredCredentials_BE GetStoredCredentials() const;
	void ClearStoredCredentials() const;

private:
	UPROPERTY()
	UGenericNativeEncryptor* Encryptor = nullptr;
	
	const FString SaveSlot = "SequenceCredentials";
	const uint32 UserIndex = 0;
};
