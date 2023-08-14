// Fill out your copyright notice in the Description page of Project Settings.


#include "SequenceEncryptor.h"
#include "Misc/AES.h"
#include "Containers/UnrealString.h"

/*
* This function is meant to be rebuilt depending on how developers want to secure their key data
* for the time being I include this key for testing but in release builds no key will be provided and
* NO state will be written unless a valid key is provided!
*/
FString USequenceEncryptor::getStoredKey()
{//***Replace this implementation with your own proper implementation***
	FString ret;

//this is for testing only a proper implementation needs to be provided for this function to work!
#if UE_EDITOR
	TArray<FString> kData;

	kData.Add("cmpnb");
	kData.Add("kFlZWYwbFJ");
	kData.Add("TRGJrOE5");
	kData.Add("nT282QTc5Uj");
	kData.Add("E4aFQ");
	kData.Add("2MFJEO");
	kData.Add("ExW");
	kData.Add("Y0RkRWN");
	kData.Add("ad");
	kData.Add("z0=");
	FString eK;
	for (FString kP : kData)
	{
		eK += kP;
	}
	FBase64::Decode(eK, ret);
#endif // UE_EDITOR
	return ret;
}

FString USequenceEncryptor::encrypt(FString payload)
{
	FString key = getStoredKey();
	if (key.Len() < 32)
	{
		UE_LOG(LogTemp, Error, TEXT("[Invalid key provided, BE SURE TO GENERATE A SECURE KEY!, NO STATE WILL BE SAVED UNLESS A SECURE KEY IS PROVIDED]"), *key);
		return "[NOP]";
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
	delete [] payloadBlob;
	delete [] keyBlob;

	return encryptedData;
}

FString USequenceEncryptor::decrypt(FString payload,int32 payloadLength)
{
	FString key = getStoredKey();
	if (key.Len() < 32)
	{
		UE_LOG(LogTemp, Error, TEXT("[Invalid key provided, BE SURE TO GENERATE A SECURE KEY!, NO STATE WILL BE SAVED UNLESS A SECURE KEY IS PROVIDED]"), *key);
		return "[NOP]";
	}
	//prepping the key for decryption
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

	delete [] payloadBlob;
	delete [] keyBlob;

	return decryptedData;
}