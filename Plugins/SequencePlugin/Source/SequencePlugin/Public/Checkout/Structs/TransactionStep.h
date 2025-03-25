#pragma once
#include "PostRequest.h"
#include "TransactionSignature.h"
#include "Checkout/Enums/StepType.h"
#include "Sequence/SequenceAPI.h"
#include "Sequence/Transactions.h"
#include "Util/SequenceSupport.h"

#include "TransactionStep.generated.h"

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetSequenceTransactionResponseSuccess, FSeqTransactionResponse_Data, Response);
DECLARE_DYNAMIC_DELEGATE(FOnGetSeqTransactionFailure);

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
		ReturnArgs += "\"signature\":" + Signature.GetArgs() + ",";
		ReturnArgs += "\"post\":" + Post.GetArgs();

		ReturnArgs.Append("}"); // Close the JSON object
		
		return ReturnArgs;
	}

	void Setup(const FJsonObject& JSON_In)
	{
		if (JSON_In.TryGetField(TEXT("id")))
		{
			Id = UStepType::GetStepTypeFromString(JSON_In.GetStringField(TEXT("id")));
		}

		Data = JSON_In.GetStringField(TEXT("data"));
		To = JSON_In.GetStringField(TEXT("to"));
		Value = JSON_In.GetStringField(TEXT("value"));
		// Setup Signature
		if (JSON_In.HasField(TEXT("signature")))
		{
			const TSharedPtr<FJsonValue> SignatureValue = JSON_In.TryGetField(TEXT("signature"));
			if (SignatureValue != nullptr)
			{
				TSharedPtr<FJsonObject> SignatureJsonObject = SignatureValue->AsObject();
				Signature.Setup(*SignatureJsonObject);
			}
		}
		// Setup Post Request
		if (JSON_In.HasField(TEXT("post")))
		{
			const TSharedPtr<FJsonValue> PostRequestValue = JSON_In.TryGetField(TEXT("post"));
			
			if (PostRequestValue != nullptr)
			{
				TSharedPtr<FJsonObject> PostRequestJsonObject = PostRequestValue->AsObject();
				Post.Setup(*PostRequestJsonObject);
			}
		}
	}
	
	static TArray<TransactionUnion> AsTransactionArray(const TArray<FTransactionStep>& Steps)
	{
		if (Steps.IsEmpty())
		{
			UE_LOG(LogTemp, Error, TEXT("Steps cannot be null or empty"));
		}

		TArray<TransactionUnion> Transactions;
		Transactions.Reserve(Steps.Num());
		for (int i = 0; i < Steps.Num(); i++)
		{
			Transactions[i] = TUnion<FRawTransaction, FERC20Transaction, FERC721Transaction, FERC1155Transaction, FDelayedTransaction>(
				FRawTransaction(Steps[i].To, Steps[i].Value, Steps[i].Data));
		}
		
		return Transactions;
	}
	
	static FTransactionStep ExtractBuyStep(const TArray<FTransactionStep>& Steps)
	{
		FTransactionStep Result;

		const int64 Length = Steps.Num();
		for (int i = 0; i < Length; i++)
		{
			if (Steps[i].Id == EStepType::Buy)
			{
				Result = Steps[i];
				break;
			}
		}
		
		return Result;
	}
};
