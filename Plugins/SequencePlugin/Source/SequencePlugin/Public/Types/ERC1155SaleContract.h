#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Sequence/Transactions.h"
#include "ERC1155SaleContract.generated.h"

UCLASS(BlueprintType, Blueprintable)
class SEQUENCEPLUGIN_API UERC1155SaleContract : public UObject
{
	GENERATED_BODY()

public:

    UFUNCTION(BlueprintCallable, Category = "ERC1155 Sale Contract")
    FRawTransaction MakePurchaseTransaction(const FString& ToAddress, const TArray<int32>& TokenIds, const TArray<int32>& Amounts, const TArray<FString>& Proof);

    UFUNCTION(BlueprintCallable, Category = "ERC1155 Sale Contract")
    FRawTransaction GetPaymentToken();

    UFUNCTION(BlueprintCallable, Category = "ERC1155 Sale Contract")
    FRawTransaction GetSaleDetails();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "ERC1155 Sale Contract")
    FString ContractAddress;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "ERC1155 Sale Contract")
    FString PaymentToken;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "ERC1155 Sale Contract")
    int32 MaxTotal;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "ERC1155 Sale Contract")
    FString Data;
};