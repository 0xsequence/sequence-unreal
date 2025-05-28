#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GenericNativeEncryptor.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class SEQUENCEPLUGIN_API UGenericNativeEncryptor : public UObject
{
	GENERATED_BODY()
public:
	virtual FString Encrypt(const FString& StringIn);

	virtual FString Decrypt(const FString& StringIn);
};
