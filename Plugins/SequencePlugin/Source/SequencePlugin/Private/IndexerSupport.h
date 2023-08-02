// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

	/*
	* Used to remove
	* \n, \", \t, \r, spaces etc
	*/
	static FString simplifyString(FString string);

private:
	static FString jsonToString(TSharedPtr<FJsonValue> jsonData);

	/*
	* Similar to simplifyString EXCEPT we also cleanup 
	* some special edge cases from json responses / parsing as well!
	*/
	static FString stringCleanup(FString string);
};
