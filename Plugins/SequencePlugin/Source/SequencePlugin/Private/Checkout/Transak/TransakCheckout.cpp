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
void UTransakCheckout::Initialize(const FString& InWalletAddress, ENetwork InNetwork)
{
	WalletAddress = InWalletAddress;
	Network = InNetwork;
}
FString UTransakCheckout::GetNFTCheckoutLink(const FTransakNFTData& Item, const FString& CallData, const FTransakContractId& ContractId)
{
	const FString NetworkString = UEnum::GetValueAsString(Network);

	if (ContractId.Chain != NetworkString)
	{
		FString ChainMismatchError = FString::Printf(TEXT("Chain mismatch: ContractId.Chain = %s, Network = %s"), *ContractId.Chain, *NetworkString);
		return ChainMismatchError;
	}

	const FString TransakCallData = FCallDataCompressor::Compress(CallData);

	const FString TransakNftDataEncoded = FTransakNFTDataEncoder::Encode(Item);

	const FString BaseURL = TEXT("https://global.transak.com/");

	const FString TContractId = ContractId.Id;
	const FString TransakCryptocurrencyCode = ContractId.PriceTokenSymbol;
	const int64 EstimatedGasLimit = 500000;

	if (WalletAddress.IsEmpty())
	{
		return TEXT("Error: Transak Checkout object wallet address is missing");
	}


	const int64 Timestamp = FDateTime::UtcNow().ToUnixTimestamp();

	const FString PartnerOrderId = FString::Printf(TEXT("%s-%lld"), *WalletAddress, Timestamp);

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


FString UTransakCheckout::BuildNFTCheckoutLinkFromERC1155(UERC1155SaleContract* SaleContract, const FTransakNFTData TransakNFTData, const FTransakContractId& ContractId,  const TArray<FString>& Proof, const FString& Data)
{

	if (!SaleContract)
	{
		return TEXT("Error: Invalid SaleContract");
	}

	if (TransakNFTData.Quantity <= 0)
	{
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


	FRawTransaction result = SaleContract->MakePurchaseTransaction(
		TransakContractAddresses[Network],
		TokenIds,
		Amounts,
		Proof,
		Data
	);

	FString callData = result.data;


	return GetNFTCheckoutLink(TransakNFTData, callData, ContractId);
}
void UTransakCheckout::BuildNFTCheckoutLinkFromCollectibleOrder(FSeqCollectibleOrder order, int64 quantity, ENFTType type, FAdditionalFee AdditionalFee, FOnTransakCheckoutGenerated OnSuccessCallback, FOnTransakCheckoutFailure OnFailureCallback)
{
	if (quantity <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid quantity"));
		return;
	}

	double PriceAmount = FCString::Atod(*order.Order.PriceAmount);
	double PriceDecimals = static_cast<double>(order.Order.PriceDecimals);
	float NormalizedPrice = static_cast<float>(PriceAmount / FMath::Pow(10.0, PriceDecimals));
	TArray<float> Prices = { NormalizedPrice };

	TArray<FString> TokenIds = { FString::FromInt(order.TokenMetadata.tokenId) };

	FTransakNFTData NFTData(
		order.TokenMetadata.image,
		order.TokenMetadata.name,
		order.TokenMetadata.contractAddress,
		TokenIds,
		Prices,
		quantity,
		ENFTType::ERC721
	);


	FTransakContractId TransakContractID = GetTransakContractIdFromCollectibleOrder(order);

	if (TransakContractID.Id.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("TransakContractID is empty. Aborting."));
		return;
	}

	USequenceCheckout* Checkout = NewObject< USequenceCheckout>();

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

		[OnFailureCallback](const FSequenceError& Error)
		{
			OnFailureCallback.ExecuteIfBound(TEXT("Error: Failed to generate buy transaction"));
		}
	);
}

