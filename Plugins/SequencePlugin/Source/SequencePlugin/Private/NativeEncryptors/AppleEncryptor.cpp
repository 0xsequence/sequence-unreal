//Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "NativeEncryptors/AppleEncryptor.h"
#include "Native/AppleBridge.h"

FString UAppleEncryptor::Encrypt(const FString& StringIn)
{
	FString Result = "";
	Result = UAppleBridge::AppleEncrypt(StringIn);
	return Result;
}

FString UAppleEncryptor::Decrypt(const FString& StringIn)
{
	FString Result = "";
	Result = UAppleBridge::AppleDecrypt(StringIn);
	return Result;
}
