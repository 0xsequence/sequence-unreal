#pragma once

#include "CoreMinimal.h"
#include "AttributeMap.h"
#include "JsonObjectConverter.h"
#include "TokenMetaData.generated.h"

USTRUCT(BlueprintType)
struct FTokenMetaData
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        int64 tokenId;
    UPROPERTY()
        FString contractAddress;
    UPROPERTY()
        FString name;
    UPROPERTY()
        FString description;
    UPROPERTY()
        FString image;
    UPROPERTY()
        float decimals;
    UPROPERTY()
        TMap<FString, UObject*> properties;
    UPROPERTY()
        FString video;
    UPROPERTY()
        FString audio;
    UPROPERTY()
        FString image_data;
    UPROPERTY()
        FString external_url;
    UPROPERTY()
        FString background_color;
    UPROPERTY()
        FString animation_url;
    UPROPERTY()
        TArray<FAttributeMap> attributes;

    FTokenMetaData() {};

    FTokenMetaData(FJsonObject json_in)
    {//need to manually parse this so we get a proper json result!
        TArray<TSharedPtr<FJsonValue>> data = json_in.GetArrayField("attributes");
        TSharedPtr<FJsonObject> json_step;
        FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create("{ \"attribute_map\": {} }"), json_step);
        FAttributeMap w_in;
        for (auto i : data)
        {//the main list get me each individual map to assign
            TSharedPtr<FJsonObject> lcl_i = i.Get()->AsObject();
            json_step.Get()->SetObjectField("attribute_map", lcl_i);

            //now convert this to a FStruct_1 then add it in!
            if (FJsonObjectConverter::JsonObjectToUStruct<FAttributeMap>(json_step.ToSharedRef(), &w_in))
            {
                attributes.Add(w_in);//now add our custom structure in
            }
        }
    };

    
};