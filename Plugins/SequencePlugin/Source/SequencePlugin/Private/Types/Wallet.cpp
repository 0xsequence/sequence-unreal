#include "Types/Wallet.h"
#include "Eth/EthTransaction.h"
#include "Bitcoin-Cryptography-Library/cpp/Ecdsa.hpp"
#include "Bitcoin-Cryptography-Library/cpp/Keccak256.hpp"
#include "Bitcoin-Cryptography-Library/cpp/Sha256Hash.hpp"
#include "Bitcoin-Cryptography-Library/cpp/Uint256.hpp"
#include "Util/HexUtility.h"
#include "Eth/Crypto.h"
#include "Bitcoin-Cryptography-Library/cpp/Sha256.hpp"

UWallet::UWallet(){}

UWallet* UWallet::Make()
{
	UWallet * Wallet = NewObject<UWallet>();
	Wallet->PrivateKey = FPrivateKey::New();
	for (int i = 0; i < 32; i++)
	{
		Wallet->PrivateKey.Arr.Get()->GetData()[i] = RandomByte();
	}
	Wallet->PublicKey = GetPublicKey(Wallet->PrivateKey);
	Wallet->Address = GetAddress(Wallet->PublicKey);
	return Wallet;
}

UWallet* UWallet::Make(FPrivateKey PrivateKey)
{
	UWallet * Wallet = NewObject<UWallet>();
	Wallet->PrivateKey = PrivateKey;
	Wallet->PublicKey = GetPublicKey(Wallet->PrivateKey);
	Wallet->Address = GetAddress(Wallet->PublicKey);
	return Wallet;
}

UWallet* UWallet::Make(const FString& PrivateKey)
{
	UWallet * Wallet = NewObject<UWallet>();
	if (PrivateKey.Len() == 64)
	{
		Wallet->PrivateKey = FPrivateKey::From(PrivateKey);
		Wallet->PublicKey = GetPublicKey(Wallet->PrivateKey);
		Wallet->Address = GetAddress(Wallet->PublicKey);
	}
	return Wallet;
}

TArray<uint8_t> UWallet::BuildSigningNonce(uint8_t* messageHash, int32 size)
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
	mesgBuff.Append(messageHash, size);

	const Sha256Hash k_1 = Sha256::getHmac(k_0, 32,mesgBuff.GetData(),mesgBuff.Num());
	const Sha256Hash v_1 = Sha256::getHmac(k_1.value,32,v_0,32);

	TArray<uint8_t> k_2_msg;//v_1 + b'\x01' + private_key_bytes + msg_hash
	k_2_msg.Append(v_1.value, 32);
	FString k_2_constant = "01";
	uint8_t* k_2_c_ptr = new uint8[1];//delete[]
	HexToBytes(k_2_constant,k_2_c_ptr);
	k_2_msg.Append(k_2_c_ptr,1);
	k_2_msg.Append(this->PrivateKey.Arr.Get()->GetData(), this->PrivateKey.GetLength());
	k_2_msg.Append(messageHash, size);

	const Sha256Hash k_2 = Sha256::getHmac(k_1.value, 32, k_2_msg.GetData(), k_2_msg.Num());
	const Sha256Hash v_2 = Sha256::getHmac(k_2.value,32,v_1.value,32);
	const Sha256Hash kb = Sha256::getHmac(k_2.value, 32, v_2.value, 32);

	nonce.Append(kb.value, 32);

	delete[] v_0;
	delete[] k_0;
	delete[] k_2_c_ptr;
	return nonce;
}

TArray<uint8> UWallet::SignMessage(TArray<uint8> messageBytes, int32 messageLength)
{
	TArray<uint8> sig;
	Uint256 BigR, BigS;
	uint16 RecoveryParameter;	
	FString LeadingByte = "\x19";//leading byte
	FString Payload = LeadingByte + "Ethereum Signed Message:\n";
	Payload.AppendInt(messageLength);

	FHash256 SigningHash = FHash256::New();
	FUnsizedData EncodedSigningData = StringToUTF8(Payload);
	EncodedSigningData.Arr->Append(messageBytes);
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

TArray<uint8> UWallet::SignMessage(FString message)
{
	TArray<uint8> sig;
	Uint256 BigR, BigS;
	uint16 RecoveryParameter;

	FString LeadingByte = "\x19";//leading byte
	FString Payload = LeadingByte + "Ethereum Signed Message:\n";
	Payload.AppendInt(message.Len());
	Payload += message;

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

FPrivateKey UWallet::GetWalletPrivateKey()
{
	return this->PrivateKey;
}

FPublicKey UWallet::GetWalletPublicKey()
{
	return this->PublicKey;
}

FAddress UWallet::GetWalletAddress()
{
	return this->Address;
}