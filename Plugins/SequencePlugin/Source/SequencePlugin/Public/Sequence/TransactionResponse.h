// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Dom/JsonObject.h"
#include "TransactionResponse.generated.h"

USTRUCT()
struct FTransactionResponse
{
	GENERATED_BODY()

	UPROPERTY()
	FString TransactionHash = "";
	TSharedPtr<FJsonObject> Json;

	FTransactionResponse(){}
	
	FTransactionResponse(const FString& TxnHashIn, const TSharedPtr<FJsonObject>& JsonIn)
	{
		TransactionHash = TxnHashIn;
		Json = JsonIn;
	}
};