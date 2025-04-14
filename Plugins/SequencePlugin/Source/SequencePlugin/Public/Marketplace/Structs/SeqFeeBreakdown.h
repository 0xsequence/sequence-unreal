#pragma once

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
#include "SeqFeeBreakdown.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqFeeBreakdown
{
    GENERATED_USTRUCT_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
    FString Kind;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
    FString RecipientAddress;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
    int64  Bps;

    TSharedPtr<FJsonObject> GetJson()
    {
        TSharedPtr<FJsonObject> ret = MakeShareable(new FJsonObject);
        ret.Get()->SetStringField("kind", Kind);
        ret.Get()->SetStringField("recipientAddress", RecipientAddress);
        ret.Get()->SetNumberField("bps", Bps);
        return ret;
    }

    void Setup(FJsonObject& json_in)
    {
        json_in.TryGetStringField(TEXT("kind"), Kind);
        json_in.TryGetStringField(TEXT("recipientAddress"), RecipientAddress);
        json_in.TryGetNumberField(TEXT("bps"), Bps);
    }
};