#include "Types/ERC20.h"
#include "ABI/ABI.h"

UERC20::UERC20()
{
}

UERC20* UERC20::Create(const FString& ContractAddress)
{
	UERC20* ERC20 = NewObject<UERC20>();
	ERC20->Init(ContractAddress);
	return ERC20;
}

void UERC20::Init(const FString& InContractAddress)
{
	this->ContractAddress = InContractAddress;
}

FRawTransaction UERC20::MakeGrantRoleTransaction(const FString& role,const FString& ToAddress)
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

	T.to = ContractAddress;
	T.data = "0x" + EncodedData.ToHex();
	T.value = "0";

	return T;
}

FRawTransaction UERC20::MakeApproveTransaction(const FString& Spender, const int32 Amount)
{
	FString FunctionSignature = "approve(address,uint256)";

	FString WalletAddress = Spender;
	FString WalletAddressNoPrefix = WalletAddress.Mid(2, WalletAddress.Len());
	FAddress WalletAddressBytes = FAddress::From(WalletAddressNoPrefix);

	TFixedABIData ABIAccount = ABI::Address(WalletAddressBytes);

	TFixedABIData ABITokenId = ABI::Int32(Amount);

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

FRawTransaction UERC20::MakeMintTransaction(const FString& ToAddress, const int32 Amount)
{
	FString FunctionSignature = "mint(address,uint256)";

	FString WalletAddress = ToAddress;
	FString WalletAddressNoPrefix = WalletAddress.Mid(2, WalletAddress.Len());
	FAddress WalletAddressBytes = FAddress::From(WalletAddressNoPrefix);

	TFixedABIData ABIAccount = ABI::Address(WalletAddressBytes);

	TFixedABIData ABIAmount = ABI::Int32(Amount);

	TArray<ABIElement*> Arr;
	Arr.Add(&ABIAccount);
	Arr.Add(&ABIAmount);

	FUnsizedData EncodedData = ABI::Encode(FunctionSignature, Arr);

	FRawTransaction T;

	T.to = ContractAddress;
	T.data = "0x" + EncodedData.ToHex();
	T.value = "0";

	return T;
}

FRawTransaction UERC20::MakeBurnTransaction(const int32 Amount)
{
	FString FunctionSignature = "burn(uint256)";

	TFixedABIData ABIAmount = ABI::Int32(Amount);

	TArray<ABIElement*> Arr;
	Arr.Add(&ABIAmount);

	FUnsizedData EncodedData = ABI::Encode(FunctionSignature, Arr);

	FRawTransaction T;

	T.to = ContractAddress;
	T.data = "0x" + EncodedData.ToHex();
	T.value = "0";

	return T;
}
