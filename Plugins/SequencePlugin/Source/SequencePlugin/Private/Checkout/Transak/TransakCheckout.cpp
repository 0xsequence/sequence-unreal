#include "Checkout/Transak/TransakCheckout.h"
#include "Misc/DateTime.h"
#include "Misc/Guid.h"
#include "Misc/Base64.h"
#include "Containers/StringConv.h"
#include "Misc/DefaultValueHelper.h"
#include "Engine/Engine.h"
#include "Checkout/Transak/CallDataCompressor.h"
#include "Checkout/Transak/TransakNFTDataEncoder.h"
#include "Checkout/Structs/TransactionStep.h"

FString UTransakCheckout::GetNFTCheckoutLink(const FTransakNFTData& Item, const FString& CallData, const FTransakContractId& ContractId)
{
	if (ContractId.Chain != UEnum::GetValueAsString(Network))
	{
		UE_LOG(LogTemp, Error, TEXT("Chain mismatch in TransakCheckout"));
		return TEXT("Error: Chain mismatch");
	}

	const FString WalletAddress = *Wallet->GetWalletAddress();
	const FString TransakCallData = FCallDataCompressor::Compress(CallData);
	const FString BaseUrl = TEXT("https://global.transak.com");
	const FString TContractId = ContractId.Id;
	const FString TransakCryptocurrencyCode = ContractId.PriceTokenSymbol;
	const FString TransakNftDataEncoded = FTransakNFTDataEncoder::Encode(Item);
	const int64 EstimatedGasLimit = 500000;

	//GASLIMIT ESTIMATOR USE NFTDATA.COLLECTIONADDRES -> Paramaeter not needed


	const int64 Timestamp = FDateTime::UtcNow().ToUnixTimestamp();
	const FString PartnerOrderId = FString::Printf(TEXT("%s-%lld"), *WalletAddress, Timestamp);

	FString URL = FString::Printf(TEXT("%s?apiKey=%s&isNFT=true&calldata=%s&contractId=%s&cryptoCurrencyCode=%s&estimatedGasLimit=%lld&nftData=%s&walletAddress=%s&disableWalletAddressForm=true&partnerOrderId=%s"),
		*BaseUrl,
		*TEXT("YOUR_API_KEY_HERE"), // Replace with actual key or inject it
		*TransakCallData,
		*TContractId,
		*TransakCryptocurrencyCode,
		EstimatedGasLimit, 
		*TransakNftDataEncoded,
		*WalletAddress,
		*PartnerOrderId
	);

	return URL;
}

FString UTransakCheckout::BuildNFTCheckoutLinkFromERC1155(UERC1155SaleContract* SaleContract,const FTransakNFTData TransakNFTData, const FTransakContractId& ContractId, const TArray<uint8>& Data, const TArray<FString>& Proof)
{
	if (TransakNFTData.Quantity <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Quantity must be greater than 0"));
		return TEXT("Error: Invalid quantity");
	}

	TArray<int32> TokenIds;
	TArray<int32> Amounts;

	for (const FString& TokenIDString : TransakNFTData.TokenID)
	{
		int32 ParsedTokenId = FCString::Atoi(*TokenIDString);
		TokenIds.Add(ParsedTokenId);
		Amounts.Add(TransakNFTData.Quantity); 
	}

	FString callData = SaleContract->MakePurchaseTransaction(
		Wallet->GetWalletAddress(),
		TokenIds,
		Amounts,
		Proof
	).data;

	return GetNFTCheckoutLink(TransakNFTData, callData, ContractId);
}
void UTransakCheckout::BuildNFTCheckoutLinkFromCollectibleOrder(FSeqCollectibleOrder order, int64 quantity, ENFTType type, FAdditionalFee AdditionalFee, FOnTransakCheckoutGenerated OnSuccessCallback)
{
	if (quantity <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid quantity"));
		OnSuccessCallback.ExecuteIfBound(TEXT("Error: Invalid quantity"));
		return;
	}

	TArray<FString> TokenIds = { FString::FromInt(order.TokenMetadata.tokenId) };
	TArray<float> Prices = { static_cast<float>(order.Order.PriceUSD) };

	FTransakNFTData NFTData(
		order.TokenMetadata.image,
		order.TokenMetadata.name,
		order.TokenMetadata.contractAddress,
		TokenIds,
		Prices,
		quantity,
		ENFTType::ERC1155
	);

	FTransakContractId TransakContractID; //Endpoint isntead of this ?
	TransakContractID.Id = FString::FromInt(order.TokenMetadata.tokenId);
	TransakContractID.ContractAddress = order.TokenMetadata.contractAddress;
	TransakContractID.Chain = FString::FromInt(order.Order.ChainId);
	TransakContractID.PriceTokenSymbol = "USD";

	Checkout->GenerateBuyTransaction(
		order.Order.ChainId,
		Wallet->GetWalletAddress(),
		order.Order,
		quantity,
		AdditionalFee,
		EWalletKind::Sequence,

		[this, NFTData, TransakContractID, OnSuccessCallback](const FGenerateTransactionResponse& Response)
		{
			const FString URL = GetNFTCheckoutLink(NFTData, *Response.Steps[0].ExtractBuyStep(Response.Steps).Data, TransakContractID);
			OnSuccessCallback.ExecuteIfBound(URL);
		},

		[OnSuccessCallback](const FSequenceError& Error)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to generate buy transaction: %s"), *Error.Message);
			OnSuccessCallback.ExecuteIfBound(TEXT("Error: Failed to generate transaction"));
		}
	);
}