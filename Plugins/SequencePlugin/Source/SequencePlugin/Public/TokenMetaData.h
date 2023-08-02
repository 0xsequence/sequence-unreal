#pragma once

#include "CoreMinimal.h"
#include "AttributeMap.h"
#include "JsonObjectConverter.h"
#include "IndexerSupport.h"
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

    TMap<FString, FString> properties;

    TArray<FAttributeMap> attributes;

    void setup(FJsonObject json_in)
    {//the json object we expect here will be a mirror of what is above EXCEPT we will be snipping out what we need!
        const TSharedPtr<FJsonObject>* refPtr;
        if (json_in.TryGetObjectField("properties", refPtr))
        {
            properties = UIndexerSupport::jsonObjectParser(*refPtr);
        }

        //now we create the attribute map!
        const TArray<TSharedPtr<FJsonValue>>* lstPtr;
        if (json_in.TryGetArrayField("attributes",lstPtr))//attempt to assign!
        {
            for (TSharedPtr<FJsonValue> i : *lstPtr)
            {
                //we need to convert i to an object! then will be able to proceed!
                const TSharedPtr<FJsonObject>* iObjPtr;
                if (i.Get()->TryGetObject(iObjPtr))
                {//failed to convert
                    FAttributeMap attr;
                    attr.setup(*iObjPtr);
                    attributes.Add(attr);
                }
            }
        }
    }
};