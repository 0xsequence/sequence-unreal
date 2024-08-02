// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once
#include "CoreMinimal.h"
#include "Indexer/IndexerSupport.h"
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

	bool IsValid() const
	{
		bool IsValid = true;
		IsValid &= SessionId.Len() > 0;
		IsValid &= IdentityType.Len() > 0;		
		return IsValid;
	}
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
		IsValid &= Data.IsValid();
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

	bool IsValid() const
	{
		bool IsValid = true;
		IsValid &= Type.Len() > 0;
		IsValid &= Sub.Len() > 0;
		IsValid &= Email.Len() > 0;
		return IsValid;
	}
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
		return UIndexerSupport::StringDateToUnixDate(CreatedAt);
	}

	int64 GetRefreshedAt() const
	{
		return UIndexerSupport::StringDateToUnixDate(RefreshedAt);
	}

	int64 GetExpiresAt() const
	{
		return UIndexerSupport::StringDateToUnixDate(ExpiresAt);
	}

	bool IsValid() const
	{
		bool IsValid = true;
		IsValid &= Id.Len() > 0;
		IsValid &= ProjectId > -1;
		IsValid &= UserId.Len() > 0;
		IsValid &= Identity.IsValid();
		IsValid &= FriendlyName.Len() > 0;
		IsValid &= CreatedAt.Len() > 0;
		IsValid &= RefreshedAt.Len() > 0;
		IsValid &= ExpiresAt.Len() > 0;
		IsValid &= GetExpiresAt() > -1;
		IsValid &= GetCreatedAt() > -1;
		IsValid &= GetRefreshedAt() > -1;
		return IsValid;
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

	bool IsValid() const
	{
		bool IsValid = true;
		IsValid &= SessionId.Len() > 0;
		IsValid &= Wallet.Len() > 0;
		return IsValid;
	}
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
		IsValid &= Data.IsValid();
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
		IsValid &= Session.IsValid();
		IsValid &= Response.IsValid();
		return IsValid;
	}
};

//OpenSession//