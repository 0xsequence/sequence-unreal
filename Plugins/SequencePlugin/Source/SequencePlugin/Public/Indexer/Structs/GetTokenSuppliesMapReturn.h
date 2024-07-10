// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "TokenSupply.h"
#include "TokenSupplyList.h"
#include "JsonObjectConverter.h"
#include "GetTokenSuppliesMapReturn.generated.h"

USTRUCT(BlueprintType)
struct FGetTokenSuppliesMapReturn
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<FString, FTokenSupplyList> supplies;
	bool customConstructor = true;//used to tell buildresponse whether or not to use a custom constructor OR the unreal one!

	/*
	* Used to return the json Object formed by this struct
	* used for args & testing purposes
	*/
	TSharedPtr<FJsonObject> GetJson()
	{
		TSharedPtr<FJsonObject> ret = MakeShareable<FJsonObject>(new FJsonObject);
		TSharedPtr<FJsonObject> nRet = MakeShareable<FJsonObject>(new FJsonObject);
		TArray<FString> keys;
		supplies.GetKeys(keys);

		for (FString key : keys)
		{
			FString value = supplies.Find(key)->GetJsonString();
			nRet.Get()->SetStringField(key,value);
		}
		ret.Get()->SetObjectField("supplies", nRet);
		return ret;
	}

	/*
	* Used to Handle Edge Cases with Unreal's Json parsing
	*/
	void Setup(FJsonObject json_in)
	{

		const TSharedPtr<FJsonObject>* itemObj;
		if (json_in.TryGetObjectField(TEXT("supplies"), itemObj))
		{
			TArray<FString> des_keys;
			supplies.GetKeys(des_keys);
			TMap<FString, TSharedPtr<FJsonValue>> src_map = (itemObj->Get())->Values;
			TArray<FString> src_keys;
			src_map.GetKeys(src_keys);
			for (auto i : des_keys)
			{
				for (auto j : src_keys)
				{
					if (i.ToLower().Compare(j.ToLower()) == 0)
					{
						const TArray<TSharedPtr<FJsonValue>>* lst;
						if (src_map.Find(i)->Get()->TryGetArray(lst))
						{
							supplies.Find(i)->setup(*lst);
						}
					}//if
				}//for j
			}//for i
		}//if valid
	}//setup

	void construct(FJsonObject json_in)
	{//need to manually parse this so we get a proper json result!
		const TSharedPtr<FJsonObject>* data;
		if (json_in.TryGetObjectField(TEXT("supplies"), data))//safely get
		{
			TMap<FString, TSharedPtr<FJsonValue>> data_map = data->Get()->Values;//get it's map!
			TArray<FString> map_keys;
			data_map.GetKeys(map_keys);//get the keys of the map

			//setup our struct!
			TSharedPtr<FJsonObject> json_step;
			FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create("{ \"token_supply_list\": [] }"), json_step);
			FTokenSupplyList w_in;

			for (auto i : map_keys)
			{//now for each map key we can add it in with our data!
				const TArray<TSharedPtr<FJsonValue>>* lst;
				if (data_map.Find(i)->Get()->TryGetArray(lst))
				{
					json_step.Get()->SetArrayField("token_supply_list", *lst);
					if (FJsonObjectConverter::JsonObjectToUStruct<FTokenSupplyList>(json_step.ToSharedRef(), &w_in))
					{
						supplies.Add(TPair<FString, FTokenSupplyList>(i, w_in));//add the newly formed structure with it's key!
					}
				}
			}
		}
	};
};