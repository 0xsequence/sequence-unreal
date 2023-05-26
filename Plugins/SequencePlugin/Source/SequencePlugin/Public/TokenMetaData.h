#pragma once

#include "CoreMinimal.h"
#include "TokenMetaData.generated.h"

USTRUCT(BlueprintType)
struct FTokenMetaData
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        int64 tokenId;
    UPROPERTY()
        FString contractAddress;
    UPROPERTY()
        FString name;
    UPROPERTY()
        FString description;
    UPROPERTY()
        FString image;
    UPROPERTY()
        float decimals;
 //   UPROPERTY()
   //     TMap<FString, *UObject> properties;
    UPROPERTY()
        FString video;
    UPROPERTY()
        FString audio;
    UPROPERTY()
        FString image_data;
    UPROPERTY()
        FString external_url;
    UPROPERTY()
        FString background_color;
    UPROPERTY()
        FString animation_url;
   // UPROPERTY()
      //  TArray<TMap<FString, UObject>> attributes;
};