

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SignedMessage.generated.h"

/**
 * 
 */
USTRUCT()
struct FSignedMessage
{
	GENERATED_BODY()

	UPROPERTY()
	FString Message;

	UPROPERTY()
	FString Signature;
};