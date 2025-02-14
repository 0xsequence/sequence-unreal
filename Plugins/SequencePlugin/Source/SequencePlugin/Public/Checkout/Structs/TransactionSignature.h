#pragma once
#include "Domain.h"
#include "Util/SequenceSupport.h"

#include "TransactionSignature.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FTransactionSignature
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FDomain Domain;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Types;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString PrimaryType;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Value;

	FTransactionSignature()
	{
		Domain = FDomain();
		Types = "";
		PrimaryType = "";
		Value = "";
	}

	FTransactionSignature(const FDomain& Domain, FString Types, FString PrimaryType, FString Value)
	{
		this->Domain = Domain;
		this->Types = Types;
		this->PrimaryType = PrimaryType;
		this->Value = Value;
	}
	
	bool customGetter = true;
	FString GetArgs() const
	{
		FString ReturnArgs = "{";

		ReturnArgs += "\"domain\":\"" + Domain.GetArgs() + "\",";
		ReturnArgs += "\"types\":\"" + Types + "\",";
		ReturnArgs += "\"primaryType\":\"" + PrimaryType + "\",";
		ReturnArgs += "\"value\":\"" + Value + "\"";
		

		ReturnArgs.Append("}"); // Close the JSON object
		
		return ReturnArgs;
	}

	void Setup(FJsonObject& JSON_In)
	{
		// Setup Domain
		if (JSON_In.TryGetField(TEXT("domain")))
		{
			const TSharedPtr<FJsonValue> DomainValue = JSON_In.GetField(TEXT("domain"), EJson::Object);
			const TSharedPtr<FJsonObject> DomainJsonObject = USequenceSupport::JsonStringToObject(DomainValue->AsString());
			Domain.Setup(*DomainJsonObject);
		}
		
		Types = JSON_In.GetStringField(TEXT("types"));
		PrimaryType = JSON_In.GetStringField(TEXT("primaryType"));
		Value = JSON_In.GetStringField(TEXT("value"));
	}
};
