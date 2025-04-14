#pragma once

#include "CoreMinimal.h"
#include "SeqCurrency.h"
#include "Dom/JsonObject.h"
#include "SeqListCurrenciesReturn.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqListCurrenciesReturn
{
    GENERATED_USTRUCT_BODY()

public:
    bool customConstructor = false;//used to tell buildresponse whether or not to use a custom constructor OR the unreal one!
    void construct(FJsonObject json_in) {};//dummy construct for templating
    
    
    // Properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Page")
    TArray<FSeqCurrency> Currencies;

    void Setup(FJsonObject json_in)
    {
        const TArray<TSharedPtr<FJsonValue>>* List;
        if (json_in.TryGetArrayField(TEXT("currencies"), List))
        {

            for (TSharedPtr<FJsonValue> Json : *List)
            {
                FSeqCurrency Currency;
                Currency.Setup(*Json->AsObject());
                Currencies.Add(Currency);
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("No  currencies field found in the SeqListCurrenciesReturn response."));
        }
    };
};