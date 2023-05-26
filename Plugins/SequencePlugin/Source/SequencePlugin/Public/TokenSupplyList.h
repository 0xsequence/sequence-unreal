#pragma once

#include "CoreMinimal.h"
#include "TokenSupply.h"
#include "TokenSupplyList.generated.h"


/*
    These structs exist because UE T structures don't support nesting BUT do allow us to wrap T structures
    in UStructs allowing us to support nesting
*/

//For use in TokenSuppliesMapReturn
USTRUCT(BlueprintType)
struct FTokenSupplyList
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        TArray<FTokenSupply> token_supply_list;
};