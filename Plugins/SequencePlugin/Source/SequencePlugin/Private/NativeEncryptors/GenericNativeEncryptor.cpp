// Fill out your copyright notice in the Description page of Project Settings.
#include "NativeEncryptors/GenericNativeEncryptor.h"

UGenericNativeEncryptor::UGenericNativeEncryptor(){ }

UGenericNativeEncryptor::~UGenericNativeEncryptor(){ }

FString UGenericNativeEncryptor::Encrypt(const FString& StringIn)
{
	UE_LOG(LogTemp,Warning,TEXT("Please Override this function with an actual implementation in child classes"));
	return "";
}

FString UGenericNativeEncryptor::Decrypt(const FString& StringIn)
{
	UE_LOG(LogTemp,Warning,TEXT("Please Override this function with an actual implementation in child classes"));
	return "";
}
