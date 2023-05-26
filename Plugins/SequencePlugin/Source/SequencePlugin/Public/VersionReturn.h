#pragma once

#include "CoreMinimal.h"
#include "Version.h"
#include "VersionReturn.generated.h"

USTRUCT(BlueprintType)
struct FVersionReturn
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        FVersion version;
};