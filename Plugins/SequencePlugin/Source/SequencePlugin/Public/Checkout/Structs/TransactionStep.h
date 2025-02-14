#pragma once
#include "PostRequest.h"
#include "TransactionSignature.h"
#include "Checkout/Enums/StepType.h"
#include "Util/SequenceSupport.h"

#include "TransactionStep.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FTransactionStep
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EStepType Id;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Data;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString To;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Value;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FTransactionSignature Signature;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FPostRequest Post;

	FTransactionStep()
	{
		this->Id = EStepType::Unknown;
		this->Data = "";
		this->To = "";
		this->Value = "";
		this->Signature = FTransactionSignature();
		this->Post = FPostRequest();
	}

	FTransactionStep(const EStepType Id, FString Data, FString To, FString Value, const FTransactionSignature& Signature, const FPostRequest& Post)
	{
		this->Id = Id;
		this->Data = Data;
		this->To = To;
		this->Value = Value;
		this->Signature = Signature;
		this->Post = Post;
	}
	
	bool customGetter = true;
	FString GetArgs() const
	{
		FString ReturnArgs = "{";

		ReturnArgs += "\"id\":\"" + UEnum::GetValueAsString(Id) + "\",";
		ReturnArgs += "\"data\":\"" + Data + "\",";
		ReturnArgs += "\"to\":\"" + To + "\",";
		ReturnArgs += "\"value\":\"" + Value + "\",";
		ReturnArgs += "\"signature\":\"" + Signature.GetArgs() + "\",";
		ReturnArgs += "\"post\":\"" + Post.GetArgs() + "\"";

		ReturnArgs.Append("}"); // Close the JSON object
		
		return ReturnArgs;
	}

	void Setup(FJsonObject& JSON_In)
	{
		if (JSON_In.TryGetField(TEXT("id")))
		{
			Id = static_cast<EStepType>(JSON_In.GetIntegerField(TEXT("id")));
		}

		Data = JSON_In.GetStringField(TEXT("data"));
		To = JSON_In.GetStringField(TEXT("to"));
		Value = JSON_In.GetStringField(TEXT("value"));
		// Setup Signature
		if (JSON_In.TryGetField(TEXT("signature")))
		{
			const TSharedPtr<FJsonValue> SignatureValue = JSON_In.GetField(TEXT("signature"), EJson::Object);
			TSharedPtr<FJsonObject> SignatureJsonObject = USequenceSupport::JsonStringToObject(SignatureValue->AsString());
			Signature.Setup(*SignatureJsonObject);
		}
		// Setup Post Request
		if (JSON_In.TryGetField(TEXT("post")))
		{
			const TSharedPtr<FJsonValue> PostRequestValue = JSON_In.GetField(TEXT("post"), EJson::Object);
			const TSharedPtr<FJsonObject> PostRequestJsonObject = USequenceSupport::JsonStringToObject(PostRequestValue->AsString());
			Post.Setup(*PostRequestJsonObject);
		}
	}
};
