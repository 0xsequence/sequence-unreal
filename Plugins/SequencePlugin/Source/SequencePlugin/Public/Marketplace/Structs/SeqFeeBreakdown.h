#pragma once

#include "CoreMinimal.h"
#include "Indexer/Structs/Struct_Data.h"
#include "Marketplace/Marketplace_Enums.h"
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
        ret->SetStringField("kind", Kind);
        ret->SetStringField("recipientAddress", RecipientAddress);
        ret->SetNumberField("bps", Bps);
        return ret;
    }

    void Setup(FJsonObject& json_in)
    {
        FString OutputString;
        TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
        FJsonSerializer::Serialize(MakeShared<FJsonObject>(json_in), Writer);

        // Log the full JSON input
        UE_LOG(LogTemp, Log, TEXT("Full JSON Input: %s"), *OutputString);

        json_in.TryGetStringField(TEXT("kind"), Kind);
        json_in.TryGetStringField(TEXT("recipientAddress"), RecipientAddress);
        json_in.TryGetNumberField(TEXT("bps"), Bps);
    }
};