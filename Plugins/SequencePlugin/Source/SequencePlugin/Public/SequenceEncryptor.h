// Fill out your copyright notice in the Description page of Project Settings.

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
	* Used to get the stored encryption key for 
	* FAES encryption
	*/
	static FString getStoredKey();

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
	static FString encrypt(FString payload);

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
	static FString decrypt(FString payload,int32 payloadLength);
};