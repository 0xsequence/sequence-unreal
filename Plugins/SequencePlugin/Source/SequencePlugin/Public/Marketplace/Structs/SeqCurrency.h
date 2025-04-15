#pragma once

#include "CoreMinimal.h"
#include "SeqCurrency.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqCurrency
{
	GENERATED_USTRUCT_BODY()

public:
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
		ChainID = json_in.GetIntegerField(TEXT("chainID"));
		ContractAddress = json_in.GetStringField(TEXT("contractAddress"));
		Name = json_in.GetStringField(TEXT("name"));
		Symbol = json_in.GetStringField(TEXT("symbol"));
		Decimals = json_in.GetIntegerField(TEXT("decimals"));
		ImageURL = json_in.GetStringField(TEXT("imageUrl"));
		ExchangeRate = json_in.GetNumberField(TEXT("exchangeRate"));
		DefaultChainCurrency = json_in.GetBoolField(TEXT("defaultChainCurrency"));
		CreatedAt = json_in.GetStringField(TEXT("createdAt"));
		UpdatedAt = json_in.GetStringField(TEXT("updatedAt"));
		json_in.TryGetStringField(TEXT("deletedAt"), DeletedAt);
	};
};