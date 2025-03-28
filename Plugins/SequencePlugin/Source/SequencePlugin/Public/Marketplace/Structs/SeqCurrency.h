#pragma once

#include "CoreMinimal.h"
#include "SeqCurrency.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqCurrency
{
	GENERATED_USTRUCT_BODY()

public:
	// Properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	int ID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	int ChainID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FString ContractAddress;
};