#pragma once

#include "CoreMinimal.h"
#include "TokenList.h"
#include "IndexerSupport.h"
#include "JsonObjectConverter.h"
#include "GetTokenSuppliesMapArgs.generated.h"

USTRUCT(BlueprintType)
struct FGetTokenSuppliesMapArgs
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        TMap<FString, FTokenList> tokenMap;
    UPROPERTY()
        bool includeMetaData;

	bool customGetter = true;

    FGetTokenSuppliesMapArgs() {};

    FGetTokenSuppliesMapArgs(FJsonObject json_in)
    {//need to manually parse this so we get a proper json result!
		TSharedPtr<FJsonObject> data = json_in.GetObjectField("tokenMap");//Get the object we need
		TMap<FString, TSharedPtr<FJsonValue>> data_map = data.Get()->Values;//get it's map!
		TArray<FString> map_keys;
		data_map.GetKeys(map_keys);//get the keys of the map

		//setup our struct!
		TSharedPtr<FJsonObject> json_step;
		FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create("{ \"token_list\": [] }"), json_step);
		FTokenList w_in;

		for (auto i : map_keys)
		{//now for each map key we can add it in with our data!

			json_step.Get()->SetArrayField("token_list", data_map.Find(i)->Get()->AsArray());

			if (FJsonObjectConverter::JsonObjectToUStruct<FTokenList>(json_step.ToSharedRef(), &w_in))
			{
				tokenMap.Add(TPair<FString, FTokenList>(i, w_in));//add the newly formed structure with it's key!
			}
		}
    };

	//this function name may need to be renamed
	FString Get()
	{
		FString ret = "{\"tokenMap\":{";
		TArray<FString> keys;
		tokenMap.GetKeys(keys);

		for (FString key : keys)
		{
			ret += "\"" + key + "\":" + UIndexerSupport::stringListToParsableString(tokenMap.Find(key)->token_list);
		}
		ret += "},";//close off tokenMap subObject
		ret += "\"includeMetaData\":";
		ret += includeMetaData ? "true" : "false";
		ret += "}";
		return ret;
	};
};