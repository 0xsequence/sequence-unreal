#pragma once

#include "CoreMinimal.h"
#include "Relayer/Models/FeeOption.h"
#include "FeeOptionsResponse.generated.h"

USTRUCT()
struct FFeeOptionsResponse
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	FString Quote;

	UPROPERTY()
	bool Sponsored;
	
	UPROPERTY()
	TArray<FFeeOption> Options;
};
