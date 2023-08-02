// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TokenMetaData.h"
#include "UObject/NoExportTypes.h"
#include "IndexerSupport.generated.h"

/**
 * Support class for the indexer
 */
UCLASS()
class UIndexerSupport : public UObject
{
	GENERATED_BODY()
public:
	/*
	* Used to convert a jsonObject into a hash map of FStrings
	*/
	static TMap<FString, FString> jsonObjectParser(TSharedPtr<FJsonObject> jsonData);

	static FString jsonToString(TSharedPtr<FJsonValue> jsonData);

	static FString stringCleanup(FString string);
};
