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

    //properties form: TMap<FString,Object>
    TSharedPtr<FJsonObject> properties;//this is our properties ref!

    //attributes form: TArray<TMap<FString, Object>>
    TArray<TSharedPtr<FJsonValue>> attributes;//this is our attributes ref!

    void setup(TSharedPtr<FJsonObject> json_in)
    {//the json object we expect here will be a mirror of what is above EXCEPT we will be snipping out what we need!
        
        const TSharedPtr<FJsonObject>* ref = &properties;
        if (!json_in.Get()->TryGetObjectField("properties", ref))
        {
            FString OutputString;
            TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
            FJsonSerializer::Serialize(json_in.ToSharedRef(), Writer);
            UE_LOG(LogTemp, Warning, TEXT("Error getting properties out of: %s"),*OutputString);
        }
        //now we create the attribute map!

        // = &attributes;
        const TArray<TSharedPtr<FJsonValue>> *lst = &attributes;
        if (!json_in.Get()->TryGetArrayField("attributes",lst))//attempt to assign!
        {
            FString OutputString;
            TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
            FJsonSerializer::Serialize(json_in.ToSharedRef(), Writer);
            UE_LOG(LogTemp, Warning, TEXT("Error getting attributes out of: %s"), *OutputString);
        }
    }
};