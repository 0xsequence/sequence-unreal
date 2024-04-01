// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
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
		TArray<FStruct_1> list;

	FStruct_0() {};
	/*
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
	*/

	/*
	//This is for reference of how to create a nested array with json parsing in unreal
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

	//This is a test of <TArray<TMap<FString,data>>
	FStruct_0(FJsonObject json_in)
	{//need to manually parse this so we get a proper json result!
		TArray<TSharedPtr<FJsonValue>> data = json_in.GetArrayField("list");
		TSharedPtr<FJsonObject> json_step;
		FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create("{ \"int_data\": {} }"), json_step);
		FStruct_1 w_in;
		for (auto i : data)
		{//the main list get me each individual map to assign
			TSharedPtr<FJsonObject> lcl_i = i.Get()->AsObject();
			json_step.Get()->SetObjectField("int_data", lcl_i);

			//now convert this to a FStruct_1 then add it in!
			if (FJsonObjectConverter::JsonObjectToUStruct<FStruct_1>(json_step.ToSharedRef(), &w_in))
			{
				list.Add(w_in);//now add our custom structure in
			}
		}
	};

	//This operates independent of the nested data inside and should be generified and used in the other
	//custom getters from now as I feel this is more reliable and simplier to understand.
	FString Get()
	{
		FString out = "{\"list\":[";

		for (auto i : list)
		{//Here we append to the returning json object with our own data.
			FString json_obj_str;
			FJsonObjectConverter::UStructToJsonObjectString(i, json_obj_str);

			json_obj_str.RemoveSpacesInline();
			FString srch_prefix = TEXT("{\"int_data\":");
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
			//{\"int_data\": [some data] }
			(json_obj_str).ReplaceInline(srch_ptr, rep_ptr, ESearchCase::IgnoreCase);//remove \t
			json_obj_str.LeftChopInline(1);//remove the tailing }
			out.Append(json_obj_str);
			out.Append(",");
		}
		out.LeftChopInline(1);//easy way of removing the trailing "," without if's
		out.Append("]}");

		return out;
	};

	/*
	* Getter for TMap with a value entry of TArray!
	FString Get()
	{
		TSharedPtr<FJsonObject> json;
		FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create("{ \"map\": {} }"), json);
		//we need to create key value pairs to insert into json out!
		TArray<FString> keys;
		map.GetKeys(keys);

		for (auto key : keys)//go through the keys!
		{
			TSharedPtr<FJsonObject> j_out = FJsonObjectConverter::UStructToJsonObject<FStruct_1>(*map.Find(key));
			json.Get()->GetObjectField("map").Get()->SetArrayField(key, j_out.Get()->GetArrayField("int_data"));
		}

		//lastly convert from a json object to a FString
		FString out;
		TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&out);
		FJsonSerializer::Serialize(json.ToSharedRef(), Writer);

		return out;
	};
	*/
};