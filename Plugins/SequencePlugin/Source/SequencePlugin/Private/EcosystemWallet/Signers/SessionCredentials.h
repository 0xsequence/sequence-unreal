#pragma once

#include "CoreMinimal.h"
#include "SessionCredentials.generated.h"

USTRUCT()
struct SEQUENCEPLUGIN_API FSessionCredentials
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	FString PrivateKey;
	
	UPROPERTY()
	FString Address;
	
	UPROPERTY()
	FString SessionAddress;
	
	UPROPERTY()
	FString LoginMethod;
	
	UPROPERTY()
	FString UserEmail;
};
