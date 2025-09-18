#pragma once

#include "CoreMinimal.h"
#include "MetaTxnReceiptLog.h"
#include "MetaTxnReceipt.generated.h"

USTRUCT()
struct FMetaTxnReceipt
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	FString Id;
	
	UPROPERTY()
	FString Status;
	
	UPROPERTY()
	FString RevertReason;
	
	UPROPERTY()
	int32 Index;
	
	UPROPERTY()
	FString TxnReceipt;
	
	UPROPERTY()
	FString TxnHash;
	
	UPROPERTY()
	FString BlockNumber;

	UPROPERTY()
	TArray<FMetaTxnReceiptLog> Logs;
};
