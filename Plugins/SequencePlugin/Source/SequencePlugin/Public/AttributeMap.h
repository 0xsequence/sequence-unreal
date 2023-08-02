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
        
        void setup(TSharedPtr<FJsonObject> json_in)
        {//the json_in will be a mirror of what we have here!
            //we just need to convert the json object into an attributeMap now!
            attribute_map = UIndexerSupport::jsonObjectParser(json_in);
        }
};
