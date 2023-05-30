#pragma once

#include "CoreMinimal.h"
#include "JsonObjectConverter.h"
#include "Kismet/KismetStringLibrary.h"
#include "Struct_1.h"
#include "Struct_0.generated.h"

USTRUCT(BlueprintType)
struct FStruct_0
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		TMap<FString, FStruct_1> map;

	FStruct_0() {};

	FStruct_0(FJsonObject json_in)
	{
		TSharedPtr<FJsonObject> data = json_in.GetObjectField("map");//Get the objectt we need
		TMap<FString, TSharedPtr<FJsonValue>> data_map = data.Get()->Values;//get it's map!
		TArray<FString> map_keys;
		data_map.GetKeys(map_keys);//get the keys of the map

		//setup our struct!
		TSharedPtr<FJsonObject> json_step;
		FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create("{ \"int_data\": [] }"), json_step);
		FStruct_1 w_in;

		for (auto i : map_keys)
		{//now for each map key we can add it in with our data!

			json_step.Get()->SetArrayField("int_data", data_map.Find(i)->Get()->AsArray());

			if (FJsonObjectConverter::JsonObjectToUStruct<FStruct_1>(json_step.ToSharedRef(), &w_in))
			{
				map.Add(TPair<FString, FStruct_1>(i, w_in));//add the newly formed structure with it's key!
			}
		}
	};


	/* This is for reference of how to create a nested array with json parsing in unreal
	FStruct_0(FJsonObject json_in)
	{//need to manually parse this so we get a proper json result!
		TArray<TSharedPtr<FJsonValue>> data = json_in.GetArrayField("list");
		TSharedPtr<FJsonObject> json_step;
		FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create("{ \"int_data\": [] }"), json_step);
		FStruct_1 w_in;
		for (auto i : data)
		{//the main list and give me each invidual list!
			TArray<TSharedPtr<FJsonValue>> lcl_i = i.Get()->AsArray();
			json_step.Get()->SetArrayField("int_data", lcl_i);

			//now convert this to a FStruct_1 then add it in!
			if (FJsonObjectConverter::JsonObjectToUStruct<FStruct_1>(json_step.ToSharedRef(), &w_in))
			{
				list.Add(w_in);//now add our custom structure in
			}
		}
	};
	*/
};