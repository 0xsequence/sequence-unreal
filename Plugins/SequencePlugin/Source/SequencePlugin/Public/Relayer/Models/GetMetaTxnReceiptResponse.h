#pragma once

#include "CoreMinimal.h"
#include "MetaTxnReceipt.h"
#include "GetMetaTxnReceiptResponse.generated.h"

USTRUCT(BlueprintType)
struct FGetMetaTxnReceiptResponse
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence SDK")
	FMetaTxnReceipt Receipt;
};
