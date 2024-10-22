#include "Types/ERC1155SaleContract.h"
#include "ABI/ABI.h"

UERC1155SaleContract::UERC1155SaleContract()
{
}

UERC1155SaleContract::UERC1155SaleContract(FString ContractAddress, FString PaymentToken, int32 MaxTotal, FString Data)
{
	this->ContractAddress = ContractAddress;
	this->PaymentToken = PaymentToken;
	this->MaxTotal = MaxTotal;
	this->Data = Data;
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

	FRawTransaction T;

	T.to = ContractAddress;
	T.data = "0x" + EncodedData.ToHex();
	T.value = "0";
    return T;
}

FContractCall UERC1155SaleContract::GetPaymentToken()
{
	FString FunctionSignature = "paymentToken()";
	TArray<ABIEncodeable*> Arr;
	FUnsizedData EncodedData = ABI::Encode(FunctionSignature, Arr);

	FContractCall CallData;
	CallData.Data = TOptional(EncodedData.ToHex());
	CallData.To = FAddress::From(ContractAddress);

	return CallData;
}

FContractCall UERC1155SaleContract::GetGlobalSaleDetails()
{
	FString FunctionSignature = "globalSaleDetails()";
	TArray<ABIEncodeable*> Arr;
	FUnsizedData EncodedData = ABI::Encode(FunctionSignature, Arr);

	FContractCall CallData;
	CallData.Data = TOptional(EncodedData.ToHex());
	CallData.To = FAddress::From(ContractAddress);

	return CallData;
}
