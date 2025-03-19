#pragma once

#include "CoreMinimal.h"
#include "SardineRegion.h"
#include "SardineGetSupportedRegionsReturn.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSardineGetSupportedRegionsReturn
{
	GENERATED_USTRUCT_BODY()

	TArray<FSardineRegion> Regions;

	bool customConstructor = false; // Flag to determine the constructor used

	// Dummy constructor method to maintain consistency
	void construct(FJsonObject json_in) {}

	/*
	* Handles edge cases with Unreal's JSON parsing
	*/
	void Setup(FJsonObject json_in)
	{
		const TArray<TSharedPtr<FJsonValue>>* lst;
		if (json_in.TryGetArrayField(TEXT("regions"), lst))
		{
			Regions.SetNum(lst->Num());

			for (int32 i = 0; i < lst->Num(); i++)
			{
				TSharedPtr<FJsonValue> item = (*lst)[i];

				if (item.IsValid())
				{
					TSharedPtr<FJsonObject> itemObj = item->AsObject();
					Regions[i].Setup(*itemObj);
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No  regions  field found in the SardineGetSupportedRegions response."));
		}
	}
};
