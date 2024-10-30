#include "Types/ERC721.h"
#include "ABI/ABI.h"

UERC721::UERC721()
{
}

UERC721::UERC721(FString ContractAddress)
{
	this->ContractAddress = ContractAddress;
}

FRawTransaction UERC721::MakeGrantRoleTransaction(const FString& role, const FString& ToAddress)
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

FRawTransaction UERC721::MakeApproveTransaction(const FString& ToAddress, const int32 TokenId)
{
	FString FunctionSignature = "approve(address,uint256)";

	FString WalletAddress = ToAddress;
	FString WalletAddressNoPrefix = WalletAddress.Mid(2, WalletAddress.Len());
	FAddress WalletAddressBytes = FAddress::From(WalletAddressNoPrefix);

	TFixedABIData ABIAccount = ABI::Address(WalletAddressBytes);

	TFixedABIData ABITokenId = ABI::Int32(TokenId);

	TArray<ABIElement*> Arr;
	Arr.Add(&ABIAccount);
	Arr.Add(&ABITokenId);

	FUnsizedData EncodedData = ABI::Encode(FunctionSignature, Arr);

	FRawTransaction T;
	T.data = "0x" + EncodedData.ToHex();
	T.to = ContractAddress;
	T.value = "0";

	return T;
}

FRawTransaction UERC721::MakeMintTransaction(const FString& ToAddress, const int32 TokenId)
{
	FString FunctionSignature = "mint(address,uint256)";

	FString WalletAddress = ToAddress;
	FString WalletAddressNoPrefix = WalletAddress.Mid(2, WalletAddress.Len());
	FAddress WalletAddressBytes = FAddress::From(WalletAddressNoPrefix);

	TFixedABIData ABIAccount = ABI::Address(WalletAddressBytes);

	TFixedABIData ABITokenId = ABI::Int32(TokenId);

	TArray<ABIElement*> Arr;
	Arr.Add(&ABIAccount);
	Arr.Add(&ABITokenId);

	FUnsizedData EncodedData = ABI::Encode(FunctionSignature, Arr);

	FRawTransaction T;
	T.data = "0x" + EncodedData.ToHex();
	T.to = ContractAddress;
	T.value = "0";

	return T;
}

FRawTransaction UERC721::MakeBurnTransaction(const int32 TokenId)
{
	FString FunctionSignature = "burn(uint256)";

	TFixedABIData ABITokenId = ABI::Int32(TokenId);

	TArray<ABIElement*> Arr;
	Arr.Add(&ABITokenId);

	FUnsizedData EncodedData = ABI::Encode(FunctionSignature, Arr);


	FRawTransaction T;
	T.data = "0x" + EncodedData.ToHex();
	T.to = ContractAddress;
	T.value = "0";

	return T;
}

FRawTransaction UERC721::MakeBatchBurnTransaction(const TArray<int32>& TokenIds)
{
	FString FunctionSignature = "burn(uint256[])";

	TArray<TSharedPtr<ABIElement>> TokenIdsArray;
	for (uint32 TokenId : TokenIds)
	{
		const TFixedABIData* TokenIdData = new TFixedABIData(ABI::Int32(TokenId));
		TokenIdsArray.Add(MakeShared<TFixedABIData>(*TokenIdData));
	}
	TDynamicABIArray ABIArrayTokenIds(TokenIdsArray);

	TArray<ABIElement*> Arr;
	Arr.Add(&ABIArrayTokenIds);

	FUnsizedData EncodedData = ABI::Encode(FunctionSignature, Arr);

	FRawTransaction T;
	T.data = "0x" + EncodedData.ToHex();
	T.to = ContractAddress;
	T.value = "0";

	return T;
}
