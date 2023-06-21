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
    //UPROPERTY()
       // TMap<FString, UObject*> properties; //these are both producing errors!
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
    //UPROPERTY()
        //TArray<FAttributeMap> attributes; // these are both producing errors

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
              //  attributes.Add(w_in);//now add our custom structure in
            }
        }
    };

    FString Get()
    {
        FString out = "{\"attributes\":[";
        /*
       for (auto i : attributes)
        {//Here we append to the returning json object with our own data.
            FString json_obj_str;
            FJsonObjectConverter::UStructToJsonObjectString<FAttributeMap>(i, json_obj_str);

            json_obj_str.RemoveSpacesInline();
            FString srch_prefix = TEXT("{\"attribute_map\":");
            FString srch_n = TEXT("\n");
            FString srch_r = TEXT("\r");
            FString srch_t = TEXT("\t");
            FString replace = TEXT("");

            const TCHAR* srch_ptr = *srch_prefix;
            const TCHAR* rep_ptr = *replace;
            const TCHAR* srch_ptr_n = *srch_n;
            const TCHAR* srch_ptr_r = *srch_r;
            const TCHAR* srch_ptr_t = *srch_t;

            (json_obj_str).ReplaceInline(srch_ptr_n, rep_ptr, ESearchCase::IgnoreCase);//remove \n
            (json_obj_str).ReplaceInline(srch_ptr_r, rep_ptr, ESearchCase::IgnoreCase);//remove \r
            (json_obj_str).ReplaceInline(srch_ptr_t, rep_ptr, ESearchCase::IgnoreCase);//remove \t

            //we've removed all the excess data now we need clean it up
            //{\"attribute_map\": data }
            (json_obj_str).ReplaceInline(srch_ptr, rep_ptr, ESearchCase::IgnoreCase);//remove \t
            json_obj_str.LeftChopInline(1);//remove the tailing }
            out.Append(json_obj_str);
            out.Append(",");
        }
        out.LeftChopInline(1);//easy way of removing the trailing "," without if's
        out.Append("]}");
        */
        return out;
    };

    
};