FTransakContractId UTransakCheckout::GetTransakContractIdFromCollectibleOrder(FSeqCollectibleOrder order)
{
	FTransakContractId TransakContractID;

	if (order.Order.ChainId == 137)
	{
		switch (order.Order.Marketplace)
		{
		case EMarketplaceKind::SEQUENCE_MARKETPLACE_V2:

			TransakContractID.Id = TEXT("67ac543448035690a20ac131");
			TransakContractID.ContractAddress = TEXT("0xfdb42A198a932C8D3B506Ffa5e855bC4b348a712");
			TransakContractID.Chain = TransakContractID.Chain = UEnum::GetValueAsString(ENetwork::PolygonChain);
			TransakContractID.PriceTokenSymbol = TEXT("POL");

			break;

		default:

			TransakContractID.Id = TEXT("6675a6d0f597abb8f3e2e9c2");
			TransakContractID.ContractAddress = TEXT("0xc2c862322e9c97d6244a3506655da95f05246fd8");
			TransakContractID.Chain = TransakContractID.Chain = UEnum::GetValueAsString(ENetwork::PolygonChain);
			TransakContractID.PriceTokenSymbol = TEXT("MATIC");

			break;
		}
	}
	else if (order.Order.ChainId == 42161)
	{

		if (order.Order.Marketplace == EMarketplaceKind::SEQUENCE_MARKETPLACE_V2)
		{
			TransakContractID.Id = TEXT("66c5a2cf2fb1688e11fcb167");
			TransakContractID.ContractAddress = TEXT("0xB537a160472183f2150d42EB1c3DD6684A55f74c");
			TransakContractID.Chain = UEnum::GetValueAsString(ENetwork::ArbitrumOne);
			TransakContractID.PriceTokenSymbol = TEXT("USDC");
		}
		else if (order.Order.Marketplace == EMarketplaceKind::SEQUENCE_MARKETPLACE_V1)
		{
			TransakContractID.Id = TEXT("66c5a2d8c00223b9cc6edfdc");
			TransakContractID.ContractAddress = TEXT("0xfdb42A198a932C8D3B506Ffa5e855bC4b348a712");
			TransakContractID.Chain = UEnum::GetValueAsString(ENetwork::ArbitrumOne);
			TransakContractID.PriceTokenSymbol = TEXT("USDC");
		}
	}

	return TransakContractID;
}


const TMap<ENetwork, FString> UTransakCheckout::TransakContractAddresses = {
	{ENetwork::Ethereum, TEXT("0xab88cd272863b197b48762ea283f24a13f6586dd")},
	{ENetwork::Sepolia, TEXT("0xD84aC4716A082B1F7eCDe9301aA91A7c4B62ECd7")},
	{ENetwork::PolygonChain, TEXT("0x4A598B7eC77b1562AD0dF7dc64a162695cE4c78A")},
	{ENetwork::PolygonAmoy, TEXT("0xCB9bD5aCD627e8FcCf9EB8d4ba72AEb1Cd8Ff5EF")},
	{ENetwork::BNBSmartChain, TEXT("0x4A598B7eC77b1562AD0dF7dc64a162695cE4c78A")},
	{ENetwork::BNBSmartChainTestnet, TEXT("0x0E9539455944BE8a307bc43B0a046613a1aD6732")},
	{ENetwork::ArbitrumOne, TEXT("0x4A598B7eC77b1562AD0dF7dc64a162695cE4c78A")},
	{ENetwork::ArbitrumSepolia, TEXT("0x489F56e3144FF03A887305839bBCD20FF767d3d1")},
	{ENetwork::Optimism, TEXT("0x4A598B7eC77b1562AD0dF7dc64a162695cE4c78A")},
	{ENetwork::OptimismSepolia, TEXT("0xCB9bD5aCD627e8FcCf9EB8d4ba72AEb1Cd8Ff5EF")},
	{ENetwork::Immutable, TEXT("0x8b83dE7B20059864C479640CC33426935DC5F85b")},
	{ENetwork::ImmutableTestnet, TEXT("0x489F56e3144FF03A887305839bBCD20FF767d3d1")},
	{ENetwork::Base, TEXT("0x8b83dE7B20059864C479640CC33426935DC5F85b")},
	{ENetwork::BaseSepolia, TEXT("0xCB9bD5aCD627e8FcCf9EB8d4ba72AEb1Cd8Ff5EF")}
};