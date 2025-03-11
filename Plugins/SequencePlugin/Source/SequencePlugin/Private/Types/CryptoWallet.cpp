// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#include "Types/CryptoWallet.h"
#include "Eth/EthTransaction.h"
#include "Bitcoin-Cryptography-Library/cpp/Ecdsa.hpp"
#include "Bitcoin-Cryptography-Library/cpp/Keccak256.hpp"
#include "Bitcoin-Cryptography-Library/cpp/Sha256Hash.hpp"
#include "Bitcoin-Cryptography-Library/cpp/Uint256.hpp"
#include "Util/HexUtility.h"
#include "Eth/Crypto.h"
#include "Bitcoin-Cryptography-Library/cpp/Sha256.hpp"
#include "Util/Log.h"

UCryptoWallet::UCryptoWallet(){}

UCryptoWallet* UCryptoWallet::Make()
{
	UCryptoWallet * Wallet = NewObject<UCryptoWallet>();
	Wallet->PrivateKey = FPrivateKey::New();
	for (int i = 0; i < 32; i++)
	{
		Wallet->PrivateKey.Arr.Get()->GetData()[i] = RandomByte();
	}
	Wallet->PublicKey = GetPublicKey(Wallet->PrivateKey);
	Wallet->Address = GetAddress(Wallet->PublicKey);
	return Wallet;
}

UCryptoWallet* UCryptoWallet::Make(FPrivateKey PrivateKey)
{
	UCryptoWallet * Wallet = NewObject<UCryptoWallet>();
	Wallet->PrivateKey = PrivateKey;
	Wallet->PublicKey = GetPublicKey(Wallet->PrivateKey);
	Wallet->Address = GetAddress(Wallet->PublicKey);
	return Wallet;
}

UCryptoWallet* UCryptoWallet::Make(const FString& PrivateKey)
{
	UCryptoWallet * Wallet = NewObject<UCryptoWallet>();
	if (PrivateKey.Len() == 64)
	{
		Wallet->PrivateKey = FPrivateKey::From(PrivateKey);
		Wallet->PublicKey = GetPublicKey(Wallet->PrivateKey);
		Wallet->Address = GetAddress(Wallet->PublicKey);
	}
	else
	{
		SEQ_LOG(Error,TEXT("PrivateKey: %s is %d Characters in length, This key needs to be 64 Hex Characters in length"),*PrivateKey, PrivateKey.Len());
	}
	return Wallet;
}

TArray<uint8_t> UCryptoWallet::BuildSigningNonce(uint8_t* MessageHash, int32 Size)
{
	TArray<uint8_t> nonce;
	FString v_0_str = "010101010101010101010101010101010101010101010101010101010101010100";
	FString k_0_str = "0000000000000000000000000000000000000000000000000000000000000000";
	uint8_t* v_0 = new uint8_t[33];//delete[]
	HexToBytes(v_0_str, v_0);
	uint8_t* k_0 = new uint8_t[32];//delete[]
	HexToBytes(k_0_str, k_0);

	TArray<uint8_t> mesgBuff;
	mesgBuff.Append(v_0, 33);//append v_0 + 00
	mesgBuff.Append(this->PrivateKey.Arr.Get()->GetData(),this->PrivateKey.GetLength());//append private key
	mesgBuff.Append(MessageHash, Size);

	const Sha256Hash k_1 = Sha256::getHmac(k_0, 32,mesgBuff.GetData(),mesgBuff.Num());
	const Sha256Hash v_1 = Sha256::getHmac(k_1.value,32,v_0,32);

	TArray<uint8_t> k_2_msg;//v_1 + b'\x01' + private_key_bytes + msg_hash
	k_2_msg.Append(v_1.value, 32);
	FString k_2_constant = "01";
	uint8_t* k_2_c_ptr = new uint8[1];//delete[]
	HexToBytes(k_2_constant,k_2_c_ptr);
	k_2_msg.Append(k_2_c_ptr,1);
	k_2_msg.Append(this->PrivateKey.Arr.Get()->GetData(), this->PrivateKey.GetLength());
	k_2_msg.Append(MessageHash, Size);

	const Sha256Hash k_2 = Sha256::getHmac(k_1.value, 32, k_2_msg.GetData(), k_2_msg.Num());
	const Sha256Hash v_2 = Sha256::getHmac(k_2.value,32,v_1.value,32);
	const Sha256Hash kb = Sha256::getHmac(k_2.value, 32, v_2.value, 32);

	nonce.Append(kb.value, 32);

	delete[] v_0;
	delete[] k_0;
	delete[] k_2_c_ptr;
	return nonce;
}

