#pragma once

#include "CoreMinimal.h"
#include "EtherBalance.h"
#include "GetEtherBalanceReturn.generated.h"

USTRUCT(BlueprintType)
struct FGetEtherBalanceReturn
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        FEtherBalance balance;
};