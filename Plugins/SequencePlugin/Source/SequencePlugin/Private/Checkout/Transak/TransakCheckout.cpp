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
#include "Util/SequenceSupport.h"

FString UTransakCheckout::GetNFTCheckoutLink(const FTransakNFTData& Item, const FString& CallData, const FTransakContractId& ContractId)
{
	if (ContractId.Chain != UEnum::GetValueAsString(Network))
	{
		UE_LOG(LogTemp, Error, TEXT("Chain mismatch in TransakCheckout"));
		return TEXT("Error: Chain mismatch");
	}

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
		*TEXT("5911d9ec-46b5-48fa-a755-d59a715ff0cf"), // This can be hardcoded as it is a public API key
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

FString UTransakCheckout::BuildNFTCheckoutLinkFromERC1155(UERC1155SaleContract* SaleContract,const FTransakNFTData TransakNFTData, const FTransakContractId& ContractId, const TArray<uint8>& Data ,const TArray<FString>& Proof)
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
		WalletAddress,
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

	FTransakContractId TransakContractID = GetTransakContractIdFromCollectibleOrder(order);

	if (TransakContractID.Id.IsEmpty())return;

	Checkout->GenerateBuyTransaction(
		order.Order.ChainId,
		WalletAddress,
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

FTransakContractId UTransakCheckout::GetTransakContractIdFromCollectibleOrder(FSeqCollectibleOrder order)
{
	FTransakContractId TransakContractID;

	// Check the chain ID and set the contract info
	if (order.Order.ChainId == 137) // Polygon
	{
		// Map the MarketplaceKind to OrderbookKind and set the corresponding contract info
		
		switch (order.Order.Marketplace)
		{
		case EMarketplaceKind::SEQUENCE_MARKETPLACE_V2:
			TransakContractID.Id = TEXT("67ac543448035690a20ac131");
			TransakContractID.ContractAddress = TEXT("0xfdb42A198a932C8D3B506Ffa5e855bC4b348a712");
			TransakContractID.Chain = USequenceSupport::GetNetworkName(ENetwork::PolygonChain); // Or as a string: TEXT("Polygon")
			TransakContractID.PriceTokenSymbol = TEXT("POL");
			break;

		default:
			// Default case for unknown or unhandled marketplace kinds
			TransakContractID.Id = TEXT("6675a6d0f597abb8f3e2e9c2");
			TransakContractID.ContractAddress = TEXT("0xc2c862322e9c97d6244a3506655da95f05246fd8");
			TransakContractID.Chain = USequenceSupport::GetNetworkName(ENetwork::PolygonChain); // Or as a string: TEXT("Polygon")
			TransakContractID.PriceTokenSymbol = TEXT("MATIC");
			break;
		}
		
	}
	else if (order.Order.ChainId == 42161) // ArbitrumOne
	{
		// Map the MarketplaceKind to OrderbookKind and set the corresponding contract info
		if (order.Order.Marketplace == EMarketplaceKind::SEQUENCE_MARKETPLACE_V2)
		{
			TransakContractID.Id = "66c5a2cf2fb1688e11fcb167";
			TransakContractID.ContractAddress = "0xB537a160472183f2150d42EB1c3DD6684A55f74c";
			TransakContractID.Chain = USequenceSupport::GetNetworkName(ENetwork::ArbitrumOne);
			TransakContractID.PriceTokenSymbol = "USDC";
		}
		else if (order.Order.Marketplace == EMarketplaceKind::SEQUENCE_MARKETPLACE_V1)
		{
			TransakContractID.Id = "66c5a2d8c00223b9cc6edfdc";
			TransakContractID.ContractAddress = "0xfdb42A198a932C8D3B506Ffa5e855bC4b348a712";
			TransakContractID.Chain = USequenceSupport::GetNetworkName(ENetwork::ArbitrumOne);
			TransakContractID.PriceTokenSymbol = "USDC";
		}
	}
	return TransakContractID;
}