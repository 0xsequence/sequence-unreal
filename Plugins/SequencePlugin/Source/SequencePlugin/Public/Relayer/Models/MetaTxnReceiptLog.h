#pragma once

#include "CoreMinimal.h"
#include "MetaTxnReceiptLog.generated.h"

USTRUCT(BlueprintType)
struct FMetaTxnReceiptLog
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence SDK")
	FString Address;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence SDK")
	FString Data;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence SDK")
	TArray<FString> Topics;
};
