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

	void Setup(FJsonObject json_in)
	{
		ID = json_in.GetIntegerField(TEXT("id"));
		ChainID = json_in.GetIntegerField(TEXT("chain_id"));
		ContractAddress = json_in.GetStringField(TEXT("contract_address"));	
	};
};