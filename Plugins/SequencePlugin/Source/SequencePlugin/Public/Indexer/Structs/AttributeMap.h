// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Indexer/IndexerSupport.h"
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
        TMap<FString, FString> AttributeMap;
        
        /*
        * Gets the json object string formed by this struct
        * Note: because this struct only exists due to UE's lack of nested data structure
        */
        FString GetJsonString()
        {
            FString Ret = "{";
            
            TArray<FString> Keys;
            AttributeMap.GetKeys(Keys);

            for (FString Key : Keys)
            {
                Ret.Append("\"");
                Ret.Append(Key);
                Ret.Append("\"");
                Ret.Append(":");
                Ret.Append(*AttributeMap.Find(Key));
                Ret.Append(",");
            }
            if (Keys.Num() > 0)
            {
                Ret.RemoveAt(Ret.Len() - 1);//remove the last index as it'll be a wrongly placed ,
            }
            Ret.Append("}");
            return Ret;
        }

        void setup(TSharedPtr<FJsonObject> JsonIn)
        {//the json_in will be a mirror of what we have here!
            //we just need to convert the json object into an attributeMap now!
            AttributeMap = UIndexerSupport::JSONObjectParser(JsonIn);
        }
};