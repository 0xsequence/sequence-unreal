#pragma once

#include "CoreMinimal.h"
#include "SendMetaTxnResponse.generated.h"

USTRUCT(BlueprintType)
struct FSendMetaTxnResponse
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence SDK")
	bool Status;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence SDK")
	FString TxnHash;
};
