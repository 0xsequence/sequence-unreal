#pragma once

#include "CoreMinimal.h"
#include "IntentPrecondition.h"
#include "MetaTxn.h"
#include "SendMetaTxnArgs.generated.h"

USTRUCT()
struct FSendMetaTxnArgs
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	FMetaTxn Call;
	
	UPROPERTY()
	FString Quote;

	UPROPERTY()
	int32 ProjectID;

	UPROPERTY()
	TArray<FIntentPrecondition> Preconditions;
};
