#pragma once

#include "CoreMinimal.h"
#include "Encryptors/GenericNativeEncryptor.h"
#include "AndroidEncryptor.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class SEQUENCEPLUGIN_API UAndroidEncryptor : public UGenericNativeEncryptor
{
	GENERATED_BODY()

public:
	virtual FString Encrypt(const FString& StringIn) override;

	virtual FString Decrypt(const FString& StringIn) override;
};
