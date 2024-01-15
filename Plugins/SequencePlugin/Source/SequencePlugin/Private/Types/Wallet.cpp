#include "Types/Wallet.h"
#include "Eth/EthTransaction.h"
#include "Bitcoin-Cryptography-Library/cpp/Ecdsa.hpp"
#include "Bitcoin-Cryptography-Library/cpp/Keccak256.hpp"
#include "Bitcoin-Cryptography-Library/cpp/Sha256Hash.hpp"
#include "Bitcoin-Cryptography-Library/cpp/Uint256.hpp"
#include "Util/HexUtility.h"
#include "Bitcoin-Cryptography-Library/cpp/Sha256.hpp"

FWallet::FWallet()
{
	this->PrivateKey = FPrivateKey::New();
	for (int i = 0; i < 32; i++)
	{
		this->PrivateKey.Arr[i] = RandomByte();
	}
	this->PublicKey = GetPublicKey(this->PrivateKey);
	this->Address = GetAddress(this->PublicKey);
}

FWallet::~FWallet()
{
	if (this->PrivateKey.Arr)
	{
		delete[] this->PrivateKey.Arr;
	}

	if (this->PublicKey.Arr)
	{
		delete[] this->PublicKey.Arr;
	}

	if (this->Address.Arr)
	{
		delete[] this->Address.Arr;
	}
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

TArray<uint8_t> FWallet::BuildSigningNonce(uint8_t* messageHash, int32 size)
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
	mesgBuff.Append(this->PrivateKey.Arr,this->PrivateKey.GetLength());//append private key
	mesgBuff.Append(messageHash, size);

	const Sha256Hash k_1 = Sha256::getHmac(k_0, 32,mesgBuff.GetData(),mesgBuff.Num());
	const Sha256Hash v_1 = Sha256::getHmac(k_1.value,32,v_0,32);

	TArray<uint8_t> k_2_msg;//v_1 + b'\x01' + private_key_bytes + msg_hash
	k_2_msg.Append(v_1.value, 32);
	FString k_2_constant = "01";
	uint8_t* k_2_c_ptr = new uint8[1];//delete[]
	HexToBytes(k_2_constant,k_2_c_ptr);
	k_2_msg.Append(k_2_c_ptr,1);
	k_2_msg.Append(this->PrivateKey.Arr, this->PrivateKey.GetLength());
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

TArray<uint8> FWallet::SignMessage(FString message)
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
	Keccak256::getHash(EncodedSigningData.Arr, EncodedSigningData.GetLength(), SigningHash.Arr);
	
	FString rawHash = SigningHash.ToHex();
	
	TArray<uint8_t> result_nonce = this->BuildSigningNonce(SigningHash.Arr,SigningHash.GetLength());

	FString result_nonce_str = BytesToHex(result_nonce.GetData(), result_nonce.Num());

	Ecdsa::sign(Uint256(this->PrivateKey.Arr), Sha256Hash(SigningHash.Arr,SigningHash.GetLength()),Uint256(result_nonce.GetData()), BigR, BigS, RecoveryParameter);

	FHash256 MyR = FHash256::New();
	FHash256 MyS = FHash256::New();
	BigR.getBigEndianBytes(MyR.Arr);
	BigS.getBigEndianBytes(MyS.Arr);

	FHash256 R = MyR;
	FHash256 S = MyS;

	FString RString = MyR.ToHex();
	FString SString = MyS.ToHex();

	for (int i = 0; i < 32; i++)
		sig.Add(R.Arr[i]);

	for (int i = 0; i < 32; i++)
		sig.Add(S.Arr[i]);

	sig.Add((uint8)RecoveryParameter);

	//cleanup
	delete[] EncodedSigningData.Arr;
	delete[] SigningHash.Arr;
	delete[] MyR.Arr;
	delete[] MyS.Arr;

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