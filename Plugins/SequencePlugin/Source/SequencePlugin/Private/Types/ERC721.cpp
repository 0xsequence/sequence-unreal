#include "Types/ERC721.h"
#include "ABI/ABI.h"

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

	TArray<ABIEncodeable*> Arr;

	Arr.Add(&ABIProof);
	Arr.Add(&ABIAccount);

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

	TArray<ABIEncodeable*> Arr;
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

	TArray<ABIEncodeable*> Arr;
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

	TArray<ABIEncodeable*> TokenIdsArray;
	for (uint32 TokenId : TokenIds)
	{
		TokenIdsArray.Add(new TFixedABIData(ABI::Int32(TokenId)));
	}
	TDynamicABIArray ABIArrayTokenIds(TokenIdsArray);

	TArray<ABIEncodeable*> Arr;
	Arr.Add(&ABIArrayTokenIds);

	FUnsizedData EncodedData = ABI::Encode(FunctionSignature, Arr);

	FRawTransaction T;
	T.data = "0x" + EncodedData.ToHex();
	T.to = ContractAddress;
	T.value = "0";

	return T;
}
