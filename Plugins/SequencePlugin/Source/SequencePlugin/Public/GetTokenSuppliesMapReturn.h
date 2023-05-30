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
   UPROPERTY()
      TMap<FString, FTokenSupplyList> supplies;

   FGetTokenSuppliesMapReturn() {};

   FGetTokenSuppliesMapReturn(FJsonObject json_in) 
   {//need to manually parse this so we get a proper json result!
	  TSharedPtr<FJsonObject> data = json_in.GetObjectField("supplies");//Get the object we need
	  TMap<FString, TSharedPtr<FJsonValue>> data_map = data.Get()->Values;//get it's map!
	  TArray<FString> map_keys;
	  data_map.GetKeys(map_keys);//get the keys of the map

	  //setup our struct!
	  TSharedPtr<FJsonObject> json_step;
	  FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create("{ \"token_supply_list\": [] }"), json_step);
	  FTokenSupplyList w_in;

	  for (auto i : map_keys)
	  {//now for each map key we can add it in with our data!

		  json_step.Get()->SetArrayField("token_supply_list", data_map.Find(i)->Get()->AsArray());

		  if (FJsonObjectConverter::JsonObjectToUStruct<FTokenSupplyList>(json_step.ToSharedRef(), &w_in))
		  {
			  supplies.Add(TPair<FString, FTokenSupplyList>(i, w_in));//add the newly formed structure with it's key!
		  }
	  }
   };

};