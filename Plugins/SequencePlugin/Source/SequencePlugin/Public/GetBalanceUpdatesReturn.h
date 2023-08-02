#pragma once

#include "CoreMinimal.h"
#include "Page.h"
#include "TokenBalance.h"
#include "GetBalanceUpdatesReturn.generated.h"

USTRUCT(BlueprintType)
struct FGetBalanceUpdatesReturn
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        FPage page;
    UPROPERTY()
        TArray<FTokenBalance> balances;
    bool customConstructor = false;//used to tell buildresponse whether or not to use a custom constructor OR the unreal one!
    void construct(FJsonObject json_in) {};//dummy construct for templating
    void setup(FJsonObject json_in)
    {
        const TArray<TSharedPtr<FJsonValue>> *lst;
        if (json_in.TryGetArrayField("balances", lst))//verify it's an arrayfield and valid!
        {
            for (int32 i = 0; i < balances.Num(); i++)
            {
                const TSharedPtr<FJsonObject> *itemObj;
                if ((*lst)[i].Get()->TryGetObject(itemObj))//verify that there's an object here!
                {
                    balances[i].setup(*itemObj->Get());
                }
            }
        }
    }
};