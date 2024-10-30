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

	TArray<TSharedPtr<ABIElement>> TokenIdsArray;
	for (uint32 TokenId : TokenIds)
	{
		const TFixedABIData* TokenIdData = new TFixedABIData(ABI::Int32(TokenId));
		TokenIdsArray.Add(MakeShared<TFixedABIData>(*TokenIdData));
	}
	TDynamicABIArray ABIArrayTokenIds(TokenIdsArray);

	TArray<TSharedPtr<ABIElement>> AmountsArray;
	for (uint32 Amount : Amounts)
	{
		const TFixedABIData* AmountData = new TFixedABIData(ABI::Int32(Amount));
		AmountsArray.Add(MakeShared<TFixedABIData>(*AmountData));
	}
	TDynamicABIArray ABIArrayAmounts(AmountsArray);

	TArray<TSharedPtr<ABIElement>> ProofArray;
	for (const FString& ProofEntry : Proof)
	{
		TArray<uint8> ProofBytes;
		ProofBytes.SetNumUninitialized(32);
		FMemory::Memcpy(ProofBytes.GetData(), TCHAR_TO_UTF8(*ProofEntry), FMath::Min(ProofEntry.Len(), 32));

		const TFixedABIData* ProofData = new TFixedABIData(ProofBytes);
		ProofArray.Add(MakeShared<TFixedABIData>(*ProofData));
	}
	TDynamicABIArray ABIArrayProof(ProofArray);

	TDynamicABIData ABIData = ABI::String(Data);
	TFixedABIData ABIAddressPayment = ABI::Address(FAddress::From(PaymentToken.Mid(2, PaymentToken.Len())));
	TFixedABIData ABIAmount = ABI::UInt32(MaxTotal);


	TArray<ABIElement*> Arr;
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
	TArray<ABIElement*> Arr;
	FUnsizedData EncodedData = ABI::Encode(FunctionSignature, Arr);

	FContractCall CallData;
	CallData.Data = TOptional(EncodedData.ToHex());
	CallData.To = FAddress::From(ContractAddress);

	return CallData;
}

FContractCall UERC1155SaleContract::GetGlobalSaleDetails()
{
	FString FunctionSignature = "globalSaleDetails()";
	TArray<ABIElement*> Arr;
	FUnsizedData EncodedData = ABI::Encode(FunctionSignature, Arr);

	FContractCall CallData;
	CallData.Data = TOptional(EncodedData.ToHex());
	CallData.To = FAddress::From(ContractAddress);

	return CallData;
}
