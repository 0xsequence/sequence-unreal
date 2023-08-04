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

	(*ret).RemoveSpacesInline();//remove spaces
	(*ret).ReplaceInline(srch_ptr_n, rep_ptr, ESearchCase::IgnoreCase);//remove \n
	(*ret).ReplaceInline(srch_ptr_r, rep_ptr, ESearchCase::IgnoreCase);//remove \r
	(*ret).ReplaceInline(srch_ptr_t, rep_ptr, ESearchCase::IgnoreCase);//remove \t
	(*ret).ReplaceInline(srch_ptr_c, rep_ptr, ESearchCase::IgnoreCase);//remove \"

	return (*ret);
}

FString UIndexerSupport::stringListToSimpleString(TArray<FString> stringData)
{
	FString ret = "[";

	for (FString string : stringData)
	{
		ret += string;
		ret += ",";
	}

	if (stringData.Num() > 0)
	{
		ret.RemoveAt(ret.Len() - 1);//remove the last comma as it'll be wrong!
	}
	ret += "]";
	return ret;
}

FString UIndexerSupport::int64ListToSimpleString(TArray<int64> intData)
{
	FString ret = "[";

	for (int64 iData : intData)
	{
		FString iDataString = FString::Printf(TEXT("%lld"), iData);
		ret += iDataString + ",";
	}

	if (intData.Num() > 0)
	{
		ret.RemoveAt(ret.Len() - 1);//remove the last comma as it'll be wrong!
	}

	ret += "]";
	return ret;
}

FString UIndexerSupport::jsonObjListToString(TArray<TSharedPtr<FJsonObject>> jsonData)
{
	FString ret = "[";
	for (TSharedPtr<FJsonObject> jObj : jsonData)
	{
		ret.Append(UIndexerSupport::jsonToString(jObj));
		ret.Append(",");
	}

	if (jsonData.Num() > 0)
	{
		ret.RemoveAt(ret.Len() - 1);
	}
	ret.Append("]");
	return ret;
}

FString UIndexerSupport::jsonObjListToSimpleString(TArray<TSharedPtr<FJsonObject>> jsonData)
{
	FString ret = "[";
	for (TSharedPtr<FJsonObject> jObj : jsonData)
	{
		ret.Append(UIndexerSupport::jsonToSimpleString(jObj));
		ret.Append(",");
	}

	if (jsonData.Num() > 0)
	{
		ret.RemoveAt(ret.Len() - 1);
	}
	ret.Append("]");
	return ret;
}

FString UIndexerSupport::jsonToString(TSharedPtr<FJsonObject> jsonData)
{
	FString ret;
	TSharedRef< TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ret);
	FJsonSerializer::Serialize(jsonData.ToSharedRef(), Writer);
	return ret;
}

FString UIndexerSupport::jsonToSimpleString(TSharedPtr<FJsonValue> jsonData)
{
	return simplifyString(jsonToString(jsonData));
}

FString UIndexerSupport::jsonToSimpleString(TSharedPtr<FJsonObject> jsonData)
{
	return simplifyString(jsonToString(jsonData));
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