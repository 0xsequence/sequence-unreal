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

        if (json_in.TryGetField("balances") != nullptr)
        {
            TArray<TSharedPtr<FJsonValue>> list = json_in.GetArrayField("balances");
            for (int32 i = 0; i < balances.Num(); i++)
            {
                balances[i].setup(*list[i].Get()->AsObject());
            }
        }
    }
};