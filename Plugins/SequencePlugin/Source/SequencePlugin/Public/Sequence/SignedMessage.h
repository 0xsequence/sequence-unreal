// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SignedMessage.generated.h"

USTRUCT(Blueprintable)
struct SEQUENCEPLUGIN_API FSignedMessageSignature
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	FString message = "";
	UPROPERTY(BlueprintReadWrite)
	FString signature = "";
};

USTRUCT(Blueprintable)
struct SEQUENCEPLUGIN_API FSignedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FSignedMessageSignature data;
};

USTRUCT()
struct SEQUENCEPLUGIN_API FSignedMessageResponseObj
{
	GENERATED_BODY()

	UPROPERTY()
	FSignedMessage response;
};