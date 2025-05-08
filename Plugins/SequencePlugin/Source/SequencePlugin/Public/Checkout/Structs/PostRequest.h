#pragma once

#include "Dom/JsonObject.h"
#include "PostRequest.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FPostRequest
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence - Checkout")
	FString EndPoint;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence - Checkout")
	FString Method;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence - Checkout")
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

	void Setup(const FJsonObject& JSON_In)
	{
		if (FString ReturnEndPointString; JSON_In.TryGetStringField(TEXT("endPoint"), ReturnEndPointString))
		{
			EndPoint = ReturnEndPointString;
		}

		if (FString ReturnMethodString; JSON_In.TryGetStringField(TEXT("method"), ReturnMethodString))
		{
			Method = ReturnMethodString;
		}
		
		if (FString ReturnBodyString; JSON_In.TryGetStringField(TEXT("body"), ReturnBodyString))
		{
			Body = ReturnBodyString;
		}
	}
};
