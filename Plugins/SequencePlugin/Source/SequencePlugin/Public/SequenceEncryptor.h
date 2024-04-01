// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SequenceEncryptor.generated.h"

/**
 * 
 */
UCLASS()
class SEQUENCEPLUGIN_API USequenceEncryptor : public UObject
{
	GENERATED_BODY()
private:

	/*
	* This function is meant to be rebuilt depending on how developers want to secure their key data
	* for the time being I include this key for testing but in release builds no key will be provided and
	* NO state will be written unless a valid key is provided!
	*/
	//***Replace this implementation with your own proper implementation***
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

	static void EncryptAES(FString data);

	static void DecryptAES();

	static int32 BytesInFString(FString data);

	static void PKC7Padding(int32 blockSize);
};