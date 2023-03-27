// Fill out your copyright notice in the Description page of Project Settings.


#include "Provider.h"

#include "HexUtility.h"
#include "HttpManager.h"
#include "JsonBuilder.h"
#include "JsonObjectConverter.h"
#include "RequestHandler.h"

FString TagToString(EBlockTag Tag)
{
	switch (Tag)
	{
	case Latest:
		return "latest";
	case Earliest:
		return "earliest";
	case Pending:
		return "pending";
	case Safe:
		return "safe";
	case Finalized:
		return "finalized";
	}

	return "";
}

FString Provider::GetBlockByNumberHelper(FString Number)
{
	const auto Content = FJsonBuilder()
		.AddString("jsonrpc", "2.0")
		->AddInt("id", 1)
		->AddString("method", "eth_getBlockByNumber")
		->AddArray("params")
			.AddValue(Number)
			->AddBool(true)
		->EndArray()
		->ToString();
	
	return NewObject<URequestHandler>()
		->PrepareRequest()
		->WithUrl(Url)
		->WithHeader("Content-type", "application/json")
		->WithVerb("POST")
		->WithContentAsString(Content)
		->Process()
		->GetContentAsString();
}

TSharedPtr<FJsonObject> Provider::Parse(FString JsonRaw)
{
	TSharedPtr<FJsonObject> JsonParsed;

	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonRaw);
	if (FJsonSerializer::Deserialize(JsonReader, JsonParsed))
	{
		return JsonParsed;
	}

	return nullptr;
}

Provider::Provider(FString Url) : Url(Url)
{
}

FString Provider::BlockByNumber(uint16 Id)
{
	return GetBlockByNumberHelper(FJsonBuilder::ConvertString(IntToHexString(Id).GetValue()));
}

FString Provider::BlockByNumber(EBlockTag Tag)
{
	return GetBlockByNumberHelper(FJsonBuilder::ConvertString(TagToString(Tag)));
}

TValueOrError<uint32, SequenceError> Provider::ChainId()
{
	const auto Content = FJsonBuilder()
		.AddString("jsonrpc", "2.0")
		->AddInt("id", 1)
		->AddString("method", "eth_chainId")
		->ToString();
	
	auto ResponseContent = NewObject<URequestHandler>()
		->PrepareRequest()
		->WithUrl(Url)
		->WithHeader("Content-type", "application/json")
		->WithVerb("POST")
		->WithContentAsString(Content)
		->Process()
		->GetContentAsString();

	auto JSON = Parse(ResponseContent);

	if(!JSON)
	{
		return MakeError(SequenceError(ErrRequestFail, "Couldn't parse " + ResponseContent));
	}

	auto Result = JSON->GetStringField("result");
	
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Result);

	auto Convert =  HexStringToInt(Result);

	if(!Convert.IsSet())
	{
		return MakeError(SequenceError(ErrRequestFail, "Couldn't convert \"" + Result + "\" to a number."));
	}
	
	return MakeValue(Convert.GetValue());
}
