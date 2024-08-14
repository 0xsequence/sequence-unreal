// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once
#include "CoreMinimal.h"
#include "Util/SequenceSupport.h"
#include "SequenceAuthResponseIntent.generated.h"

//InitiateAuth//

USTRUCT()
struct SEQUENCEPLUGIN_API FSeqInitiateAuthResponse_Data
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY()
	FString SessionId = "";
	UPROPERTY()
	FString IdentityType = "";
	UPROPERTY()
	int64 ExpiresIn = -1;
	UPROPERTY()
	FString Challenge = "";
};

USTRUCT()
struct SEQUENCEPLUGIN_API FSeqInitiateAuthResponse_Response
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY()
	FString Code = "";
	UPROPERTY()
	FSeqInitiateAuthResponse_Data Data;
	
	bool IsValid() const
	{
		bool IsValid = true;
		IsValid &= Code.Equals(TEXT("authInitiated"), ESearchCase::IgnoreCase);
		return IsValid;
	}
};

USTRUCT()
struct SEQUENCEPLUGIN_API FSeqInitiateAuthResponse
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY()
	FSeqInitiateAuthResponse_Response Response;

	bool IsValid() const
	{
		return Response.IsValid();
	}
};

//InitiateAuth//

//OpenSession//

USTRUCT()
struct SEQUENCEPLUGIN_API FSeqOpenSessionResponse_Identity
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY()
	FString Type = "";
	UPROPERTY()
	FString Iss = "";
	UPROPERTY()
	FString Sub = "";
	UPROPERTY()
	FString Email = "";
};

USTRUCT()
struct SEQUENCEPLUGIN_API FSeqOpenSessionResponse_Session
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY()
	FString Id = "";
	UPROPERTY()
	int64 ProjectId = -1;
	UPROPERTY()
	FString UserId = "";
	UPROPERTY()
	FSeqOpenSessionResponse_Identity Identity;
	UPROPERTY()
	FString FriendlyName = "";
	UPROPERTY()
	FString CreatedAt = "";
	UPROPERTY()
	FString RefreshedAt = "";
	UPROPERTY()
	FString ExpiresAt = "";

	int64 GetCreatedAt() const
	{
		return USequenceSupport::StringDateToUnixDate(CreatedAt);
	}

	int64 GetRefreshedAt() const
	{
		return USequenceSupport::StringDateToUnixDate(RefreshedAt);
	}

	int64 GetExpiresAt() const
	{
		return USequenceSupport::StringDateToUnixDate(ExpiresAt);
	}
};

USTRUCT()
struct SEQUENCEPLUGIN_API FSeqOpenSessionResponse_Data
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY()
	FString SessionId = "";
	UPROPERTY()
	FString Wallet = "";
};

USTRUCT()
struct SEQUENCEPLUGIN_API FSeqOpenSessionResponse_Response
{
	GENERATED_USTRUCT_BODY()	
	UPROPERTY()
	FString Code = "";
	UPROPERTY()
	FSeqOpenSessionResponse_Data Data;

	bool IsValid() const
	{
		bool IsValid = true;
		IsValid &= Code.Equals(TEXT("sessionOpened"), ESearchCase::IgnoreCase);
		return IsValid;
	}
};

USTRUCT()
struct SEQUENCEPLUGIN_API FSeqOpenSessionResponse
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY()
	FSeqOpenSessionResponse_Session Session;
	UPROPERTY()
	FSeqOpenSessionResponse_Response Response;

	int64 GetCreatedAt() const
	{
		return Session.GetCreatedAt();
	}

	int64 GetRefreshedAt() const
	{
		return Session.GetRefreshedAt();
	}

	int64 GetExpiresAt() const
	{
		return Session.GetExpiresAt();
	}
	
	bool IsValid() const
	{
		bool IsValid = true;
		IsValid &= Response.IsValid();
		return IsValid;
	}
};

//OpenSession//