#pragma once
#include "Util/Async.h"
#include "Errors.h"
#include "Dom/JsonObject.h"
#include "Util/JsonBuilder.h"

class RPCCaller
{
public:
	virtual ~RPCCaller() = default;
	TSharedPtr<FJsonObject> Parse(FString JsonRaw);
	TResult<TSharedPtr<FJsonObject>> ExtractJsonObjectResult(FString JsonRaw);
	TResult<FString> ExtractStringResult(FString JsonRaw);
	TResult<uint64> ExtractUIntResult(FString JsonRaw);
	virtual void SendRPC(FString Url, FString Content, TSuccessCallback<FString> OnSuccess, FFailureCallback OnFailure);

	template<typename T>
	void SendRPCAndExtract(FString Url, FString Content, TSuccessCallback<T> OnSuccess, TFunction<TResult<T> (FString)> Extractor, FFailureCallback OnFailure);
	
	static FJsonBuilder RPCBuilder(FString MethodName);
};


template<typename T> using Extractor = TFunction<TResult<T> (FString)>;

template <typename T>
void RPCCaller::SendRPCAndExtract(FString Url, FString Content, TSuccessCallback<T> OnSuccess, Extractor<T> Extractor, FFailureCallback OnFailure)
{
	SendRPC(Url, Content, [OnSuccess, Extractor](FString Result)
	{
		TResult<T> Value = Extractor(Result);

		if(Value.HasValue())
		{
			OnSuccess(Value.GetValue());
		}
	}, OnFailure);
}


