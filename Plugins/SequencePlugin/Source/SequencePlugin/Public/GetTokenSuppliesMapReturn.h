#pragma once

#include "CoreMinimal.h"
#include "TokenSupply.h"
#include "TokenSupplyList.h"
#include "GetTokenSuppliesMapReturn.generated.h"

USTRUCT(BlueprintType)
struct FGetTokenSuppliesMapReturn
{
    GENERATED_USTRUCT_BODY()
public:
   UPROPERTY()
      TMap<FString, FTokenSupplyList> supplies;

};