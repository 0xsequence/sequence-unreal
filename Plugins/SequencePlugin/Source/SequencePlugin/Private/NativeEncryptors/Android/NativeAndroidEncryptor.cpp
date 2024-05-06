// Fill out your copyright notice in the Description page of Project Settings.

#include "NativeEncryptors/Android/NativeAndroidEncryptor.h"

UNativeAndroidEncryptor::UNativeAndroidEncryptor() : UGenericNativeEncryptor()
{ }

FString UNativeAndroidEncryptor::Encrypt(const FString& StringIn) override
{
	FString Result = "";
	return Result;
}

FString UNativeAndroidEncryptor::Decrypt(const FString& StringIn) override
{
	FString Result = "";
	return Result;
}
