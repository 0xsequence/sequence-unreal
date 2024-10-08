#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Sequence/Transactions.h" 
#include "ERC721SaleContract.generated.h"

UCLASS(BlueprintType, Blueprintable)
class SEQUENCEPLUGIN_API UERC721SaleContract : public UObject
{
    GENERATED_BODY()

public:
    // This function creates the purchase transaction, similar to the Mint method for ERC721
    UFUNCTION(BlueprintCallable, Category = "ERC721 Sale Contract")
    FRawTransaction MakePurchaseTransaction(const FString& ToAddress, const int32& Amount, const TArray<FString>& Proof);

    // Expose these properties at construction (without the need for Initialize)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "ERC721 Sale Contract")
    FString ContractAddress;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "ERC721 Sale Contract")
    FString PaymentToken;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "ERC721 Sale Contract")
    int32 MaxTotal;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "ERC721 Sale Contract")
    FString Data;
};