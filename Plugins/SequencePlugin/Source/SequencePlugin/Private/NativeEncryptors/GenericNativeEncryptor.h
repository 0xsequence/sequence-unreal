// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GenericNativeEncryptor.generated.h"

/**
 * 
 */
UCLASS()
class abstract UGenericNativeEncryptor : public UObject
{
	GENERATED_BODY()

public:

	virtual FString Encrypt(const FString& StringIn);

	virtual FString Decrypt(const FString& StringIn);
};
