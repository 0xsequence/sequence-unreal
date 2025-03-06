// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Indexer/Structs/SeqAttributeMap.h"
#include "Util/SequenceSupport.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "SeqTokenMetaData.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqTokenMetaData
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        int64 tokenId = -1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        FString contractAddress = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        FString name = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        FString description = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        FString image = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        float decimals = 0.0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        FString video = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        FString audio = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        FString image_data = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        FString external_url = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        FString background_color = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        FString animation_url = "";
    //These 2 need to be manually built
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        TMap<FString, FString> properties;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        TArray<FSeqAttributeMap> attributes;

    /*
    * Used to get the json object formed by this struct
    * used for args and testing
    */
    TSharedPtr<FJsonObject> GetJson()
    {
        TSharedPtr<FJsonObject> ret = MakeShareable<FJsonObject>(new FJsonObject);
        ret.Get()->SetNumberField("tokenId", tokenId);
        ret.Get()->SetStringField("contractAddress", contractAddress);
        ret.Get()->SetStringField("name", name);
        ret.Get()->SetStringField("description", description);
        ret.Get()->SetStringField("image", image);
        ret.Get()->SetNumberField("decimals", decimals);
        ret.Get()->SetStringField("video", video);
        ret.Get()->SetStringField("audio", audio);
        ret.Get()->SetStringField("image_data", image_data);
        ret.Get()->SetStringField("external_url", external_url);
        ret.Get()->SetStringField("background_color", background_color);
        ret.Get()->SetStringField("animation_url", animation_url);

        //need to build properties object now!
        TSharedPtr<FJsonObject> propObj = MakeShareable<FJsonObject>(new FJsonObject);

        TArray<FString> keys;
        properties.GetKeys(keys);
        for (FString key : keys)
        {
            propObj.Get()->SetStringField(key,*properties.Find(key));
        }

        ret.Get()->SetObjectField("properties", propObj);

        //need to build attribute list now! easier to work this in raw string form then UE json
        FString attributeString = "[";
        for (FSeqAttributeMap aMap : attributes)
        {
            attributeString.Append(USequenceSupport::SimplifyString(aMap.GetJsonString()));
            attributeString.Append(",");
        }
        if (attributes.Num() > 0)
        {
            attributeString.RemoveAt(attributeString.Len() - 1);//remove the last index as it'll be a wrongly placed ,
        }
        attributeString.Append("]");
        ret.Get()->SetStringField("attributes", attributeString);

        return ret;
    };

    /*
    * Used to handle Edge cases with UE's json parsing
    */
    void Setup(FJsonObject json_in)
    {//the json object we expect here will be a mirror of what is above EXCEPT we will be snipping out what we need!

        UE_LOG(LogTemp, Warning, TEXT("seq token meta data setup start"));
        
        FString tokenIdString;
        if (json_in.TryGetStringField(TEXT("tokenId"), tokenIdString))
        {
            tokenId = FCString::Atoi64(*tokenIdString);
        }
        FString decimalsString;
        if (json_in.TryGetStringField(TEXT("decimals"), decimalsString))
        {
            decimals = FCString::Atoi64(*decimalsString);
        }

        json_in.TryGetStringField(TEXT("name"), name);
        json_in.TryGetStringField(TEXT("description"), description);
        json_in.TryGetStringField(TEXT("image"), image);
        json_in.TryGetStringField(TEXT("video"), video);
        json_in.TryGetStringField(TEXT("audio"), audio);
        json_in.TryGetStringField(TEXT("image_data"), image_data);
        json_in.TryGetStringField(TEXT("background_color"), background_color);
        json_in.TryGetStringField(TEXT("animation_url"), animation_url);
        json_in.TryGetStringField(TEXT("external_url"), external_url);

        attributes.Empty();
        properties.Empty();
        
        const TSharedPtr<FJsonObject>* refPtr;
        if (json_in.TryGetObjectField(TEXT("properties"), refPtr))
        {
            properties = USequenceSupport::JSONObjectParser(*refPtr);
        }
        
        //now we create the attribute map!
        const TArray<TSharedPtr<FJsonValue>>* lstPtr;
        if (json_in.TryGetArrayField(TEXT("attributes"),lstPtr))//attempt to assign!
        {
            for (TSharedPtr<FJsonValue> i : *lstPtr)
            {
                //we need to convert i to an object! then will be able to proceed!
                const TSharedPtr<FJsonObject>* iObjPtr;
                if (i.Get()->TryGetObject(iObjPtr))
                {//failed to convert
                    FSeqAttributeMap attr;
                    attr.setup(*iObjPtr);
                    attributes.Add(attr);
                }
            }
        }


        UE_LOG(LogTemp, Warning, TEXT("seq token meta data setup done"));
    }
};