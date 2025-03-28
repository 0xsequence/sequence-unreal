#pragma once

#include "CoreMinimal.h"
#include "SeqCurrency.h"
#include "SeqListCurrenciesReturn.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqListCurrenciesReturn
{
    GENERATED_USTRUCT_BODY()

public:
    bool customConstructor = false;//used to tell buildresponse whether or not to use a custom constructor OR the unreal one!
    void construct(FJsonObject json_in) {};//dummy construct for templating
    void Setup(FJsonObject json_in) {};//dummy method to allow for templating in build response!
    
    // Properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Page")
    TArray<FSeqCurrency> Currencies;
};