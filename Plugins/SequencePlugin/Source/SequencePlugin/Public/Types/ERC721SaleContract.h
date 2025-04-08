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

    UERC721SaleContract(FString ContractAddress, FString PaymentToken, int32 MaxTotal, FString Data);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "ERC721 Sale")
    FString ContractAddress;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "ERC721 Sale")
    FString PaymentToken;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "ERC721 Sale")
    int32 MaxTotal;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "ERC721 Sale")
    FString Data;

    UFUNCTION(BlueprintCallable, Category = "ERC721 Sale")
    FRawTransaction MakePurchaseTransaction(const FString& ToAddress, const int32& Amount, const TArray<FString>& Proof);

    void Initialize(FString _ContractAddress, FString _PaymentToken, int32 _MaxTotal, FString _Data);

    FContractCall GetSaleDetails();

};