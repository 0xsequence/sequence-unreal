// Fill out your copyright notice in the Description page of Project Settings.


#include "Auth.h"
#include "JsonObjectConverter.h"
#include "SequenceEncryptor.h"


/*
* Used for setting up fresh auth data for this session,
* used in the case when we had no auth data stored
*/
void UAuth::SetNewSecureStorableAuth(FStoredAuthState_BE NewAuthData)
{
	this->auth = NewAuthData;
}

/*
* Used to take stored Auth data decrypt it, and reconstruct a FStoredAuthState_BE Struct
* if this process fails at any step we return false to let the caller know we were unable to parse
* the data we were given
*/
bool UAuth::SetSecureStorableAuth(FSecureKey SecureStoredAuth)
{
	bool bSucceeded = false;

	//decrypt the auth data
	const FString DecryptedJsonAuth = USequenceEncryptor::decrypt(SecureStoredAuth.ky, SecureStoredAuth.of);
	TSharedPtr<FJsonObject> JSONAuthObj = MakeShareable<FJsonObject>(new FJsonObject);
	FStoredAuthState_BE StoredAuth;

	if (FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(DecryptedJsonAuth), JSONAuthObj))
	{
		if (FJsonObjectConverter::JsonObjectToUStruct<FStoredAuthState_BE>(JSONAuthObj.ToSharedRef(), &StoredAuth))
		{
			bSucceeded = true;
			this->auth = StoredAuth;//update our auth with the stored data & report success!
		}
		else
		{//failed to convert the jsonObject into a UStruct
			UE_LOG(LogTemp, Error, TEXT("[Failed to convert jsonObject into a UStruct: [%s]]"), *DecryptedJsonAuth);
		}
	}
	else
	{//failed to convert the decrypted string into a jsonObject
		UE_LOG(LogTemp, Error, TEXT("[Failed to convert stored jsonObjectString into a jsonObject: [%s]]"), *DecryptedJsonAuth);
	}
	return bSucceeded;
}

/*
* Used for prepping our StoredAuthData_BE struct to be stored securely
* on disk using FAES encryption
*/
FSecureKey UAuth::GetSecureStorableAuth() const
{
	FSecureKey Ret;
	if (FString JSONAuthString; FJsonObjectConverter::UStructToJsonObjectString<FStoredAuthState_BE>(this->auth, JSONAuthString))
	{
		Ret.of = JSONAuthString.Len();//store the offset
		Ret.ky = USequenceEncryptor::encrypt(JSONAuthString);//encrypt and store the jsonAuthObject
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[Failed to convert stored auth data into a jsonAuthString]"));
	}

	return Ret;
}
