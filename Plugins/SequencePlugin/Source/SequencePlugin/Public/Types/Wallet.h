// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once
#include "BinaryData.h"
#include "Wallet.generated.h"

UCLASS()
class SEQUENCEPLUGIN_API UWallet : public UObject
{
	GENERATED_BODY()
private:
	FPrivateKey PrivateKey = FPrivateKey::New();
	FPublicKey PublicKey = FPublicKey::New();
	FAddress Address = FAddress::New();
public:
	/*
	* Generates a random wallet
	*/
	UWallet();

	static UWallet* Make();
	static UWallet* Make(FPrivateKey PrivateKey);
	static UWallet* Make(const FString& PrivateKey);

	TArray<uint8> SignMessage(TArray<uint8> MessageBytes,int32 MessageLength);
	
	/*
	* Signs the given message
	*/
	TArray<uint8> SignMessage(FString Message);
	
	FString SignMessageWithoutPrefix(const FString& Message);
	
	FString SignMessageWithoutPrefix(const TArray<uint8>& MessageBytes,int32 MessageLength);
	
	FString SignMessageWithPrefix(const FString& Message);
	
	FString SignMessageWithPrefix(const TArray<uint8>& MessageBytes,int32 MessageLength);

	FString GetSessionId();

	FString GetSessionHash();
	
	//Accessors//
	FPrivateKey GetWalletPrivateKey();
	FPublicKey GetWalletPublicKey();
	FAddress GetWalletAddress();

	FString GetWalletPrivateKeyString();
	//Accessors//

	//Used to build out the nonce used during the signing process
private: TArray<uint8_t> BuildSigningNonce(uint8_t* MessageHash, int32 Size);
};