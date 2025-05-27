#pragma once

#include "CoreMinimal.h"
#include "Encryptors/GenericNativeEncryptor.h"
#include "WindowsEncryptor.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class SEQUENCEPLUGIN_API UWindowsEncryptor : public UGenericNativeEncryptor
{
	GENERATED_BODY()

public:
	virtual FString Encrypt(const FString& StringIn) override;

	virtual FString Decrypt(const FString& StringIn) override;
};
