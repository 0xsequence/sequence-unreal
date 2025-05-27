#pragma once

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
#include "Util/SequenceSupport.h"
#include "SeqSwapQuote.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqSwapQuote
{
    GENERATED_USTRUCT_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence", meta = (JsonFieldName = "currencyAddress"))
    FString CurrencyAddress;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence", meta = (JsonFieldName = "currencyBalance"))
    FString CurrencyBalance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence", meta = (JsonFieldName = "price"))
    FString Price;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence", meta = (JsonFieldName = "maxPrice"))
    FString MaxPrice;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence", meta = (JsonFieldName = "to"))
    FString To;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence", meta = (JsonFieldName = "transactionData"))
    FString TransactionData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence", meta = (JsonFieldName = "transactionValue"))
    FString TransactionValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence", meta = (JsonFieldName = "approveData"))
    FString ApproveData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence", meta = (JsonFieldName = "amount"))
    FString Amount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence", meta = (JsonFieldName = "amountMin"))
    FString AmountMin;
};