#pragma once

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
#include "SeqCurrency.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqCurrency
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	int ChainID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FString ContractAddress;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FString Symbol;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	int Decimals;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FString ImageURL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	double ExchangeRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	bool DefaultChainCurrency;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FString CreatedAt;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FString UpdatedAt;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
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