#pragma once

#include "CoreMinimal.h"
#include "GetTokenSuppliesMapArgs.generated.h"

USTRUCT(BlueprintType)
struct FGetTokenSuppliesMapArgs
{
    GENERATED_USTRUCT_BODY()
public:
  //  UPROPERTY()
      //  TMap<FString, TArray<FString>> tokenMap;
    UPROPERTY()
        bool includeMetaData;
};