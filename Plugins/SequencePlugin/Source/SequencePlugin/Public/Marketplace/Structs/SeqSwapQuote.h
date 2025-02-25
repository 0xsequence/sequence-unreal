#pragma once

#include "CoreMinimal.h"
#include "SeqFeeBreakdown.h"
#include "Marketplace/Marketplace_Enums.h"
#include "Util/SequenceSupport.h"
#include "SeqSwapQuote.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqSwapQuote
{
    GENERATED_USTRUCT_BODY()

public:
    bool FromResponse(const FString& Response)
    {
        TSharedPtr<FJsonObject> Json;

        if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Response), Json))
        {
            return false;
        }
        
        if(!Json->HasField(TEXT("swapQuote")))
        {
            return false;
        }

        const TSharedPtr<FJsonObject> Values;
        const TSharedPtr<FJsonObject>* JsonOut = &Values;
        if(!Json->TryGetObjectField(TEXT("swapQuote"), JsonOut))
        {
            return false;
        }

        return FromJson(*JsonOut);
    }
    
    bool FromJson(const TSharedPtr<FJsonObject>& Json) {
        CurrencyAddress = Json->GetStringField(TEXT("currencyAddress"));
        CurrencyBalance = Json->GetStringField(TEXT("currencyBalance"));
        Price = Json->GetStringField(TEXT("price"));
        MaxPrice = Json->GetStringField(TEXT("maxPrice"));
        To = Json->GetStringField(TEXT("to"));
        TransactionData = Json->GetStringField(TEXT("transactionData"));
        TransactionValue = Json->GetStringField(TEXT("transactionValue"));
        ApproveData = Json->GetStringField(TEXT("approveData"));
        return true;
    }
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString CurrencyAddress;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString CurrencyBalance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Price;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString MaxPrice;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString To;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString TransactionData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString TransactionValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ApproveData;
};