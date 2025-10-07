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
	int64 ChainId;

	UPROPERTY()
	int64 ValueLimit;

	UPROPERTY()
	int64 Deadline;

	UPROPERTY()
	TArray<FPermission> Permissions;

	FSessionArgs() {}
	explicit FSessionArgs(const FString& InSessionAddress)
		: SessionAddress(InSessionAddress)
		, ChainId(0)
		, ValueLimit(0)
		, Deadline(0)
		, Permissions(TArray<FPermission>())
	{}

	FSessionArgs(const FString& InSessionAddress, int64 InChainId, int64 InValueLimit, int64 InDeadline, const TArray<FPermission>& InPermissions)
		: SessionAddress(InSessionAddress)
		, ChainId(InChainId)
		, ValueLimit(InValueLimit)
		, Deadline(InDeadline)
		, Permissions(InPermissions)
	{}
};
