#pragma once

#include "CoreMinimal.h"
#include "SendMetaTxnResponse.generated.h"

USTRUCT()
struct FSendMetaTxnResponse
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	bool Status;
	
	UPROPERTY()
	FString TxnHash;
};
