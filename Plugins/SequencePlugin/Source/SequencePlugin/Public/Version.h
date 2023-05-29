#pragma once

#include "CoreMinimal.h"
#include "Version.generated.h"

USTRUCT(BlueprintType)
struct FVersion
{
    GENERATED_USTRUCT_BODY()
public:
    //FVersion(FJsonObject in)
    //{
     //   this->webrpcVersion = in.GetStringField("webrpcVersion");
      //  this->schemaVersion = in.GetStringField("schemaVersion");
       // this->schemaHash = in.GetStringField("schemaHash");
        //this->appVersion = in.GetStringField("appVersion");
    //}

    UPROPERTY()
        FString webrpcVersion;
    UPROPERTY()
        FString schemaVersion;
    UPROPERTY()
        FString schemaHash;
    UPROPERTY()
        FString appVersion;
};