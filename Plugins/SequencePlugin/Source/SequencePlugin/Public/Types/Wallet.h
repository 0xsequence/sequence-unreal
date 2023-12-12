#pragma once
#include "BinaryData.h"
#include "Eth/EthTransaction.h"
#include "Eth/Crypto.h"

class FWallet
{
private:
	FPrivateKey PrivateKey;
	FPublicKey PublicKey;
	FAddress Address;
public:
	/*
	* Generates a random wallet
	*/
	FWallet();

	/*
	* Generates a wallet based on a given private key
	*/
	FWallet(FPrivateKey PrivateKey);

	FWallet(const FString& PrivateKey);

	TArray<uint8> SignMessage(FString message);

	FPrivateKey GetWalletPrivateKey();
	FPublicKey GetWalletPublicKey();
	FAddress GetWalletAddress();

private: TArray<uint8_t> BuildSigningNonce(uint8_t* messageHash, int32 size);
private: TArray<uint8_t> FStringToHexCharList(FString in);
};
