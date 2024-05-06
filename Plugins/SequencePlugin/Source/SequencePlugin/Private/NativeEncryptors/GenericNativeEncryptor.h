// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericNativeEncryptor.generated.h"

/**
 * 
 */
UCLASS()
class SEQUENCEPLUGIN_API UGenericNativeEncryptor : public UObject
{
	GENERATED_BODY()
public:
	UGenericNativeEncryptor();

	virtual FString Encrypt(const FString& StringIn);

	virtual FString Decrypt(const FString& StringIn);
};
