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
	UE_LOG(LogTemp, Warning, TEXT("Entered GetNFTCheckoutLink"));

	const FString NetworkString = UEnum::GetValueAsString(Network);
	UE_LOG(LogTemp, Warning, TEXT("NetworkString: %s"), *NetworkString);
	UE_LOG(LogTemp, Warning, TEXT("Provided ContractId.Chain: %s"), *ContractId.Chain);

	if (ContractId.Chain != NetworkString)
	{
		UE_LOG(LogTemp, Error, TEXT("Chain mismatch in TransakCheckout"));
		return TEXT("Error: Chain mismatch");
	}

	UE_LOG(LogTemp, Warning, TEXT("Compressing call data..."));
	const FString TransakCallData = FCallDataCompressor::Compress(CallData);
	UE_LOG(LogTemp, Warning, TEXT("Compressed call data: %s"), *TransakCallData);

	UE_LOG(LogTemp, Warning, TEXT("Encoding NFT data..."));
	const FString TransakNftDataEncoded = FTransakNFTDataEncoder::Encode(Item);
	UE_LOG(LogTemp, Warning, TEXT("Base64 Encoded (with padding): %s"), *TransakNftDataEncoded);

	const FString BaseURL = TEXT("https://global.transak.com/");
	UE_LOG(LogTemp, Warning, TEXT("BaseURL: %s"), *BaseURL);

	const FString TContractId = ContractId.Id;
	const FString TransakCryptocurrencyCode = ContractId.PriceTokenSymbol;
	const int64 EstimatedGasLimit = 500000;

	if (WalletAddress.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("WalletAddress is not set!"));
		return TEXT("Error: Wallet address missing");
	}

	UE_LOG(LogTemp, Warning, TEXT("WalletAddress: %s"), *WalletAddress);

	const int64 Timestamp = FDateTime::UtcNow().ToUnixTimestamp();
	UE_LOG(LogTemp, Warning, TEXT("Timestamp: %lld"), Timestamp);

	const FString PartnerOrderId = FString::Printf(TEXT("%s-%lld"), *WalletAddress, Timestamp);

	UE_LOG(LogTemp, Warning, TEXT("Building final URL..."));

	FString address = *WalletAddress;

	FString URL =
		BaseURL + TEXT("?apiKey=5911d9ec-46b5-48fa-a755-d59a715ff0cf")
		+ TEXT("&isNFT=true")
		+ TEXT("&calldata=") + TransakCallData
		+ TEXT("&contractId=") + TContractId
		+ TEXT("&cryptoCurrencyCode=") + TransakCryptocurrencyCode
		+ TEXT("&estimatedGasLimit=") + FString::Printf(TEXT("%lld"), EstimatedGasLimit)
		+ TEXT("&nftData=") + TransakNftDataEncoded
		+ TEXT("&walletAddress=") + address
		+ TEXT("&disableWalletAddressForm=true")
		+ TEXT("&partnerOrderId=") + PartnerOrderId;

	UE_LOG(LogTemp, Warning, TEXT("Final URL: %s"), *URL);
	return URL;
}


FString UTransakCheckout::BuildNFTCheckoutLinkFromERC1155(UERC1155SaleContract* SaleContract, const FTransakNFTData TransakNFTData, const FTransakContractId& ContractId, const TArray<uint8>& Data, const TArray<FString>& Proof)
{
	UE_LOG(LogTemp, Warning, TEXT("Entered BuildNFTCheckoutLinkFromERC1155"));

	if (!SaleContract)
	{
		UE_LOG(LogTemp, Error, TEXT("SaleContract is nullptr!"));
		return TEXT("Error: Invalid SaleContract");
	}

	if (TransakNFTData.Quantity <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Quantity must be greater than 0"));
		return TEXT("Error: Invalid quantity");
	}

	TArray<int32> TokenIds;
	TArray<int32> Amounts;

	UE_LOG(LogTemp, Warning, TEXT("Processing Token IDs..."));
	for (const FString& TokenIDString : TransakNFTData.TokenID)
	{
		int32 ParsedTokenId = FCString::Atoi(*TokenIDString);
		TokenIds.Add(ParsedTokenId);
		Amounts.Add(TransakNFTData.Quantity);
		UE_LOG(LogTemp, Warning, TEXT("Parsed TokenID: %d, Quantity: %d"), ParsedTokenId, TransakNFTData.Quantity);
	}

	UE_LOG(LogTemp, Warning, TEXT("Calling MakePurchaseTransaction on SaleContract..."));


	FRawTransaction result = SaleContract->MakePurchaseTransaction(
		WalletAddress,
		TokenIds,
		Amounts,
		Proof
	);

	FString callData = result.data;


	UE_LOG(LogTemp, Warning, TEXT("Transaction callData: %s"), *callData);
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