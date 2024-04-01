// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "WebRPCError.generated.h"

USTRUCT(BlueprintType)
struct FWebRPCError
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        FString code = "";
    UPROPERTY()
        FString msg = "";
    UPROPERTY()
        int32 status = -1;
};