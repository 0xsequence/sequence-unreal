// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once
#include "CoreMinimal.h"
#include "PlayFabResponseIntent.generated.h"

USTRUCT()
struct SEQUENCEPLUGIN_API FPlayFabResponse_Entity
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FString Id = "";
	UPROPERTY()
	FString TypeString = "";
};

USTRUCT()
struct SEQUENCEPLUGIN_API FPlayFabRegisterUserResponse_EntityToken
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FString EntityToken = "";
	UPROPERTY()
	FString TokenExpiration = "";
	UPROPERTY()
	FPlayFabResponse_Entity Entity;
};

USTRUCT()
struct SEQUENCEPLUGIN_API FPlayFabResponse_SettingsForUser
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	bool NeedsAttribution = false;
	UPROPERTY()
	bool GatherDeviceInfo = false;
	UPROPERTY()
	bool GatherFocusInfo = false;
};

USTRUCT()
struct SEQUENCEPLUGIN_API FPlayFabRegisterUserResponse_Data
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FString PlayFabId = "";
	UPROPERTY()
	FString SessionTicket = "";
	UPROPERTY()
	FString Username = "";
	UPROPERTY()
	FPlayFabResponse_SettingsForUser SettingsForUser;
	UPROPERTY()
	FPlayFabRegisterUserResponse_EntityToken EntityToken;
};

USTRUCT()
struct SEQUENCEPLUGIN_API FPlayFabRegisterUserResponse
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	int32 Code = -1;
	UPROPERTY()
	FString Status = "";
	UPROPERTY()
	FPlayFabRegisterUserResponse_Data Data;

	bool IsValid() const
	{
		bool IsValid = true;
		
		IsValid &= Code == 200;
		IsValid &= Status.Equals(TEXT("OK"), ESearchCase::IgnoreCase);
		
		return IsValid;
	}
};

USTRUCT()
struct SEQUENCEPLUGIN_API FPlayFabLoginUserResponse_EntityToken
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FString EntityToken = "";
	UPROPERTY()
	FString TokenExpiration = "";
	UPROPERTY()
	FPlayFabResponse_Entity Entity;
};

USTRUCT()
struct SEQUENCEPLUGIN_API FPlayFabLoginUserResponse_Data
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FString SessionTicket = "";
	UPROPERTY()
	FString PlayFabId = "";
	UPROPERTY()
	bool NewlyCreated = false;
	UPROPERTY()
	FPlayFabResponse_SettingsForUser SettingsForUser;
	UPROPERTY()
	FString LastLoginTime = "";
	UPROPERTY()
	FPlayFabLoginUserResponse_EntityToken EntityToken;
	
};

USTRUCT()
struct SEQUENCEPLUGIN_API FPlayFabLoginUserResponse
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	int32 Code = -1;
	UPROPERTY()
	FString Status = "";
	UPROPERTY()
	FPlayFabLoginUserResponse_Data Data;

	bool IsValid() const
	{
		bool IsValid = true;
		
		IsValid &= Code == 200;
		IsValid &= Status.Equals(TEXT("OK"), ESearchCase::IgnoreCase);
		
		return IsValid;
	}
};