TArray<uint8> UCryptoWallet::SignMessage(TArray<uint8> MessageBytes, int32 MessageLength)
{
	TArray<uint8> sig;
	Uint256 BigR, BigS;
	uint16 RecoveryParameter;	
	FString LeadingByte = "\x19";//leading byte
	FString Payload = LeadingByte + "Ethereum Signed Message:\n";
	Payload.AppendInt(MessageLength);

	FHash256 SigningHash = FHash256::New();
	FUnsizedData EncodedSigningData = StringToUTF8(Payload);
	EncodedSigningData.Arr->Append(MessageBytes);
	Keccak256::getHash(EncodedSigningData.Ptr(), EncodedSigningData.GetLength(), SigningHash.Ptr());
	
	FString rawHash = SigningHash.ToHex();
	
	TArray<uint8_t> result_nonce = this->BuildSigningNonce(SigningHash.Ptr(),SigningHash.GetLength());

	FString result_nonce_str = BytesToHex(result_nonce.GetData(), result_nonce.Num());

	Ecdsa::sign(Uint256(this->PrivateKey.Ptr()), Sha256Hash(SigningHash.Ptr(),SigningHash.GetLength()),Uint256(result_nonce.GetData()), BigR, BigS, RecoveryParameter);

	FHash256 MyR = FHash256::New();
	FHash256 MyS = FHash256::New();
	BigR.getBigEndianBytes(MyR.Ptr());
	BigS.getBigEndianBytes(MyS.Ptr());

	FHash256 R = MyR;
	FHash256 S = MyS;

	FString RString = MyR.ToHex();
	FString SString = MyS.ToHex();

	for (int i = 0; i < 32; i++)
		sig.Add(R.Ptr()[i]);

	for (int i = 0; i < 32; i++)
		sig.Add(S.Ptr()[i]);

	sig.Add((uint8)RecoveryParameter);

	return sig;
}

TArray<uint8> UCryptoWallet::SignMessage(FString Message)
{
	TArray<uint8> sig;
	Uint256 BigR, BigS;
	uint16 RecoveryParameter;

	FString LeadingByte = "\x19";//leading byte
	FString Payload = LeadingByte + "Ethereum Signed Message:\n";
	Payload.AppendInt(Message.Len());
	Payload += Message;

	FHash256 SigningHash = FHash256::New();
	FUnsizedData EncodedSigningData = StringToUTF8(Payload);
	Keccak256::getHash(EncodedSigningData.Ptr(), EncodedSigningData.GetLength(), SigningHash.Ptr());
	
	FString rawHash = SigningHash.ToHex();
	
	TArray<uint8_t> result_nonce = this->BuildSigningNonce(SigningHash.Ptr(),SigningHash.GetLength());

	FString result_nonce_str = BytesToHex(result_nonce.GetData(), result_nonce.Num());

	Ecdsa::sign(Uint256(this->PrivateKey.Ptr()), Sha256Hash(SigningHash.Ptr(),SigningHash.GetLength()),Uint256(result_nonce.GetData()), BigR, BigS, RecoveryParameter);

	FHash256 MyR = FHash256::New();
	FHash256 MyS = FHash256::New();
	BigR.getBigEndianBytes(MyR.Ptr());
	BigS.getBigEndianBytes(MyS.Ptr());

	FHash256 R = MyR;
	FHash256 S = MyS;

	FString RString = MyR.ToHex();
	FString SString = MyS.ToHex();

	for (int i = 0; i < 32; i++)
		sig.Add(R.Ptr()[i]);

	for (int i = 0; i < 32; i++)
		sig.Add(S.Ptr()[i]);

	sig.Add((uint8)RecoveryParameter);

	return sig;
}

FString UCryptoWallet::SignMessageWithoutPrefix(const FString& Message)
{
	TArray<uint8> SigBytes = SignMessage(Message);
	FString Signature = BytesToHex(SigBytes.GetData(), SigBytes.Num()).ToLower();
	return Signature;
}

FString UCryptoWallet::SignMessageWithoutPrefix(const TArray<uint8>& MessageBytes, const int32 MessageLength)
{
	TArray<uint8> SigBytes = SignMessage(MessageBytes, MessageLength);
	FString Signature = BytesToHex(SigBytes.GetData(), SigBytes.Num()).ToLower();
	return Signature;
}

FString UCryptoWallet::SignMessageWithPrefix(const FString& Message)
{
	TArray<uint8> SigBytes = SignMessage(Message);
	FString Signature = "0x" + BytesToHex(SigBytes.GetData(), SigBytes.Num()).ToLower();
	return Signature;
}

FString UCryptoWallet::SignMessageWithPrefix(const TArray<uint8>& MessageBytes, const int32 MessageLength)
{
	TArray<uint8> SigBytes = SignMessage(MessageBytes, MessageLength);
	FString Signature = "0x" + BytesToHex(SigBytes.GetData(), SigBytes.Num()).ToLower();
	return Signature;
}

FString UCryptoWallet::GetSessionId()
{
	return "0x00" + GetWalletAddress().ToHex().ToLower();
}

FString UCryptoWallet::GetSessionHash()
{
	const FHash256 SessionHashBytes = FHash256::New();
	const FUnsizedData EncodedSigningData = StringToUTF8(GetSessionId());
	Keccak256::getHash(EncodedSigningData.Arr.Get()->GetData(), EncodedSigningData.GetLength(), SessionHashBytes.Ptr());
	return "0x" + SessionHashBytes.ToHex().ToLower();
}

FPrivateKey UCryptoWallet::GetWalletPrivateKey()
{
	return this->PrivateKey;
}

FPublicKey UCryptoWallet::GetWalletPublicKey()
{
	return this->PublicKey;
}

FAddress UCryptoWallet::GetWalletAddress()
{
	return this->Address;
}

FString UCryptoWallet::GetWalletPrivateKeyString()
{
	return BytesToHex(GetWalletPrivateKey().Ptr(), GetWalletPrivateKey().GetLength()).ToLower();
}
