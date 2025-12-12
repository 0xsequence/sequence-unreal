#pragma once

#include "CoreMinimal.h"
#include "FeeOptionsArgs.generated.h"

USTRUCT(BlueprintType)
struct FFeeOptionsArgs
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence SDK")
	FString Wallet;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence SDK")
	FString To;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence SDK")
	FString Data;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence SDK")
	bool Simulate;
};
