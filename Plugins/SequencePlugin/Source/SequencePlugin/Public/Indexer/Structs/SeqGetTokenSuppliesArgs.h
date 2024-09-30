// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "SeqPage.h"
#include "SeqGetTokenSuppliesArgs.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqGetTokenSuppliesArgs
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        FString contractAddress = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        bool includeMetaData = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        TOptional<FSeqPage> page;

    bool customGetter = true;
    FString GetArgs() { 
        TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
    
        JsonObject->SetStringField("contractAddress", contractAddress);
        JsonObject->SetBoolField("includeMetaData", includeMetaData);
    
        if (page.IsSet())
        {
            TSharedPtr<FJsonObject> PageObject = MakeShared<FJsonObject>();
            FJsonObjectConverter::UStructToJsonObject(FSeqPage::StaticStruct(), &(page.GetValue()), PageObject.ToSharedRef(), 0, 0);
            JsonObject->SetObjectField("page", PageObject);
        }

        FString OutputString = "";
        TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
        FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

        return OutputString;
    }
};