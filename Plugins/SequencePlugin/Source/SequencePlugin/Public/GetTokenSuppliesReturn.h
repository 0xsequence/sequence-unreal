#pragma once

#include "CoreMinimal.h"
#include "Page.h"
#include "Indexer_Enums.h"
#include "TokenSupply.h"
#include "GetTokenSuppliesReturn.generated.h"

USTRUCT(BlueprintType)
struct FGetTokenSuppliesReturn
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        FPage page;
    UPROPERTY()
        TEnumAsByte<EContractType> contractType;
    UPROPERTY()
        TArray<FTokenSupply> tokenIDs;
    bool customConstructor = false;//used to tell buildresponse whether or not to use a custom constructor OR the unreal one!
    void construct(FJsonObject json_in) {};//dummy construct for templating
    void setup(FJsonObject json_in)
    {
        if (json_in.TryGetField("tokenIDs") != nullptr)
        {
            TArray<TSharedPtr<FJsonValue>> list = json_in.GetArrayField("tokenIDs");

            for (int32 i = 0; i < tokenIDs.Num(); i++)
            {
                tokenIDs[i].setup(*list[i].Get()->AsObject());
            }
        }
    }
};