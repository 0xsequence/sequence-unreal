#include "Types/ERC1155SaleContract.h"
#include "ABI/ABI.h"
#include "Util/Log.h"

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

void UERC1155SaleContract::Initialize(FString _ContractAddress, FString _PaymentToken, int32 _MaxTotal, FString _Data)
{
	this->ContractAddress = _ContractAddress;
	this->PaymentToken = _PaymentToken;
	this->MaxTotal = _MaxTotal;
	this->Data = _Data;
}


FRawTransaction UERC1155SaleContract::MakePurchaseTransaction(const FString& ToAddress, const TArray<int32>& TokenIds, const TArray<int32>& Amounts, const TArray<FString>& Proof)
{
	FString FunctionSignature = "mint(address,uint256[],uint256[],bytes,address,uint256,bytes32[])";
	
	FString WalletAddress = ToAddress;
	FString WalletAddressNoPrefix = WalletAddress.Mid(2, WalletAddress.Len());
	FAddress WalletAddressBytes = FAddress::From(WalletAddressNoPrefix);

	TSharedPtr<TFixedABIData> ABIAccount = MakeShared<TFixedABIData>(ABI::Address(WalletAddressBytes));

	TArray<TSharedPtr<ABIElement>> TokenIdsArray;
	for (uint32 TokenId : TokenIds)
	{
		const TFixedABIData* TokenIdData = new TFixedABIData(ABI::Int32(TokenId));
		TokenIdsArray.Add(MakeShared<TFixedABIData>(*TokenIdData));
	}
	TSharedPtr<TDynamicABIArray> ABIArrayTokenIds = MakeShared<TDynamicABIArray>(TokenIdsArray);

	TArray<TSharedPtr<ABIElement>> AmountsArray;
	for (uint32 Amount : Amounts)
	{
		const TFixedABIData* AmountData = new TFixedABIData(ABI::Int32(Amount));
		AmountsArray.Add(MakeShared<TFixedABIData>(*AmountData));
	}
	TSharedPtr<TDynamicABIArray> ABIArrayAmounts = MakeShared<TDynamicABIArray>(AmountsArray);

	TArray<TSharedPtr<ABIElement>> ProofArray;
	for (const FString& ProofEntry : Proof)
	{
		TArray<uint8> ProofBytes;
		ProofBytes.SetNumUninitialized(32);
		FMemory::Memcpy(ProofBytes.GetData(), TCHAR_TO_UTF8(*ProofEntry), FMath::Min(ProofEntry.Len(), 32));

		const TFixedABIData* ProofData = new TFixedABIData(ProofBytes);
		ProofArray.Add(MakeShared<TFixedABIData>(*ProofData));
	}
	TSharedPtr<TDynamicABIArray> ABIArrayProof = MakeShared<TDynamicABIArray>(ProofArray);

	TSharedPtr<TDynamicABIData> ABIData = MakeShared<TDynamicABIData>(ABI::String(Data));
	TSharedPtr<TFixedABIData> ABIAddressPayment = MakeShared<TFixedABIData>(ABI::Address(FAddress::From(PaymentToken.Mid(2, PaymentToken.Len()))));
	TSharedPtr<TFixedABIData> ABIAmount = MakeShared<TFixedABIData>(ABI::UInt32(MaxTotal));


	TArray<TSharedPtr<ABIElement>> Arr;
	Arr.Add(ABIAccount);
	Arr.Add(ABIArrayTokenIds);
	Arr.Add(ABIArrayAmounts);
	Arr.Add(ABIData);
	Arr.Add(ABIAddressPayment);
	Arr.Add(ABIAmount);
	Arr.Add(ABIArrayProof);

	TOptional<FUnsizedData> EncodedData = ABI::Encode(FunctionSignature, Arr);
	FRawTransaction T;
	
	if (!EncodedData.IsSet())
	{
		SEQ_LOG(Display, TEXT("Encoded data is invalid"));
		return T;
	}

	T.to = ContractAddress;
	T.data = "0x" + EncodedData.GetValue().ToHex();
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
	CallData.To = FAddress::From(ContractAddress.Mid(2, ContractAddress.Len()));

	return CallData;
}

FContractCall UERC1155SaleContract::GetGlobalSaleDetails()
{
	FString FunctionSignature = "globalSaleDetails()";
	TArray<ABIElement*> Arr;
	FUnsizedData EncodedData = ABI::Encode(FunctionSignature, Arr);

	FContractCall CallData;
	CallData.Data = TOptional(EncodedData.ToHex());
	CallData.To = FAddress::From(ContractAddress.Mid(2, ContractAddress.Len()));

	return CallData;
}

FContractCall UERC1155SaleContract::GetTokenSaleDetails(long TokenID)
{
	FString FunctionSignature = "tokenSaleDetails(uint256)";
	TFixedABIData* TokenIDData = new TFixedABIData(ABI::Int32(TokenID));

	TArray<ABIElement*> Arr;
	Arr.Add(TokenIDData);
	FUnsizedData EncodedData = ABI::Encode(FunctionSignature, Arr);

	FContractCall CallData;
	CallData.Data = TOptional(EncodedData.ToHex());
	CallData.To = FAddress::From(ContractAddress.Mid(2, ContractAddress.Len()));

	return CallData;
}
