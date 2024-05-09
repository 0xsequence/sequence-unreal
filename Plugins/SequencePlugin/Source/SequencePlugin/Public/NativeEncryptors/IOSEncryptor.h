//Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "NativeEncryptors/GenericNativeEncryptor.h"
#include "IOSEncryptor.generated.h"

/**
 * 
 */
UCLASS()
class SEQUENCEPLUGIN_API UIOSEncryptor : public UGenericNativeEncryptor
{
	GENERATED_BODY()

public:
	virtual FString Encrypt(const FString& StringIn) override;

	virtual FString Decrypt(const FString& StringIn) override;
};
