#pragma once

#include "CoreMinimal.h"
#include "Permission.h"
#include "SessionPermissions.generated.h"

USTRUCT()
struct SEQUENCEPLUGIN_API FSessionPermissions
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	int64 ChainId;

	UPROPERTY()
	FString SessionAddress;

	UPROPERTY()
	int64 ValueLimit;

	UPROPERTY()
	int64 Deadline;

	UPROPERTY()
	TArray<FPermission> Permissions;
};
