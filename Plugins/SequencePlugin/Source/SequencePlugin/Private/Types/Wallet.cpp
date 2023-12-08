#include "Types/Wallet.h"
#include "Eth/EthTransaction.h"
#include "Bitcoin-Cryptography-Library/cpp/Ecdsa.hpp"
#include "Bitcoin-Cryptography-Library/cpp/Keccak256.hpp"
#include "Bitcoin-Cryptography-Library/cpp/Sha256Hash.hpp"
#include "Bitcoin-Cryptography-Library/cpp/Uint256.hpp"
#include "Util/HexUtility.h"

FWallet::FWallet()
{
	for (int i = 0; i < 32; i++)
	{
		this->PrivateKey.Arr[i] = (uint8)FMath::RandRange(MIN_int32, MAX_int32);
	}
	this->PublicKey = GetPublicKey(this->PrivateKey);
	this->Address = GetAddress(this->PublicKey);
}

FWallet::FWallet(const FPrivateKey PrivateKey) : PrivateKey(PrivateKey)
{
	this->PublicKey = GetPublicKey(this->PrivateKey);
	this->Address = GetAddress(this->PublicKey);
}

FWallet::FWallet(const FString& PrivateKey)
{
	if (PrivateKey.Len() == 64)
	{
		this->PrivateKey = FPrivateKey::From(PrivateKey);
		this->PublicKey = GetPublicKey(this->PrivateKey);
		this->Address = GetAddress(this->PublicKey);
	}
}

TArray<uint8> FWallet::SignMessage(FString message)
{
	FUnsizedData MesgData = StringToUTF8(message);
	FHash256 MesgHash = FHash256::New();
	Keccak256::getHash(MesgData.Arr, MesgData.GetLength(), MesgHash.Arr);

	FString InternalMessage = "\x19";
	InternalMessage += "Ethereum Signed Message:\n32" + BytesToString(MesgHash.Arr,MesgHash.GetLength());

	TArray<uint8> sig;
	Uint256 BigR, BigS;
	uint16 RecoveryParameter;
	FUnsizedData EncodedSigningData = StringToUTF8(InternalMessage);
	FHash256 MyR = FHash256::New();
	FHash256 MyS = FHash256::New();
	FHash256 MyY = FHash256::New();
	FHash256 SigningHash = FHash256::New();

	Keccak256::getHash(EncodedSigningData.Arr, EncodedSigningData.GetLength(), SigningHash.Arr);

	if (!Ecdsa::signWithHmacNonce(Uint256(this->PrivateKey.Arr), Sha256Hash(SigningHash.Arr, FHash256::Size), BigR, BigS, RecoveryParameter))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Error Signing Message in FWallet]"));
		return sig;//return something with nothing in it
	}

	BigR.getBigEndianBytes(MyR.Arr);
	BigS.getBigEndianBytes(MyS.Arr);

	FHash256 R = MyR;
	FHash256 S = MyS;

	//RSV are slapped together to form a 65 byte signature! where R & S are 32 bytes, and V is 1 byte
	for (int i = 0; i < 32; i++)
	{
		sig.Add(R.Arr[i]);
	}

	for (int i = 0; i < 32; i++)
	{
		sig.Add(S.Arr[i]);
	}

	sig.Add((uint8)RecoveryParameter);
	return sig;
}

FPrivateKey FWallet::GetWalletPrivateKey()
{
	return this->PrivateKey;
}

FPublicKey FWallet::GetWalletPublicKey()
{
	return this->PublicKey;
}

FAddress FWallet::GetWalletAddress()
{
	return this->Address;
}