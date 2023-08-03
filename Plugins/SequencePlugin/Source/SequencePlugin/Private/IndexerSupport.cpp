// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "IndexerSupport.h"
#include "Version.h"

/*
* This will convert a jsonObject into a TMap<FString,FString> thereby making a dynamic
* object usable in the UI!
*/
TMap<FString, FString> UIndexerSupport::jsonObjectParser(TSharedPtr<FJsonObject> jsonData)
{
	TArray<TPair<FString, TSharedPtr<FJsonValue>>> jsonList = jsonData.Get()->Values.Array();
	TMap<FString, FString> ret;
	ret.Reserve(jsonList.Num());//put some slack in to speed this up!

	for (TPair<FString,TSharedPtr<FJsonValue>> kvp : jsonList)
	{
		TPair<FString, FString> tPair(kvp.Key, stringCleanup(jsonToString(kvp.Value)));
		ret.Add(tPair);
	}
	return ret;
}

FString UIndexerSupport::simplifyString(FString string)
{
	FString* ret = &string;

	FString srch_n = TEXT("\n");//we want no returns the UI will take of this for us!
	FString srch_r = TEXT("\r");
	FString srch_t = TEXT("\t");
	FString srch_c = TEXT("\"");//this will be apart of json formatting get rid of it!
	FString replace = TEXT("");

	const TCHAR* srch_ptr_n = *srch_n;
	const TCHAR* srch_ptr_r = *srch_r;
	const TCHAR* srch_ptr_t = *srch_t;
	const TCHAR* srch_ptr_c = *srch_c;
	const TCHAR* rep_ptr = *replace;

	(*ret).ReplaceInline(srch_ptr_n, rep_ptr, ESearchCase::IgnoreCase);//remove \n
	(*ret).ReplaceInline(srch_ptr_r, rep_ptr, ESearchCase::IgnoreCase);//remove \r
	(*ret).ReplaceInline(srch_ptr_t, rep_ptr, ESearchCase::IgnoreCase);//remove \t
	(*ret).ReplaceInline(srch_ptr_c, rep_ptr, ESearchCase::IgnoreCase);//remove \"

	return (*ret);
}

/*
* This will convert a json value to an FString!
*/
FString UIndexerSupport::jsonToString(TSharedPtr<FJsonValue> jsonData)
{
	FString ret;
	TSharedRef< TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ret);
	FJsonSerializer::Serialize(jsonData.ToSharedRef(),"", Writer);
	return ret;
}

FString UIndexerSupport::stringCleanup(FString string)
{
	FString *ret = &string;

	//For some reason FJsonValues when they get parsed will have leading commas!
	if (string[0] == ',')
	{
		string.RemoveAt(0);//remove the initial character if it's a treasonous comma!
	}

	FString srch_n = TEXT("\n");//we want no returns the UI will take of this for us!
	FString srch_r = TEXT("\r");
	FString srch_t = TEXT("\t");
	FString srch_c = TEXT("\"");//this will be apart of json formatting get rid of it!
	FString replace = TEXT("");

	const TCHAR* srch_ptr_n = *srch_n;
	const TCHAR* srch_ptr_r = *srch_r;
	const TCHAR* srch_ptr_t = *srch_t;
	const TCHAR* srch_ptr_c = *srch_c;
	const TCHAR* rep_ptr = *replace;

	(*ret).ReplaceInline(srch_ptr_n, rep_ptr, ESearchCase::IgnoreCase);//remove \n
	(*ret).ReplaceInline(srch_ptr_r, rep_ptr, ESearchCase::IgnoreCase);//remove \r
	(*ret).ReplaceInline(srch_ptr_t, rep_ptr, ESearchCase::IgnoreCase);//remove \t
	(*ret).ReplaceInline(srch_ptr_c, rep_ptr, ESearchCase::IgnoreCase);//remove \"

	return (*ret);
}