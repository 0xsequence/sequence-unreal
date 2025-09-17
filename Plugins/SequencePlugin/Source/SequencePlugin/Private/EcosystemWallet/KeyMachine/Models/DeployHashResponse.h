#pragma once

#include "CoreMinimal.h"
#include "DeployHashResponse.generated.h"

USTRUCT()
struct FDeployHashContext
{
	GENERATED_BODY()

public:
	UPROPERTY()
	int32 Version;
	
	UPROPERTY()
	FString Factory;

	UPROPERTY()
	FString GuestModule;

	UPROPERTY()
	FString MainModule;

	UPROPERTY()
	FString MainModuleUpgradable;

	UPROPERTY()
	FString WalletCreationCode;
};

USTRUCT()
struct FDeployHashResponse
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	FString DeployHash;

	UPROPERTY()
	FDeployHashContext Context;
};
