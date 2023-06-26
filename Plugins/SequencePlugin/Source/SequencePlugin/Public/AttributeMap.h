#pragma once

#include "CoreMinimal.h"
#include "AttributeMap.generated.h"

/*
    These structs exist because UE T structures don't support nesting BUT do allow us to wrap T structures
    in UStructs allowing us to support nesting
*/

//for use in TokenMetaData
USTRUCT(BlueprintType)
struct FAttributeMap
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        TMap<FString, UObject*> attribute_map;//this will probably need to be stored as is as well!
};
