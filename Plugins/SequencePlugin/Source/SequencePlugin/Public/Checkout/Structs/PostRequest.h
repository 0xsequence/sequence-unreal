#pragma once

#include "PostRequest.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FPostRequest
{
	GENERATED_USTRUCT_BODY()

public:
	FString EndPoint;
	FString Method;
	FString Body;

	FPostRequest()
	{
		EndPoint = "";
		Method = "";
		Body = "";
	}

	FPostRequest(FString EndPoint, FString Method, FString Body)
	{
		this->EndPoint = EndPoint;
		this->Method = Method;
		this->Body = Body;
	}

	bool customGetter = true;
	FString GetArgs() const
	{
		FString ReturnArgs = "{";

		ReturnArgs += "\"endPoint\":\"" + EndPoint + "\",";
		ReturnArgs += "\"method\":\"" + Method + "\",";
		ReturnArgs += "\"body\":\"" + Body + "\"";

		ReturnArgs.Append("}"); // Close the JSON object
		
		return ReturnArgs;
	}

	void Setup(FJsonObject& JSON_In)
	{
		EndPoint = JSON_In.GetStringField(TEXT("endPoint"));
		Method = JSON_In.GetStringField(TEXT("method"));
		Body = JSON_In.GetStringField(TEXT("body"));
	}
};
