#pragma once

#include "CoreMinimal.h"
#include "TransakContractId.generated.h"

// You may need to replace these forward declarations with actual types if you have them defined
USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FTransakContractId
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Transak")
	FString Id;

	// Assuming Address is a string or a custom wrapper, using FString for now
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Transak")
	FString ContractAddress;

	// Assuming Chain is an enum, replace with appropriate type if needed
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Transak")
	FString Chain;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Transak")
	FString PriceTokenSymbol;
};
