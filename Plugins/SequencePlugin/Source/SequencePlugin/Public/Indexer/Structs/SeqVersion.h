#pragma once

#include "CoreMinimal.h"
#include "SeqVersion.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqVersion
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        FString webrpcVersion = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        FString schemaVersion = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        FString schemaHash = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        FString appVersion = "";
};