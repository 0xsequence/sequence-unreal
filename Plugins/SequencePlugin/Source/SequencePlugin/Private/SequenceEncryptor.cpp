// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once
#include "SequenceEncryptor.h"
#include "Misc/AES.h"
#include "Containers/UnrealString.h"

/*
* This function is meant to be rebuilt depending on how developers want to secure their key data
* for the time being I include this key for testing but in release builds no key will be provided and
* NO state will be written unless a valid key is provided!
*/
FString USequenceEncryptor::GetStoredKey()
{//***Replace this implementation with your own proper implementation***
	FString Ret;
//this is for testing only a proper implementation needs to be provided for this function to work!
	return "aaaabbbbccccddddaaaabbbbccccdddd";
}


void USequenceEncryptor::PKC7Padding(int32 blockSize)
{
	FString testString = "abc";
	int32 buffSize = sizeof(testString);
	uint8_t * buff = new uint8_t[buffSize];

	StringToBytes(testString, buff, buffSize);

}


int32 BytesInFString(FString data)
{
	int32 bytes = 0;
	for (auto c : data.GetCharArray())
	{
		bytes += sizeof(c);
	}
	return bytes;
}

//need to write a couple new functions for this based on AES-CBC-256, IV 16 bytes

void USequenceEncryptor::EncryptAES(FString data)
{
	FString key = GetStoredKey();
	if (key.Len() != 32)
	{
		UE_LOG(LogTemp, Error, TEXT("[Invalid key provided, BE SURE TO GENERATE A SECURE KEY!, NO STATE WILL BE SAVED UNLESS A SECURE KEY IS PROVIDED]"), *key);
		return;// "[NOP]";
	}
	

	//StringToBytes(data, PtrString, buffSize);

	//AES_init_ctx_iv(PtrCtx, PtrKey, PtrIV);//init then use
	//AES_CBC_encrypt_buffer(PtrCtx, PtrString, buffSize);

	//FString DuringEncrypt = BytesToString(PtrString, buffSize);
}

void USequenceEncryptor::DecryptAES()
{

}


FString USequenceEncryptor::Encrypt(FString Payload)
{
	FString key = GetStoredKey();
	if (key.Len() < 32)
	{
		UE_LOG(LogTemp, Error, TEXT("[Invalid key provided, BE SURE TO GENERATE A SECURE KEY!, NO STATE WILL BE SAVED UNLESS A SECURE KEY IS PROVIDED]"), *key);
		return "[NOP]";
	}
	//prepping the key for encryption
	const uint32 keySize = 32;//hard code this!
	uint8* keyBlob = new uint8[keySize];
	StringToBytes(key, keyBlob, keySize);
	
	uint32 size = Payload.Len();//computing size and respecting block offsets here!
	size = size + (FAES::AESBlockSize - (size % FAES::AESBlockSize));

	uint8* payloadBlob = new uint8[size];
	StringToBytes(Payload, payloadBlob, Payload.Len());

	FAES::EncryptData(payloadBlob, size, keyBlob, keySize);//encrypt
	FString encryptedData = BytesToString(payloadBlob, size);//get into usable form for storage
	
	//cleanup
	delete [] payloadBlob;
	delete [] keyBlob;

	return encryptedData;
}

FString USequenceEncryptor::Decrypt(FString Payload,int32 PayloadLength)
{
	FString key = GetStoredKey();
	if (key.Len() < 32)
	{
		UE_LOG(LogTemp, Error, TEXT("[Invalid key provided, BE SURE TO GENERATE A SECURE KEY!, NO STATE WILL BE SAVED UNLESS A SECURE KEY IS PROVIDED]"), *key);
		return "[NOP]";
	}
	//prepping the key for decryption
	const uint32 keySize = 32;//hard code this!
	uint8* keyBlob = new uint8[keySize];
	StringToBytes(key, keyBlob, keySize);

	uint32 size = Payload.Len();
	size = size + (FAES::AESBlockSize - (size % FAES::AESBlockSize));//align size with block size!
	uint8 * payloadBlob = new uint8[size];

	StringToBytes(Payload, payloadBlob, Payload.Len());
	FAES::DecryptData(payloadBlob, size, keyBlob, keySize);
	FString decryptedData = BytesToString(payloadBlob, size);

	//because post encryption we can have some fat left, we trim it
	decryptedData = decryptedData.Left(PayloadLength);

	delete [] payloadBlob;
	delete [] keyBlob;

	return decryptedData;
}