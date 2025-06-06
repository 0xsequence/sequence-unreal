#include "RPCCaller.h"
#include "Util/HexUtility.h"
#include "RequestHandler.h"
#include "Templates/SharedPointer.h"
#include "Serialization/JsonReader.h"
#include "Util/JsonBuilder.h"
#include "Util/Log.h"

URPCCaller::URPCCaller()
{
	Validator = NewObject<UResponseSignatureValidator>();
}

TSharedPtr<FJsonObject> URPCCaller::Parse(const FString& JsonRaw)
{
	TSharedPtr<FJsonObject> JsonParsed;
	const TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonRaw);
	if (FJsonSerializer::Deserialize(JsonReader, JsonParsed))
	{
		return JsonParsed;
	}
	return nullptr;
}

TResult<TSharedPtr<FJsonObject>> URPCCaller::ExtractJsonObjectResult(const FString& JsonRaw)
{
	const TSharedPtr<FJsonObject> Json = Parse(JsonRaw);
	
	if(!Json)
	{
		return MakeError(FSequenceError(EmptyResponse, "Could not extract response"));
	}

	return MakeValue(Json->GetObjectField(TEXT("result")));
}

TResult<FString> URPCCaller::ExtractStringResult(const FString& JsonRaw)
{
	SEQ_LOG(Display, TEXT("%s"), *JsonRaw);
	const TSharedPtr<FJsonObject> Json = Parse(JsonRaw);
	
	if(!Json)
	{
		return MakeError(FSequenceError(EmptyResponse, "Could not extract response"));
	}

	return MakeValue(Json->GetStringField(TEXT("result")));
}

TResult<uint64> URPCCaller::ExtractUIntResult(const FString& JsonRaw)
{
	TResult<FString> Result = ExtractStringResult(JsonRaw);
	if(!Result.HasValue())
	{
		return MakeError(Result.GetError());
	}

	TOptional<uint64> Convert = HexStringToUint64(Result.GetValue());
	if(!Convert.IsSet())
	{
		return MakeError(FSequenceError(ResponseParseError, "Couldn't convert \"" + Result.GetValue() + "\" to a number."));
	}
	return MakeValue(Convert.GetValue());
}

void URPCCaller::SendRPC(const FString& Url, const FString& Content, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnError, bool bUseValidator)
{
	URequestHandler* RequestHandler = NewObject<URequestHandler>();

	SEQ_LOG_EDITOR(Display, TEXT("%s - %s"), *Url, *Content);

	RequestHandler->PrepareRequest()
		->WithUrl(Url)
		->WithHeader("Content-type", "application/json")
		->WithHeader("Accept", "application/json")
		->WithVerb("POST")
		->WithContentAsString(Content)
		->ProcessAndThen(*Validator, OnSuccess, OnError, bUseValidator);
}

FJsonBuilder URPCCaller::RPCBuilder(const FString& MethodName)
{
	return *FJsonBuilder().ToPtr()
		->AddString("jsonrpc", "2.0")
		->AddInt("id", 1)
		->AddString("method", MethodName);
}