// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "SequenceEncryptor.h"
#include "ConfigFetcher.h"
#include "Util/Log.h"

FString USequenceEncryptor::GetStoredKey()
{
	return UConfigFetcher::GetConfigVar(UConfigFetcher::FallBackEncryptionKey);
}

FString USequenceEncryptor::Encrypt(FString Payload)
{
	FString key = GetStoredKey();
	if (key.Len() < 32)
	{
		SEQ_LOG(Error, TEXT("[Invalid key provided, BE SURE TO GENERATE A SECURE KEY that is at least 32 CHARACTERS in length!, NO STATE WILL BE SAVED UNLESS A SECURE KEY IS PROVIDED]"));
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
		SEQ_LOG(Error, TEXT("[Invalid key provided, BE SURE TO GENERATE A SECURE KEY!, NO STATE WILL BE SAVED UNLESS A SECURE KEY IS PROVIDED]"), *key);
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