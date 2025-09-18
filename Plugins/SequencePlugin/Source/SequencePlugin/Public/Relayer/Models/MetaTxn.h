#pragma once

#include "CoreMinimal.h"
#include "MetaTxn.generated.h"

USTRUCT()
struct FMetaTxn
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	FString WalletAddress;

	UPROPERTY()
	FString Contract;
	
	UPROPERTY()
	FString Input;
};
