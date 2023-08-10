#pragma once

#include "CoreMinimal.h"
#include "Version.generated.h"

USTRUCT(BlueprintType)
struct FVersion
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        FString webrpcVersion = "";
    UPROPERTY()
        FString schemaVersion = "";
    UPROPERTY()
        FString schemaHash = "";
    UPROPERTY()
        FString appVersion = "";
};