// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "SeqTokenList.h"
#include "Util/SequenceSupport.h"
#include "JsonObjectConverter.h"
#include "SeqGetTokenSuppliesMapArgs.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqGetTokenSuppliesMapArgs
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        TMap<FString, FSeqTokenList> tokenMap;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        bool includeMetaData = false;

	bool customGetter = true;

    FSeqGetTokenSuppliesMapArgs() {};

    FSeqGetTokenSuppliesMapArgs(FJsonObject json_in)
    {//need to manually parse this so we get a proper json result!
		TSharedPtr<FJsonObject> data = json_in.GetObjectField(TEXT("tokenMap"));//Get the object we need
		TMap<FString, TSharedPtr<FJsonValue>> data_map = data.Get()->Values;//get it's map!
		TArray<FString> map_keys;
		data_map.GetKeys(map_keys);//get the keys of the map

		//setup our struct!
		TSharedPtr<FJsonObject> json_step;
		FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create("{ \"token_list\": [] }"), json_step);
		FSeqTokenList w_in;

		for (auto i : map_keys)
		{//now for each map key we can add it in with our data!

			json_step.Get()->SetArrayField("token_list", data_map.Find(i)->Get()->AsArray());

			if (FJsonObjectConverter::JsonObjectToUStruct<FSeqTokenList>(json_step.ToSharedRef(), &w_in))
			{
				tokenMap.Add(TPair<FString, FSeqTokenList>(i, w_in));//add the newly formed structure with it's key!
			}
		}
    };

	/*
	* Used to get the Json Object String formed by this struct
	* used for args and testing
	*/
	FString GetArgs()
	{
		FString ret = "{\"tokenMap\":{";
		TArray<FString> keys;
		tokenMap.GetKeys(keys);

		for (FString key : keys)
		{
			ret += "\"" + key + "\":" + USequenceSupport::StringListToParsableString(tokenMap.Find(key)->token_list) + ",";
		}
		ret = ret.LeftChop(1); //remove the last comma
		ret += "},";//close off tokenMap subObject
		ret += "\"includeMetaData\":";
		ret += includeMetaData ? "true" : "false";
		ret += "}";
		return ret;
	};
};