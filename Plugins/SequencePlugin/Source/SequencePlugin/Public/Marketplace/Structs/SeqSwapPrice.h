#pragma once

#include "CoreMinimal.h"
#include "SeqFeeBreakdown.h"
#include "Marketplace/Marketplace_Enums.h"
#include "Util/SequenceSupport.h"
#include "SeqSwapPrice.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqSwapPrice
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
        
        if(!Json->HasField(TEXT("swapPrice")))
        {
            return false;
        }

        const TSharedPtr<FJsonObject> Values;
        const TSharedPtr<FJsonObject>* JsonOut = &Values;
        if(!Json->TryGetObjectField(TEXT("swapPrice"), JsonOut))
        {
            return false;
        }

        return FromJson(Values);
    }
    

    bool FromJson(const TSharedPtr<FJsonObject>& Json) {
        CurrencyAddress = Json->GetStringField(TEXT("currencyAddress"));
        CurrencyBalance = Json->GetStringField(TEXT("currencyBalance"));
        Price = Json->GetStringField(TEXT("price"));
        MaxPrice = Json->GetStringField(TEXT("maxPrice"));
        TransactionValue = Json->GetStringField(TEXT("transactionValue"));
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
    FString TransactionValue;
};