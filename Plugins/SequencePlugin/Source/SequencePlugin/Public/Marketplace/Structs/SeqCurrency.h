#pragma once

#include "CoreMinimal.h"
#include "SeqCurrency.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqCurrency
{
	GENERATED_USTRUCT_BODY()

public:
	// Properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ChainID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ContractAddress;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Symbol;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Decimals;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ImageURL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double ExchangeRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool DefaultChainCurrency;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString CreatedAt;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString UpdatedAt;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DeletedAt;

	void Setup(FJsonObject json_in)
	{
		ID = json_in.GetIntegerField(TEXT("id"));
		ChainID = json_in.GetIntegerField(TEXT("chain_id"));
		ContractAddress = json_in.GetStringField(TEXT("contract_address"));
		Name = json_in.GetStringField(TEXT("name"));
		Symbol = json_in.GetStringField(TEXT("symbol"));
		Decimals = json_in.GetIntegerField(TEXT("decimals"));
		ImageURL = json_in.GetStringField(TEXT("image_url"));
		ExchangeRate = json_in.GetNumberField(TEXT("exchange_rate"));
		DefaultChainCurrency = json_in.GetBoolField(TEXT("default_chain_currency"));
		CreatedAt = json_in.GetStringField(TEXT("created_at"));
		UpdatedAt = json_in.GetStringField(TEXT("updated_at"));
		DeletedAt = json_in.GetStringField(TEXT("deleted_at"));
	};
};