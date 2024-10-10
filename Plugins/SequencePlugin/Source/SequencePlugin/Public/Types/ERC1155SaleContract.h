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

    UERC1155SaleContract(FString in_ContractAddress, FString in_PaymentToken, int32 in_MaxTotal, FString in_Data);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "ERC1155 Sale")
    FString ContractAddress;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "ERC1155 Sale")
    FString PaymentToken;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "ERC1155 Sale")
    int32 MaxTotal;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "ERC1155 Sale")
    FString Data;

    UFUNCTION(BlueprintCallable, Category = "ERC1155 Sale")
    FRawTransaction MakePurchaseTransaction(const FString& ToAddress, const TArray<int32>& TokenIds, const TArray<int32>& Amounts, const TArray<FString>& Proof);

    UFUNCTION(BlueprintCallable, Category = "ERC1155 Sale")
    FRawTransaction GetPaymentToken();

    UFUNCTION(BlueprintCallable, Category = "ERC1155 Sale")
    FRawTransaction GetSaleDetails();
};