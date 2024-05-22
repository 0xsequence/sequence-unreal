// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once
#include "CoreMinimal.h"
#include "Util/Async.h"
#include "Errors.h"
#include "Dom/JsonObject.h"
#include "Templates/SharedPointer.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "RPCCaller.generated.h"

template<typename T> using Extractor = TFunction<TResult<T> (FString)>;

class FJsonBuilder;

UCLASS()
class SEQUENCEPLUGIN_API URPCCaller : public UObject
{
	GENERATED_BODY()
public:
	static TSharedPtr<FJsonObject> Parse(const FString& JsonRaw);
	static TResult<TSharedPtr<FJsonObject>> ExtractJsonObjectResult(const FString& JsonRaw);
	static TResult<FString> ExtractStringResult(const FString& JsonRaw);
	static TResult<uint64> ExtractUIntResult(const FString& JsonRaw);
	virtual void SendRPC(const FString& Url, const FString& Content, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure);

	template<typename T>
	void SendRPCAndExtract(const FString& Url, const FString& Content, const TSuccessCallback<T>& OnSuccess, const TFunction<TResult<T> (FString)>& Extractor, const FFailureCallback& OnFailure)
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
	
	static FJsonBuilder RPCBuilder(const FString& MethodName);
};