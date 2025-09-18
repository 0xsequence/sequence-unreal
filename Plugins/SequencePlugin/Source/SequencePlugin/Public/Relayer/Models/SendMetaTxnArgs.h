#pragma once

#include "CoreMinimal.h"
#include "IntentPrecondition.h"
#include "MetaTxn.h"
#include "SendMetaTxnArgs.generated.h"

USTRUCT(BlueprintType)
struct FSendMetaTxnArgs
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence SDK")
	FMetaTxn Call;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence SDK")
	FString Quote;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence SDK")
	int32 ProjectID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence SDK")
	TArray<FIntentPrecondition> Preconditions;
};
