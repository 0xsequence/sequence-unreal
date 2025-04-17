#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Marketplace/SequenceMarketplace.h"
#include "Marketplace/TransactionNFTCheckoutProvider.h"
#include "Checkout/SequenceCheckout.h"
#include "Types/ERC1155SaleContract.h"
#include "Sequence/SequenceWallet.h"
#include "Util/Structs/BE_Enums.h"
#include "Structs/TransakStructs.h"

#include "TransakCheckout.generated.h"


UCLASS()
class SEQUENCEPLUGIN_API UTransakCheckout : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, Category = "Transak")
	USequenceWallet* Wallet;

	UPROPERTY(BlueprintReadWrite, Category = "Transak")
	TEnumAsByte<ENetwork> Network;

	UPROPERTY(BlueprintReadWrite, Category = "Transak")
	USequenceCheckout* Checkout;

	UPROPERTY(BlueprintReadWrite, Category = "Transak")
	USequenceMarketplace* Marketplace;

	UPROPERTY(BlueprintReadWrite, Category = "Transak")
	FTransakContractId TransakContractId;


	UFUNCTION(BlueprintCallable, Category = "Transak")
	FString GetNFTCheckoutLink(const FTransakNFTData& Item, const FString& CallData, const FTransakContractId& ContractId);

	UFUNCTION(BlueprintCallable, Category = "Transak")
	FString BuildNFTCheckoutLinkFromERC1155(UERC1155SaleContract* SaleContract, const FTransakNFTData TransakNFTData, const FTransakContractId& ContractId,
		const TArray<uint8>& Data, const TArray<FString>& Proof);

};

