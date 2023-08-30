// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Util/Structs/BE_Structs.h"
#include "Auth.generated.h"

/*
* Used for storage of key portion of KVP in sMap
*/
USTRUCT(BlueprintType)
struct FSecureKey
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int32 of = -1;//encrypted auth data length
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString ky = "";//encrypted auth data
};

/**
 * 
 */
UCLASS()
class SEQUENCEPLUGIN_API UAuth : public UObject
{
	GENERATED_BODY()
public:
	//Auth data
	FStoredAuthState_BE auth;

	/*
	* Used for setting up fresh auth data for this session,
	* used in the case when we had no auth data stored
	*/
	void SetNewSecureStorableAuth(FStoredAuthState_BE NewAuthData);
	
	/*
	* Used to take stored Auth data decrypt it, and reconstruct a FStoredAuthState_BE Struct
	* if this process fails at any step we return false to let the caller know we were unable to parse
	* the data we were given
	*/
	bool SetSecureStorableAuth(FSecureKey SecureStoredAuth);

	/*
	* Used for prepping our StoredAuthData_BE struct to be stored securely
	* on disk using FAES encryption
	*/
	FSecureKey GetSecureStorableAuth() const;
};