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

		ReturnArgs += "\"domain\":" + Domain.GetArgs() + ",";
		ReturnArgs += "\"types\":\"" + Types + "\",";
		ReturnArgs += "\"primaryType\":\"" + PrimaryType + "\",";
		ReturnArgs += "\"value\":\"" + Value + "\"";
		

		ReturnArgs.Append("}"); // Close the JSON object
		
		return ReturnArgs;
	}

	void Setup(const FJsonObject& JSON_In)
	{
		// Setup Domain
		if (JSON_In.HasField(TEXT("domain")))
		{
			const TSharedPtr<FJsonValue> DomainValue = JSON_In.TryGetField(TEXT("domain"));
			if (DomainValue != nullptr)
			{
				const TSharedPtr<FJsonObject> DomainJsonObject = DomainValue->AsObject(); //USequenceSupport::JsonStringToObject(DomainValue->AsString());
				if (DomainJsonObject != nullptr)
				{
					Domain.Setup(*DomainJsonObject);
				}
			}
		}

		if (FString ReturnTypesString; JSON_In.TryGetStringField(TEXT("types"), ReturnTypesString))
		{
			Types = ReturnTypesString;
		}

		if (FString ReturnPrimaryTypeString; JSON_In.TryGetStringField(TEXT("primaryType"), ReturnPrimaryTypeString))
		{
			PrimaryType = ReturnPrimaryTypeString;
		}
		
		if (FString ReturnValueString; JSON_In.TryGetStringField(TEXT("value"), ReturnValueString))
		{
			Value = ReturnValueString;
		}
	}
};
