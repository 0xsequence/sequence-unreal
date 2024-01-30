

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Session.generated.h"

/**
 * 
 */
USTRUCT()
struct FSession
{
	GENERATED_BODY()

	UPROPERTY()
	FString Id;
	UPROPERTY()
	FString Address;
	UPROPERTY()
	FString UserID;
	UPROPERTY()
	FString ProjectID;
	UPROPERTY()
	FString Issuer;
	UPROPERTY()
	FString Subject;
	UPROPERTY()
	FString FriendlyName;
	UPROPERTY()
	FString CreatedAt;
	UPROPERTY()
	FString RefreshedAt;
	UPROPERTY()
	FString ExpiresAt;
};
