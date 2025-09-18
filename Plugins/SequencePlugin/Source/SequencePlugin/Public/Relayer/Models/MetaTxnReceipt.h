#pragma once

#include "CoreMinimal.h"
#include "MetaTxnReceiptLog.h"
#include "MetaTxnReceipt.generated.h"

USTRUCT(BlueprintType)
struct FMetaTxnReceipt
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence SDK")
	FString Id;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence SDK")
	FString Status;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence SDK")
	FString RevertReason;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence SDK")
	int32 Index;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence SDK")
	FString TxnReceipt;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence SDK")
	FString TxnHash;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence SDK")
	FString BlockNumber;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence SDK")
	TArray<FMetaTxnReceiptLog> Logs;
};
