// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SignedMessage.generated.h"

USTRUCT()
struct FSignedMessageSignature
{
	GENERATED_BODY()
	UPROPERTY()
	FString message = "";
	UPROPERTY()
	FString signature = "";
};

USTRUCT()
struct FSignedMessage
{
	GENERATED_BODY()

	UPROPERTY()
	FSignedMessageSignature data;
};

USTRUCT()
struct FSignedMessageResponseObj
{
	GENERATED_BODY()

	UPROPERTY()
	FSignedMessage response;
};