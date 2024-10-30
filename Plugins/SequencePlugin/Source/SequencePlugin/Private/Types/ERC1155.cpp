#include "Types/ERC1155.h"
#include "ABI/ABI.h"

UERC1155::UERC1155()
{
}

UERC1155::UERC1155(FString ContractAddress, FString Data)
{
	this->ContractAddress = ContractAddress;
	this->Data = Data;
}

FRawTransaction UERC1155::MakeGrantRoleTransaction(const FString& role, const FString& ToAddress)
{
	FString FunctionSignature = "grantRole(bytes32,address)";

	FString Role = role;

	TArray<uint8> ProofBytes;
	ProofBytes.SetNumUninitialized(32);
	FMemory::Memcpy(ProofBytes.GetData(), TCHAR_TO_UTF8(*Role), FMath::Min(Role.Len(), 32));

	TFixedABIData ABIProof(ProofBytes);

	FString WalletAddress = ToAddress;
	FString WalletAddressNoPrefix = WalletAddress.Mid(2, WalletAddress.Len());
	FAddress WalletAddressBytes = FAddress::From(WalletAddressNoPrefix);

	TFixedABIData ABIAccount = ABI::Address(WalletAddressBytes);

	TArray<ABIElement*> Arr;

	Arr.Add(&ABIProof);
	Arr.Add(&ABIAccount);

	FUnsizedData EncodedData = ABI::Encode(FunctionSignature, Arr);

	FRawTransaction T;
	T.data = "0x" + EncodedData.ToHex();
	T.to = ContractAddress;
	T.value = "0";

	return T;
}

FRawTransaction UERC1155::MakeSetApprovalForAllTransaction(const FString& Opperator, bool Approved)
{
	FString FunctionSignature = "setApprovalForAll(address,bool)";

	FString WalletAddress = Opperator;
	FString WalletAddressNoPrefix = WalletAddress.Mid(2, WalletAddress.Len());
	FAddress WalletAddressBytes = FAddress::From(WalletAddressNoPrefix);

	TFixedABIData ABIAccount = ABI::Address(WalletAddressBytes);
	TFixedABIData ABIApproval = ABI::Bool(Approved);


	TArray<ABIElement*> Arr;
	Arr.Add(&ABIAccount);
	Arr.Add(&ABIApproval);

	FUnsizedData EncodedData = ABI::Encode(FunctionSignature, Arr);
	FRawTransaction T;
	T.data = "0x" + EncodedData.ToHex();
	T.to = ContractAddress;
	T.value = "0";

	return T;
}


FRawTransaction UERC1155::MakeMintTransaction(const FString& ToAddress, const int32 TokenId, const int32 Amount)
{
	FString FunctionSignature = "mint(address,uint256,unit256,bytes)";

	FString WalletAddress = ToAddress;
	FString WalletAddressNoPrefix = WalletAddress.Mid(2, WalletAddress.Len());
	FAddress WalletAddressBytes = FAddress::From(WalletAddressNoPrefix);

	TFixedABIData ABIAccount = ABI::Address(WalletAddressBytes);

	TFixedABIData ABITokenId = ABI::Int32(TokenId);
	TFixedABIData ABIAmount = ABI::Int32(Amount);
	TDynamicABIData ABIData = ABI::String(Data);

	TArray<ABIElement*> Arr;
	Arr.Add(&ABIAccount);
	Arr.Add(&ABITokenId);
	Arr.Add(&ABIAmount);
	Arr.Add(&ABIData);

	FUnsizedData EncodedData = ABI::Encode(FunctionSignature, Arr);

	FRawTransaction T;
	T.data = "0x" + EncodedData.ToHex();
	T.to = ContractAddress;
	T.value = "0";

	return T;
}

FRawTransaction UERC1155::MakeBatchMintTransaction(const FString& ToAddress, const TArray<int32>& TokenIds, const TArray<int32>& Amounts)
{
	FString FunctionSignature = "mint(address,uint256[],uint256[],bytes)";

	FString WalletAddress = ToAddress;
	FString WalletAddressNoPrefix = WalletAddress.Mid(2, WalletAddress.Len());
	FAddress WalletAddressBytes = FAddress::From(WalletAddressNoPrefix);

	TFixedABIData ABIAccount = ABI::Address(WalletAddressBytes);

	TArray<TSharedPtr<ABIElement>> TokenIdsArray;
	for (uint32 TokenId : TokenIds)
	{
		const TFixedABIData* Data = new TFixedABIData(ABI::Int32(TokenId));
		TokenIdsArray.Add(MakeShared<TFixedABIData>(*Data));
	}
	TDynamicABIArray ABIArrayTokenIds(TokenIdsArray);

	TArray<TSharedPtr<ABIElement>> AmountsArray;
	for (uint32 Amount : Amounts)
	{
		const TFixedABIData* Data = new TFixedABIData(ABI::Int32(Amount));
		AmountsArray.Add(MakeShared<TFixedABIData>(*Data));
	}
	
	TDynamicABIArray ABIArrayAmounts(AmountsArray);
	TDynamicABIData ABIData = ABI::String(Data);

	TArray<ABIElement*> Arr;
	Arr.Add(&ABIAccount);
	Arr.Add(&ABIArrayTokenIds);
	Arr.Add(&ABIArrayAmounts);
	Arr.Add(&ABIData);

	FUnsizedData EncodedData = ABI::Encode(FunctionSignature, Arr);

	FRawTransaction T;
	T.data = "0x" + EncodedData.ToHex();
	T.to = ContractAddress;
	T.value = "0";

	return T;
}

FRawTransaction UERC1155::MakeBurnTransaction(int32 TokenId, int32 Amount)
{
	FString FunctionSignature = "burn(uint256,uint256)";

	TFixedABIData ABITokenId = ABI::Int32(TokenId);

	TFixedABIData ABIFixedAmount = ABI::Int32(Amount);

	TArray<ABIElement*> Arr;

	Arr.Add(&ABITokenId);
	Arr.Add(&ABIFixedAmount);

	FUnsizedData EncodedData = ABI::Encode(FunctionSignature, Arr);

	FRawTransaction T;
	T.data = "0x" + EncodedData.ToHex();
	T.to = ContractAddress;
	T.value = "0";

	return T;
}

FRawTransaction UERC1155::MakeBatchBurnTransaction(const TArray<int32>& TokenIds, const TArray<int32>& Amounts)
{
	FString FunctionSignature = "burn(uint256[],uint256[])";

	TArray<TSharedPtr<ABIElement>> TokenIdsArray;
	for (uint32 TokenId : TokenIds)
	{
		const TFixedABIData* Data = new TFixedABIData(ABI::Int32(TokenId));
		TokenIdsArray.Add(MakeShared<TFixedABIData>(*Data));
	}
	TDynamicABIArray ABIArrayTokenIds(TokenIdsArray);

	TArray<TSharedPtr<ABIElement>> AmountsArray;
	for (uint32 Amount : Amounts)
	{
		const TFixedABIData* Data = new TFixedABIData(ABI::Int32(Amount));
		AmountsArray.Add(MakeShared<TFixedABIData>(*Data));
	}
	TDynamicABIArray ABIArrayAmounts(AmountsArray);

	TArray<ABIElement*> Arr;
	Arr.Add(&ABIArrayTokenIds);
	Arr.Add(&ABIArrayAmounts);

	FUnsizedData EncodedData = ABI::Encode(FunctionSignature, Arr);

	FRawTransaction T;
	T.data = "0x" + EncodedData.ToHex();
	T.to = ContractAddress;
	T.value = "0";

	return T;
}