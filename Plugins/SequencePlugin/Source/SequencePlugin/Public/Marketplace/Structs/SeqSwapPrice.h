#pragma once

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
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
        
        if(!Json->HasField(TEXT("swapPermit2Price")))
        {
            return false;
        }

        const TSharedPtr<FJsonObject> Values;
        const TSharedPtr<FJsonObject>* JsonOut = &Values;
        if(!Json->TryGetObjectField(TEXT("swapPermit2Price"), JsonOut))
        {
            return false;
        }

        return FromJson(*JsonOut);
    }
    

    bool FromJson(const TSharedPtr<FJsonObject>& Json) {
        CurrencyAddress = Json->GetStringField(TEXT("currencyAddress"));
        Price = Json->GetStringField(TEXT("price"));
        return true;
    }
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
    FString CurrencyAddress;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
    FString Price;
};