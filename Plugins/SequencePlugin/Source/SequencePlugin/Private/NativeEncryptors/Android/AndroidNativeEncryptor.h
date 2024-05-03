#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "NativeEncryptors/GenericNativeEncryptor.h"
#include "AndroidNativeEncryptor.generated.h"

UCLASS()
class UAndroidNativeEncryptor : public UGenericNativeEncryptor
{
	GENERATED_BODY()

	public:

	virtual FString Encrypt(const FString& StringIn) override;

	virtual FString Decrypt(const FString& StringIn) override;
};