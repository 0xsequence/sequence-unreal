#pragma once

#include "CoreMinimal.h"
#include "IntentPrecondition.generated.h"

USTRUCT(BlueprintType)
struct FIntentPrecondition
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence SDK")
	FString Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence SDK")
	FString ChainId;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence SDK")
	FString Data;
};
