// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "SeqEtherBalance.h"
#include "SeqGetEtherBalanceReturn.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqGetEtherBalanceReturn
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        FSeqEtherBalance balance;
    bool customConstructor = false;//used to tell buildresponse whether or not to use a custom constructor OR the unreal one!
    void construct(FJsonObject json_in) {};//dummy construct for templating

    /*
    * Used to Handle Edge Cases with Unreal's Json parsing
    */
    void Setup(FJsonObject json_in) {};//dummy method to allow for templating in build response!
};