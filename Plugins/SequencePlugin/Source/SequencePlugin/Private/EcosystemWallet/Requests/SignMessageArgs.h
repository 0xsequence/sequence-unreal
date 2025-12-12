#pragma once

#include "CoreMinimal.h"
#include "SignMessageArgs.generated.h"

USTRUCT()
struct FSignMessageArgs
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	FString ChainId;
	
	UPROPERTY()
	FString Address;

	UPROPERTY()
	FString Message;

	bool CustomJson = false;

	FString ToJson() const { return ""; }
};
