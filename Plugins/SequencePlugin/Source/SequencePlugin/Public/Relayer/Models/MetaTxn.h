#pragma once

#include "CoreMinimal.h"
#include "MetaTxn.generated.h"

USTRUCT(BlueprintType)
struct FMetaTxn
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence SDK")
	FString WalletAddress;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence SDK")
	FString Contract;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence SDK")
	FString Input;
};
