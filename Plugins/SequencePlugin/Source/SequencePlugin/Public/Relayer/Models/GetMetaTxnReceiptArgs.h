#pragma once

#include "CoreMinimal.h"
#include "GetMetaTxnReceiptArgs.generated.h"

USTRUCT()
struct FGetMetaTxnReceiptArgs
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	FString MetaTxID;
};
