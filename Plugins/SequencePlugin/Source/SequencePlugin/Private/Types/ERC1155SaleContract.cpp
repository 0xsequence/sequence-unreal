#include "Types/ERC1155SaleContract.h"
#include "ABI/ABI.h"

UERC1155SaleContract::UERC1155SaleContract(FString in_ContractAddress, FString in_PaymentToken, int32 in_MaxTotal, FString in_Data)
{
	ContractAddress = in_ContractAddress;
	PaymentToken = in_PaymentToken;
	MaxTotal = in_MaxTotal;
	Data = in_Data;
}

FRawTransaction UERC1155SaleContract::MakePurchaseTransaction(const FString& ToAddress, const TArray<int32>& TokenIds, const TArray<int32>& Amounts, const TArray<FString>& Proof)
{
	FString FunctionSignature = "mint(address,uint256[],uint256[],bytes,address,uint256,bytes32[])";

	FString WalletAddress = ToAddress;
	FString WalletAddressNoPrefix = WalletAddress.Mid(2, WalletAddress.Len());
	FAddress WalletAddressBytes = FAddress::From(WalletAddressNoPrefix);

	TFixedABIData ABIAccount = ABI::Address(WalletAddressBytes);

	TArray<ABIEncodeable*> TokenIdsArray;
	for (uint32 TokenId : TokenIds)
	{
		TokenIdsArray.Add(new TFixedABIData(ABI::Int32(TokenId)));
	}
	TDynamicABIArray ABIArrayTokenIds(TokenIdsArray);

	TArray<ABIEncodeable*> AmountsArray;
	for (uint32 Amount : Amounts)
	{
		AmountsArray.Add(new TFixedABIData(ABI::Int32(Amount)));
	}
	TDynamicABIArray ABIArrayAmounts(AmountsArray);

	TArray<ABIEncodeable*> ProofArray;
	for (const FString& ProofEntry : Proof)
	{
		TArray<uint8> ProofBytes;
		ProofBytes.SetNumUninitialized(32);
		FMemory::Memcpy(ProofBytes.GetData(), TCHAR_TO_UTF8(*ProofEntry), FMath::Min(ProofEntry.Len(), 32));
		ProofArray.Add(new TFixedABIData(ProofBytes));
	}
	TDynamicABIArray ABIArrayProof(ProofArray);

	TDynamicABIData ABIData = ABI::String(Data);
	TFixedABIData ABIAddressPayment = ABI::Address(FAddress::From(PaymentToken.Mid(2, PaymentToken.Len())));
	TFixedABIData ABIAmount = ABI::UInt32(MaxTotal);


	TArray<ABIEncodeable*> Arr;
	Arr.Add(&ABIAccount);
	Arr.Add(&ABIArrayTokenIds);
	Arr.Add(&ABIArrayAmounts);
	Arr.Add(&ABIData);
	Arr.Add(&ABIAddressPayment);
	Arr.Add(&ABIAmount);
	Arr.Add(&ABIArrayProof);

	FUnsizedData EncodedData = ABI::Encode(FunctionSignature, Arr);

	TArray<TUnion<FRawTransaction, FERC20Transaction, FERC721Transaction, FERC1155Transaction, FDelayedTransaction>> Txn;
	FRawTransaction T;
	T.data = "0x" + EncodedData.ToHex();
	T.to = ContractAddress;
	T.value = "0";
    return T;
}

FRawTransaction UERC1155SaleContract::GetPaymentToken()
{
    return FRawTransaction();
}

FRawTransaction UERC1155SaleContract::GetSaleDetails()
{
    return FRawTransaction();
}
