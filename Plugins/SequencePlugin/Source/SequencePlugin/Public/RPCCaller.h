// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once
#include "CoreMinimal.h"
#include "Util/Async.h"
#include "Errors.h"
#include "Dom/JsonObject.h"
#include "Util/JsonBuilder.h"
#include "Templates/SharedPointer.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "RPCCaller.generated.h"

template<typename T> using Extractor = TFunction<TResult<T> (FString)>;

UCLASS()
class SEQUENCEPLUGIN_API URPCCaller : public UObject
{
	GENERATED_BODY()
public:
	TSharedPtr<FJsonObject> Parse(FString JsonRaw);
	TResult<TSharedPtr<FJsonObject>> ExtractJsonObjectResult(FString JsonRaw);
	TResult<FString> ExtractStringResult(FString JsonRaw);
	TResult<uint64> ExtractUIntResult(FString JsonRaw);
	virtual void SendRPC(FString Url, FString Content, TSuccessCallback<FString> OnSuccess, FFailureCallback OnFailure);

	template<typename T>
	void SendRPCAndExtract(FString Url, FString Content, TSuccessCallback<T> OnSuccess, TFunction<TResult<T> (FString)> Extractor, FFailureCallback OnFailure)
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
	
	static FJsonBuilder RPCBuilder(FString MethodName);
};