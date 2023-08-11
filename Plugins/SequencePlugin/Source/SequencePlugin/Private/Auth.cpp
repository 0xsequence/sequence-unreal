// Fill out your copyright notice in the Description page of Project Settings.


#include "Auth.h"
#include "JsonObjectConverter.h"
#include "SequenceEncryptor.h"


/*
* Used for setting up fresh auth data for this session,
* used in the case when we had no auth data stored
*/
void UAuth::setNewSecureStorableAuth(FStoredAuthState_BE newAuthData)
{
	this->auth = newAuthData;
}

/*
* Used to take stored Auth data decrypt it, and reconstruct a FStoredAuthState_BE Struct
* if this process fails at any step we return false to let the caller know we were unable to parse
* the data we were given
*/
bool UAuth::setSecureStorableAuth(FSecureKey secureStoredAuth)
{
	bool bSucceeded = false;

	//decrypt the auth data
	FString decryptedJsonAuth = USequenceEncryptor::decrypt(secureStoredAuth.ky, secureStoredAuth.of);
	TSharedPtr<FJsonObject> jsonAuthObj = MakeShareable<FJsonObject>(new FJsonObject);
	FStoredAuthState_BE storedAuth;

	if (FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(decryptedJsonAuth), jsonAuthObj))
	{
		if (FJsonObjectConverter::JsonObjectToUStruct<FStoredAuthState_BE>(jsonAuthObj.ToSharedRef(), &storedAuth))
		{
			bSucceeded = true;
			this->auth = storedAuth;//update our auth with the stored data & report success!
		}
		else
		{//failed to convert the jsonObject into a UStruct
			UE_LOG(LogTemp, Error, TEXT("[Failed to convert jsonObject into a UStruct: [%s]]"), *decryptedJsonAuth);
		}
	}
	else
	{//failed to convert the decrypted string into a jsonObject
		UE_LOG(LogTemp, Error, TEXT("[Failed to convert stored jsonObjectString into a jsonObject: [%s]]"), *decryptedJsonAuth);
	}
	return bSucceeded;
}

/*
* Used for prepping our StoredAuthData_BE struct to be stored securely
* on disk using FAES encryption
*/
FSecureKey UAuth::getSecureStorableAuth()
{
	FSecureKey ret;
	FString jsonAuthString;
	if (FJsonObjectConverter::UStructToJsonObjectString<FStoredAuthState_BE>(this->auth, jsonAuthString))
	{
		ret.of = jsonAuthString.Len();//store the offset
		ret.ky = USequenceEncryptor::encrypt(jsonAuthString);//encrypt and store the jsonAuthObject
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[Failed to convert stored auth data into a jsonAuthString]"));
	}

	return ret;
}