#pragma once

#include "CoreMinimal.h"
#include "SessionArgs.h"
#include "ConnectArgs.generated.h"

USTRUCT()
struct FConnectArgs
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	FString PreferredLoginMethod;

	UPROPERTY()
	FString Email;

	UPROPERTY()
	FString Origin;
	
	UPROPERTY()
	bool IncludeImplicitSession = false;

	UPROPERTY()
	FSessionArgs Session;
};
