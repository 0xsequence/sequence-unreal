#pragma once

#include "CoreMinimal.h"
#include "MetaTxnReceiptLog.generated.h"

USTRUCT()
struct FMetaTxnReceiptLog
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	FString Address;
	
	UPROPERTY()
	FString Data;
	
	UPROPERTY()
	TArray<FString> Topics;
};
