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
	
public:

	/*
	* Encryptor built on Unreals FAES encryptor
	* 
	* @Param payload what we want to encrypt
	* 
	* @Key the key must be 32 bytes in length
	* 
	* @Return the AES encrypted data
	*/
	static FString encrypt(FString payload,FString key);

	/*
	* Decryptor built on Unreals FAES encryptor
	* @Param payload what we want to decrypt
	* 
	* @Param payload length the length of the result prior to encryption
	* We need this because when we decrypt we can end up with some fat on the end of the string
	* and we need to remove it
	* 
	* @Key the key must be 32 bytes in length
	* 
	* @Return the decrypted data
	*/
	static FString decrypt(FString payload,int32 payloadLength, FString key);
};