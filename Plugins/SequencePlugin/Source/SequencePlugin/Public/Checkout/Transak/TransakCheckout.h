#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Marketplace/SequenceMarketplace.h"
#include "Marketplace/Structs/SeqCollectibleOrder.h"
#include "Marketplace/TransactionNFTCheckoutProvider.h"
#include "Checkout/SequenceCheckout.h"
#include "Types/ERC1155SaleContract.h"
#include "Sequence/SequenceWallet.h"
#include "Util/Structs/BE_Enums.h"
#include "Structs/TransakStructs.h"

#include "TransakCheckout.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnTransakCheckoutGenerated, const FString&, CheckoutURL);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnTransakCheckoutFailure, const FString&, Error);


UCLASS(Blueprintable)
class SEQUENCEPLUGIN_API UTransakCheckout : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, Category = "Transak")
	FString WalletAddress;

	UPROPERTY(BlueprintReadWrite, Category = "Transak")
	TEnumAsByte<ENetwork> Network;

	static const TMap<ENetwork, FString> TransakContractAddresses;

	UFUNCTION(BlueprintCallable, Category = "Transak")
	void Initialize(const FString& InWalletAddress, ENetwork InNetwork);

	UFUNCTION(BlueprintCallable, Category = "Transak")
	FString GetNFTCheckoutLink(const FTransakNFTData& Item, const FString& CallData, const FTransakContractId& ContractId);


	UFUNCTION(BlueprintCallable, Category = "Transak")
	FString BuildNFTCheckoutLinkFromERC1155(UERC1155SaleContract* SaleContract, const FTransakNFTData TransakNFTData, const FTransakContractId& ContractId,
		const TArray<FString>& Proof,
		const FString& Data);

	UFUNCTION(BlueprintCallable, Category = "Transak")
	void BuildNFTCheckoutLinkFromCollectibleOrder(FSeqCollectibleOrder Order, int64 Quantity, ENFTType Type, FAdditionalFee AdditionalFee, FOnTransakCheckoutGenerated OnSuccessCallback, FOnTransakCheckoutFailure OnFailureCallback);

	UFUNCTION(BlueprintCallable, Category = "Transak")
	FTransakContractId GetTransakContractIdFromCollectibleOrder(FSeqCollectibleOrder order);



};
