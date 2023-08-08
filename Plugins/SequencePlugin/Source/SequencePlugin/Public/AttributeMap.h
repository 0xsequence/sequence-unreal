#pragma once

#include "CoreMinimal.h"
#include "IndexerSupport.h"
#include "AttributeMap.generated.h"

/*
    These structs exist because UE T structures don't support nesting BUT do allow us to wrap T structures
    in UStructs allowing us to support nesting
*/

//for use in TokenMetaData
USTRUCT(BlueprintType)
struct FAttributeMap
{
    GENERATED_USTRUCT_BODY()
public:
        TMap<FString, FString> attribute_map;
        
        //used for testing
        FString GetJsonString()
        {
            FString ret = "{";
            
            TArray<FString> keys;
            attribute_map.GetKeys(keys);

            for (FString key : keys)
            {
                ret.Append("\"");
                ret.Append(key);
                ret.Append("\"");
                ret.Append(":");
                ret.Append(*attribute_map.Find(key));
                ret.Append(",");
            }
            if (keys.Num() > 0)
            {
                ret.RemoveAt(ret.Len() - 1);//remove the last index as it'll be a wrongly placed ,
            }
            ret.Append("}");
            return ret;
        }

        void setup(TSharedPtr<FJsonObject> json_in)
        {//the json_in will be a mirror of what we have here!
            //we just need to convert the json object into an attributeMap now!
            attribute_map = UIndexerSupport::jsonObjectParser(json_in);
        }
};