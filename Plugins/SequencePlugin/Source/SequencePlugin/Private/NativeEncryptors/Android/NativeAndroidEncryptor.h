// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NativeEncryptors/GenericNativeEncryptor.h"
#include "NativeAndroidEncryptor.generated.h"

/**
 * 
 */
UCLASS()
class SEQUENCEPLUGIN_API UNativeAndroidEncryptor : public UGenericNativeEncryptor
{
	GENERATED_BODY()

public:
	UNativeAndroidEncryptor();

	virtual FString Encrypt(const FString& StringIn) override;

	virtual FString Decrypt(const FString& StringIn) override;
};
