#pragma once

#include "CoreMinimal.h"
#include "EcosystemWallet/Primitives/Permission/SessionPermissions.h"
#include "ConnectArgs.generated.h"

USTRUCT()
struct FConnectArgs
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	FString SessionAddress;

	UPROPERTY()
	FString PreferredLoginMethod;

	UPROPERTY()
	FString Email;

	UPROPERTY()
	FString Origin;
	
	UPROPERTY()
	bool bIncludeImplicitSession = false;

	UPROPERTY()
	FSessionPermissions Permissions;
};
