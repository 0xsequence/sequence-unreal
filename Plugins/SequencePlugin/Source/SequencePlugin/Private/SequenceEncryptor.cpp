// Fill out your copyright notice in the Description page of Project Settings.


#include "SequenceEncryptor.h"
#include "Misc/AES.h"
#include "Containers/UnrealString.h"

FString USequenceEncryptor::encrypt(FString payload,FString key)
{
	if (key.Len() < 32)
	{
		UE_LOG(LogTemp, Error, TEXT("[Cannot decrypt with a key length less than 32]"), *key);
		return "";
	}
	//prepping the key for encryption
	const uint32 keySize = 32;//hard code this!
	uint8* keyBlob = new uint8[keySize];
	StringToBytes(key, keyBlob, keySize);
	
	uint32 size = payload.Len();//computing size and respecting block offsets here!
	size = size + (FAES::AESBlockSize - (size % FAES::AESBlockSize));

	uint8* payloadBlob = new uint8[size];
	StringToBytes(payload, payloadBlob, payload.Len());

	FAES::EncryptData(payloadBlob, size, keyBlob, keySize);//encrypt
	FString encryptedData = BytesToString(payloadBlob, size);//get into usable form for storage
	
	//cleanup
	delete payloadBlob;
	delete keyBlob;

	return encryptedData;
}

FString USequenceEncryptor::decrypt(FString payload,int32 payloadLength, FString key)
{
	if (key.Len() < 32)
	{
		UE_LOG(LogTemp, Error, TEXT("[Cannot decrypt with a key length less than 32]"), *key);
		return "";
	}
	//prepping the key for encryption
	const uint32 keySize = 32;//hard code this!
	uint8* keyBlob = new uint8[keySize];
	StringToBytes(key, keyBlob, keySize);

	uint32 size = payload.Len();
	size = size + (FAES::AESBlockSize - (size % FAES::AESBlockSize));//align size with block size!
	uint8 * payloadBlob = new uint8[size];

	StringToBytes(payload, payloadBlob, payload.Len());
	FAES::DecryptData(payloadBlob, size, keyBlob, keySize);
	FString decryptedData = BytesToString(payloadBlob, size);

	//because post encryption we can have some fat left, we trim it
	decryptedData = decryptedData.Left(payloadLength);

	delete payloadBlob;
	delete keyBlob;

	return decryptedData;
}