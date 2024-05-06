#pragma once

#include "CoreMinimal.h"
#include "NativeEncryptors/GenericNativeEncryptor.h"
#include "AndroidNativeEncryptor.generated.h"

UCLASS()
class SEQUENCEPLUGIN_API UAndroidNativeEncryptor : public UGenericNativeEncryptor
{
	GENERATED_UCLASS_BODY()

private:

	UAndroidNativeEncryptor();
	
public:

	virtual FString Encrypt(const FString& StringIn) override;

	virtual FString Decrypt(const FString& StringIn) override;
};