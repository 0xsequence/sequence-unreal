#pragma once

#include "EcosystemWallet/SeqCall.generated.h"

UENUM()
enum class EBehaviorOnError : uint8
{
	Ignore UMETA(DisplayName = "Ignore"),
	Revert UMETA(DisplayName = "Revert"),
	Abort UMETA(DisplayName = "Abort")
};

USTRUCT()
struct FSeqCall
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	FString To;

	UPROPERTY()
	int64 Value;

	UPROPERTY()
	TArray<uint8> Data;

	UPROPERTY()
	int64 GasLimit;

	UPROPERTY()
	bool DelegateCall;

	UPROPERTY()
	bool OnlyFallback;

	UPROPERTY()
	EBehaviorOnError BehaviorOnError;
};
