#pragma once

#include "CoreMinimal.h"
#include "TransakContractId.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FTransakContractId
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Transak")
	FString Id;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Transak")
	FString ContractAddress;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Transak")
	FString Chain;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Transak")
	FString PriceTokenSymbol;
};
