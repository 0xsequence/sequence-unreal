#pragma once

#include "CoreMinimal.h"
#include "SeqEtherBalance.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqEtherBalance
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        FString accountAddress = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        int64 balanceWei = -1;
};