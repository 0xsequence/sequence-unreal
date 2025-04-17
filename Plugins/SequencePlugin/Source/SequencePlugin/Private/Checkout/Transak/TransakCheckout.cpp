#include "Checkout/Transak/TransakCheckout.h"
#include "Misc/DateTime.h"
#include "Misc/Guid.h"
#include "Misc/Base64.h"
#include "Containers/StringConv.h"
#include "Misc/DefaultValueHelper.h"
#include "Engine/Engine.h"

// Placeholder for external helpers
namespace CallDataCompressor
{
	FString Compress(const FString& In)
	{
		// Placeholder 
		return FBase64::Encode(In);
	}
}

namespace NftDataEncoder
{
	FString Encode(const FTransakNFTData& NftData)
	{
		// Placeholder
		return FBase64::Encode(TEXT("{ \"sample\": \"nftData\" }"));
	}
}

FString UTransakCheckout::GetNFTCheckoutLink(const FTransakNFTData& Item, const FString& CallData, const FTransakContractId& ContractId)
{
	if (ContractId.Chain != UEnum::GetValueAsString(Network))
	{
		UE_LOG(LogTemp, Error, TEXT("Chain mismatch in TransakCheckout"));
		return TEXT("Error: Chain mismatch");
	}
	const FString WalletAddress = *Wallet->GetWalletAddress();
	const FString TransakCallData = CallDataCompressor::Compress(CallData);
	const FString BaseUrl = TEXT("https://global.transak.com");
	const FString TContractId = ContractId.Id;
	const FString TransakCryptocurrencyCode = ContractId.PriceTokenSymbol;
	const FString TransakNftDataEncoded = NftDataEncoder::Encode(Item);
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

	FContractCall PaymentTokenContractCall = SaleContract->GetPaymentToken();
	TOptional<FString> PaymentToken = PaymentTokenContractCall.Data;

	FContractCall SaleDetailsContractCall = SaleContract->GetPaymentToken();
	TOptional<FString> SaleDetails = SaleDetailsContractCall.Data;

	//Missing actual queries

	int32 Decimals = PaymentTokenContract->GetDecimals(); //undefined 

	FString CallData = //ContractCallFunction from unity(
		TransakContractAddresses[Network],
		{ TokenId },
		{ Quantity },
		Data,
		PaymentToken,
		SaleContract.SaleDetails.Cost * Quantity, 
		Proof
	); 

	TArray<FString> TokenIDs = { TokenId };
	TArray<float> Prices;
	Prices.Add(DecimalNormalizer::ReturnToNormalPrecise(SaleDetails.Cost, Decimals));

	return GetNFTCheckoutLink(TransakNFTData, CallData, ContractId);
}
