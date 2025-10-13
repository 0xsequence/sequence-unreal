#pragma once

#include "CoreMinimal.h"
#include "Calls.h"
#include "CallTypedData.generated.h"

USTRUCT()
struct FTypedCall
{
	GENERATED_BODY()
	
	UPROPERTY()
	FString To;

	UPROPERTY()
	FString Data;

	UPROPERTY()
	int64 Value;

	UPROPERTY()
	int64 GasLimit;

	UPROPERTY()
	bool DelegateCall;

	UPROPERTY()
	bool OnlyFallback;

	UPROPERTY()
	int64 BehaviorOnError;
};

USTRUCT()
struct FMessage
{
	GENERATED_BODY()

	UPROPERTY()
	FString Space;

	UPROPERTY()
	FString Nonce;

	UPROPERTY()
	TArray<FString> Wallets;

	UPROPERTY()
	TArray<FTypedCall> Calls;
};

USTRUCT()
struct FTypedDataDomain
{
	GENERATED_BODY()

	UPROPERTY()
	FString Name;

	UPROPERTY()
	FString Version;

	UPROPERTY()
	int64 ChainId;

	UPROPERTY()
	FString VerifyingContract;
};

USTRUCT()
struct FEIP712TypeEntry
{
	GENERATED_BODY()

	UPROPERTY()
	FString Name;

	UPROPERTY()
	FString Type;
};

USTRUCT()
struct FEIP712Types
{
	GENERATED_BODY()

	UPROPERTY(meta = (DisplayName = "Calls"))
	TArray<FEIP712TypeEntry> Calls;

	UPROPERTY(meta = (DisplayName = "Call"))
	TArray<FEIP712TypeEntry> Call;
};

USTRUCT()
struct FCallTypedDataParent
{
	GENERATED_BODY()

	UPROPERTY()
	FTypedDataDomain Domain;

	UPROPERTY()
	FEIP712Types Types;

	UPROPERTY()
	FString PrimaryType;

	UPROPERTY()
	FMessage Message;
};
