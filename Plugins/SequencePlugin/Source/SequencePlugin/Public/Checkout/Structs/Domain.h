#pragma once

#include "Domain.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FDomain
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Name;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Version;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int64 ChainId;
	// Chain enum?
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString VerifyingContract;

	FDomain()
	{
		this->Name = "";
		this->Version = "";
		this->ChainId = 0;
		this->VerifyingContract = "";
	}

	FDomain(FString Name, FString Version, const uint64 ChainId, FString VerifyingContract)
	{
		this->Name = Name;
		this->Version = Version;
		this->ChainId = ChainId;
		this->VerifyingContract = VerifyingContract;
	}
	
	bool customGetter = true;
	FString GetArgs() const
	{
		FString ReturnArgs = "{";

		ReturnArgs += "\"name\":\"" + Name + "\",";
		ReturnArgs += "\"version\":\"" + Version + "\",";
		ReturnArgs += "\"chainId\":\"" + FString::Printf(TEXT("%lld"), ChainId) + "\",";
		ReturnArgs += "\"verifyingContract\":\"" + VerifyingContract + "\"";
		

		ReturnArgs.Append("}"); // Close the JSON object
		
		return ReturnArgs;
	}

	void Setup(FJsonObject& JSON_In)
	{
		Name = JSON_In.GetStringField(TEXT("name"));
		Version = JSON_In.GetStringField(TEXT("version"));
		ChainId = JSON_In.GetIntegerField(TEXT("chainId"));
		VerifyingContract = JSON_In.GetStringField(TEXT("verifyingContract"));
	}
};
