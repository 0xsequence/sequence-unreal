// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "JsonObjectConverter.h"
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

	/*
	* Allows for the converting of a Struct straigt into a nicely formatted json string!
	*/
	template < typename T > static FString structToString(T structVar)
	{
		FString ret;
		FJsonObjectConverter::UStructToJsonObjectString<T>(structVar, ret, 0, 0);
		return ret;
	}

	/*
	* Allows for the converting of a Struct straigt into a nicely formatted json string!
	* This also cleans up the string from json formatting in the event we just want an
	* easily testable string!
	*/
	template < typename T > static FString structToSimpleString(T structVar)
	{
		FString ret;
		FJsonObjectConverter::UStructToJsonObjectString<T>(structVar, ret, 0, 0);
		ret = simplifyString(ret);
		return ret;
	}

	static FString int64ListToSimpleString(TArray<int64> intData);

	static FString jsonObjListToString(TArray<TSharedPtr<FJsonObject>> jsonData);
	
	static FString jsonObjListToSimpleString(TArray<TSharedPtr<FJsonObject>> jsonData);

	static FString jsonToString(TSharedPtr<FJsonValue> jsonData);

	static FString jsonToSimpleString(TSharedPtr<FJsonValue> jsonData);

	static FString jsonToString(TSharedPtr<FJsonObject> jsonData);

	static FString jsonToSimpleString(TSharedPtr<FJsonObject> jsonData);
private:
	/*
	* Similar to simplifyString EXCEPT we also cleanup 
	* some special edge cases from json responses / parsing as well!
	*/
	static FString stringCleanup(FString string);
};
