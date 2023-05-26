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
};