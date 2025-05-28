#pragma once

#include "CoreMinimal.h"
#include "Encryptors/GenericNativeEncryptor.h"
#include "AppleEncryptor.generated.h"

/**
 * 
 */
UCLASS()
class SEQUENCEPLUGIN_API UAppleEncryptor : public UGenericNativeEncryptor
{
	GENERATED_BODY()

public:
	virtual FString Encrypt(const FString& StringIn) override;

	virtual FString Decrypt(const FString& StringIn) override;
};
