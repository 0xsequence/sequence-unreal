// Fill out your copyright notice in the Description page of Project Settings.


#include "NativeEncryptors/GenericNativeEncryptor.h"

UGenericNativeEncryptor::~UGenericNativeEncryptor(){ }

FString UGenericNativeEncryptor::Encrypt(const FString& StringIn)
{
	return StringIn;
}

FString UGenericNativeEncryptor::Decrypt(const FString& StringIn)
{
	return StringIn;
}
