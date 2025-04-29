#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Sequence/Transactions.h"
#include "ContractCall.h"
#include "ERC1155SaleContract.generated.h"

UCLASS(BlueprintType, Blueprintable)
class SEQUENCEPLUGIN_API UERC1155SaleContract : public UObject
{
	GENERATED_BODY()

public:
    UERC1155SaleContract();

    UERC1155SaleContract(FString ContractAddress, FString PaymentToken, int32 MaxTotal, FString Data);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "0xSequence - ERC1155 Sale")
    FString ContractAddress;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "0xSequence - ERC1155 Sale")
    FString PaymentToken;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "0xSequence - ERC1155 Sale")
    int32 MaxTotal;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "0xSequence - ERC1155 Sale")
    FString Data;

    UFUNCTION(BlueprintCallable, Category = "0xSequence - ERC1155 Sale")
    FRawTransaction MakePurchaseTransaction(const FString& ToAddress, const TArray<int32>& TokenIds, const TArray<int32>& Amounts, const TArray<FString>& Proof);


    FContractCall GetPaymentToken();

    FContractCall GetGlobalSaleDetails();


};