// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once
#include "CoreMinimal.h"
#include "Misc/AES.h"
#include "Containers/UnrealString.h"
#include "SequenceEncryptor.generated.h"

/**
 * 
 */
UCLASS()
class SEQUENCEPLUGIN_API USequenceEncryptor : public UObject
{
	GENERATED_BODY()
private:
	
	static FString GetStoredKey();

public:

	/*
	* Encryptor built on Unreals FAES encryptor
	* 
	* @Param payload what we want to encrypt
	* 
	* @For the key we utilize the stored key generated in project settings
	* 
	* @Return the AES encrypted data
	*/
	static FString Encrypt(FString Payload);

	/*
	* Decryptor built on Unreals FAES encryptor
	* @Param payload what we want to decrypt
	* 
	* @Param payload length the length of the result prior to encryption
	* We need this because when we decrypt we can end up with some fat on the end of the string
	* and we need to remove it
	* 
	* @For the key we utilize the stored key generated in project settings
	* 
	* @Return the decrypted data
	*/
	static FString Decrypt(FString Payload,int32 PayloadLength);
};