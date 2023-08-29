#pragma once

#include "CoreMinimal.h"
#include "TokenList.generated.h"

/*
    These structs exist because UE T structures don't support nesting BUT do allow us to wrap T structures
    in UStructs allowing us to support nesting
*/

//For use in GetTokenSuppliesMapArgs
USTRUCT(BlueprintType)
struct FTokenList
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        TArray<FString> token_list;
};
