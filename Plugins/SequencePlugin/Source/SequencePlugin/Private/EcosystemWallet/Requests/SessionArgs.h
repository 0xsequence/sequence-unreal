#pragma once
#include "EcosystemWallet/Primitives/Permission/Permission.h"
#include "SessionArgs.generated.h"

USTRUCT()
struct FSessionArgs
{
	GENERATED_BODY()

	UPROPERTY()
	FString SessionAddress;

	UPROPERTY()
	FString ChainId;

	UPROPERTY()
	FString ValueLimit;

	UPROPERTY()
	FString Deadline;

	UPROPERTY()
	TArray<FPermission> Permissions;

	FSessionArgs() {}
	explicit FSessionArgs(const FString& InSessionAddress)
		: SessionAddress(InSessionAddress)
		, ChainId("0")
		, ValueLimit("0")
		, Deadline("0")
		, Permissions(TArray<FPermission>())
	{}

	FSessionArgs(const FString& InSessionAddress, const FString& InChainId, const FString& InValueLimit, const FString& InDeadline, const TArray<FPermission>& InPermissions)
		: SessionAddress(InSessionAddress)
		, ChainId(InChainId)
		, ValueLimit(InValueLimit)
		, Deadline(InDeadline)
		, Permissions(InPermissions)
	{}
};
