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

// "\x19 Ethereum Signed Message:\n" + message.Len() + BytesToString(MesgHash.Arr, MesgHash.GetLength());
// 12B1013E3EEBC2C6D11C4371D8919E87D54BBC572F14D74112AFF3F0224D361430BDB5F04554749B4CD4F5EACCF46A4F0966E538BC597726611F2D35DA9E711301

// "\x19" + "Ethereum Signed Message:\n" + message.Len() + BytesToString(MesgHash.Arr, MesgHash.GetLength());
// 7B893AAFCDD55830A641B209A803F2599B6D40A0A9F25441C45AE4B58EEEE43632F6D11218548D0ADF0D4EEFAFE6116AF3346AA75CC8316CD22BDC99A2662AD900

// "\x19" + "Ethereum Signed Message:\n32" + "BytesToString(MesgHash.Arr, MesgHash.GetLength())"
// 343E2C893CC66979AD7E9BF15C25C32C351FF4F0F8F03CFD4D9664286D61487B67CAC33B23C211A92477FC757D9740077D54873F654B67F87F1A48DF47E7FB3901

TArray<uint8_t> FWallet::FStringToHexCharList(FString in)
{
	TArray<uint8_t> result;
	for (int i = 0; i < in.Len(); i++)
		result.Add((unsigned char)in[i]);
	return result;
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
	//FString k_1_str = BytesToHex(k_1.value, 32);
	//UE_LOG(LogTemp, Display, TEXT("k1: %s"), *k_1_str);

	const Sha256Hash v_1 = Sha256::getHmac(k_1.value,32,v_0,32);
	//FString v_1_str = BytesToHex(v_1.value, 32);
	//UE_LOG(LogTemp, Display, TEXT("v1: %s"), *v_1_str);

	TArray<uint8_t> k_2_msg;//v_1 + b'\x01' + private_key_bytes + msg_hash
	k_2_msg.Append(v_1.value, 32);
	FString k_2_constant = "01";
	uint8_t* k_2_c_ptr = new uint8[1];//delete[]
	HexToBytes(k_2_constant,k_2_c_ptr);
	k_2_msg.Append(k_2_c_ptr,1);
	k_2_msg.Append(this->PrivateKey.Arr, this->PrivateKey.GetLength());
	k_2_msg.Append(messageHash, size);

	const Sha256Hash k_2 = Sha256::getHmac(k_1.value, 32, k_2_msg.GetData(), k_2_msg.Num());
	//FString k_2_str = BytesToHex(k_2.value, 32);
	//UE_LOG(LogTemp, Display, TEXT("k2: %s"), *k_2_str);

	const Sha256Hash v_2 = Sha256::getHmac(k_2.value,32,v_1.value,32);
	//FString v_2_str = BytesToHex(v_2.value, 32);
	//UE_LOG(LogTemp, Display, TEXT("v2: %s"), *v_2_str);

	const Sha256Hash kb = Sha256::getHmac(k_2.value, 32, v_2.value, 32);
	//FString kb_str = BytesToHex(kb.value, 32);
	//UE_LOG(LogTemp, Display, TEXT("kb: %s"), *kb_str);

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

	FHash256 SigningHash = FHash256::New();
	FUnsizedData EncodedSigningData = StringToUTF8(message);
	Keccak256::getHash(EncodedSigningData.Arr, EncodedSigningData.GetLength(), SigningHash.Arr);
	
	FString rawHash = SigningHash.ToHex();
	//UE_LOG(LogTemp, Display, TEXT("Hash: %s"), *rawHash);
	
	TArray<uint8_t> result_nonce = this->BuildSigningNonce(SigningHash.Arr,SigningHash.GetLength());

	FString result_nonce_str = BytesToHex(result_nonce.GetData(), result_nonce.Num());
	//UE_LOG(LogTemp, Display, TEXT("Nonce: %s"), *result_nonce_str);

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