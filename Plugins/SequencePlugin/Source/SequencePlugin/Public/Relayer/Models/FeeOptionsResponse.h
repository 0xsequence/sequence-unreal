#pragma once

#include "CoreMinimal.h"
#include "Relayer/Models/FeeOption.h"
#include "FeeOptionsResponse.generated.h"

USTRUCT(BlueprintType)
struct FFeeOptionsResponse
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence SDK")
	FString Quote;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence SDK")
	bool Sponsored;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence SDK")
	TArray<FFeeOption> Options;
};
