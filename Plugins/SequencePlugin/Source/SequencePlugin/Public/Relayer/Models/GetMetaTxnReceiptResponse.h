#pragma once

#include "CoreMinimal.h"
#include "MetaTxnReceipt.h"
#include "GetMetaTxnReceiptResponse.generated.h"

USTRUCT()
struct FGetMetaTxnReceiptResponse
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	FMetaTxnReceipt Receipt;
};
