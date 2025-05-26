#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Sequence/Transactions.h"
#include "ContractCall.h"
#include "ERC1155GlobalSaleDetails.h"
#include "ERC1155SaleContract.generated.h"

UCLASS(BlueprintType, Blueprintable)
class SEQUENCEPLUGIN_API UERC1155SaleContract : public UObject
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetGlobalSaleDetails, const FERC1155GlobalSaleDetails&, SaleDetails);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnFailure, const FString&, Error);
	
    UERC1155SaleContract();

    UFUNCTION(BlueprintCallable, Category = "0xSequence - ERC1155 Sale")
    static UERC1155SaleContract* Create(const FString& ContractAddress, const FString& PaymentToken, int32 MaxTotal, const FString& Data);

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

	UFUNCTION(BlueprintCallable, Category = "0xSequence - ERC1155 Sale")
    FContractCall GetPaymentToken();

	UFUNCTION(BlueprintCallable, Category = "0xSequence - ERC1155 Sale")
	FContractCall GetGlobalSaleDetailsCallData();

	UFUNCTION(BlueprintCallable, Category = "0xSequence - ERC1155 Sale")
	void GetGlobalSaleDetails(FOnGetGlobalSaleDetails OnSuccess, FOnFailure OnFailure);

private:
    void Init(const FString& InContractAddress, const FString& InPaymentToken, int32 InMaxTotal, const FString& InData);
};