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

    UERC1155SaleContract(FString ContractAddress, FString PaymentToken, int32 MaxTotal);

    UFUNCTION(BlueprintCallable, Category = "ERC1155 Sale")
    static UERC1155SaleContract* Create(const FString& ContractAddress, const FString& PaymentToken, int32 MaxTotal);
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "0xSequence - ERC1155 Sale")
    FString ContractAddress;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "0xSequence - ERC1155 Sale")
    FString PaymentToken;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "0xSequence - ERC1155 Sale")
    int32 MaxTotal;

    UFUNCTION(BlueprintCallable, Category = "0xSequence - ERC1155 Sale")
    FRawTransaction MakePurchaseTransaction(const FString& ToAddress, const TArray<int32>& TokenIds, const TArray<int32>& Amounts, const TArray<FString>& Proof, const FString Data);

    FContractCall GetPaymentToken();

    FContractCall GetGlobalSaleDetails();
    
private:
    void Init(const FString& InContractAddress, const FString& InPaymentToken, int32 InMaxTotal);

};

USTRUCT(BlueprintType)
struct FERC1155SaleDetails
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "ERC1155 Sale")
    FString Cost; 

    UPROPERTY(BlueprintReadWrite, Category = "ERC1155 Sale")
    FString SupplyCap; 

    UPROPERTY(BlueprintReadWrite, Category = "ERC1155 Sale")
    int64 StartTime; 

    UPROPERTY(BlueprintReadWrite, Category = "ERC1155 Sale")
    int64 EndTime;

    UPROPERTY(BlueprintReadWrite, Category = "ERC1155 Sale")
    FString MerkleRoot; 
};