#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Sequence/Transactions.h"
#include "ContractCall.h"
#include "ERC721SaleContract.generated.h"

UCLASS(BlueprintType, Blueprintable)
class SEQUENCEPLUGIN_API UERC721SaleContract : public UObject
{
    GENERATED_BODY()

public:
    UERC721SaleContract();

    UFUNCTION(BlueprintCallable, Category = "0xSequence - ERC721 Sale")
    static UERC721SaleContract* Create(const FString& ContractAddress, const FString& PaymentToken, int32 MaxTotal, const FString& Data);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "0xSequence - ERC721 Sale")
    FString ContractAddress;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "0xSequence - ERC721 Sale")
    FString PaymentToken;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "0xSequence - ERC721 Sale")
    int32 MaxTotal;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "0xSequence - ERC721 Sale")
    FString Data;

    UFUNCTION(BlueprintCallable, Category = "0xSequence - ERC721 Sale")
    FRawTransaction MakePurchaseTransaction(const FString& ToAddress, const int32& Amount, const TArray<FString>& Proof);

    UFUNCTION(BlueprintCallable, Category = "0xSequence - ERC721 Sale")
    FContractCall GetSaleDetails();

private:
    void Init(const FString& InContractAddress, const FString& InPaymentToken, int32 InMaxTotal, const FString& InData);
};