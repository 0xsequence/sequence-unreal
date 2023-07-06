#include "FEthTransaction.h"

#include "Types/BinaryData.h"
#include "Crypto.h"
#include "HexUtility.h"
#include "RLP.h"
#include "Bitcoin-Cryptography-Library/cpp/Ecdsa.hpp"
#include "Bitcoin-Cryptography-Library/cpp/Keccak256.hpp"
#include "Bitcoin-Cryptography-Library/cpp/Sha256Hash.hpp"
#include "Bitcoin-Cryptography-Library/cpp/Uint256.hpp"

FEthTransaction::FEthTransaction(FBlockNonce Nonce, FNonUniformData GasPrice, FNonUniformData GasLimit, FAddress To,
	FNonUniformData Value, FNonUniformData Data) : Nonce(Nonce), GasPrice(GasPrice), GasLimit(GasLimit), To(To),
	                                               Value(Value), Data(Data), V(nullptr, 0), R(FHash256{}), S(FHash256{})
{
}

void FEthTransaction::Sign(FPrivateKey PrivateKey, int ChainID)
{
	auto NonceStr = TrimHex(Nonce.ToHex());
	UE_LOG(LogTemp, Display, TEXT("Nonce: %s"), *NonceStr);

	auto trimmed_nonce = Trimmed(Nonce);

	UE_LOG(LogTemp, Display, TEXT("My trimmed nonce is %s"), *trimmed_nonce.ToHex());
	
	auto EncodedSigningData = RLP::Encode(Itemize(new RLPItem[]
	{
		Itemize(trimmed_nonce), // Nonce
		Itemize(GasPrice.Trim()), // GasPrice
		Itemize(GasLimit.Trim()), // GasLimit
		Itemize(Trimmed(To)), // To
		Itemize(Value), // Value
		Itemize(Data), // Data
		Itemize(HexStringToBinary(IntToHexString(ChainID))), // V
		Itemize(HexStringToBinary("")), // R 
		Itemize(HexStringToBinary("")), // S
	}, 9));

	trimmed_nonce.Destroy();

	auto SigningHash = FHash256::New();
	Keccak256::getHash(EncodedSigningData.Arr, EncodedSigningData.GetLength(), SigningHash.Arr);

	FPublicKey PublicKey = GetPublicKey(PrivateKey);
	FAddress Addr = GetAddress(PublicKey);
	auto MyR = FHash256::New();
	auto MyS = FHash256::New();
	auto MyY = FHash256::New();
	Uint256 BigR, BigS;
	uint16 recoveryParameter;
	auto IsSuccess = Ecdsa::signWithHmacNonce(Uint256(PrivateKey.Arr), Sha256Hash(SigningHash.Arr, FHash256::Size), BigR, BigS, recoveryParameter);
	BigR.getBigEndianBytes(MyR.Arr);
	BigS.getBigEndianBytes(MyS.Arr);
	
	const uint16 BigV = ChainID * 2 + 35 + recoveryParameter;
	UE_LOG(LogTemp, Display, TEXT("Recovery Bit: %d"), recoveryParameter);
	UE_LOG(LogTemp, Display, TEXT("ENCODED SIGNING DATA: %s"), *EncodedSigningData.ToHex());
	UE_LOG(LogTemp, Display, TEXT("SIGNING HASH: %s"), *SigningHash.ToHex());
	UE_LOG(LogTemp, Display, TEXT("R: %s"), *MyR.ToHex());
	UE_LOG(LogTemp, Display, TEXT("S: %s"), *MyS.ToHex());
	UE_LOG(LogTemp, Display, TEXT("PRIVATE KEY: %s"), *PrivateKey.ToHex());

	this->V = HexStringToBinary(IntToHexString(BigV));
	this->R = MyR;
	this->S = MyS;
}

FHash256 FEthTransaction::GetTransactionHash(int ChainID)
{
	auto NonceStr = TrimHex(Nonce.ToHex());
	UE_LOG(LogTemp, Display, TEXT("Nonce: %s"), *NonceStr);

	auto trimmed_nonce = Trimmed(Nonce);
	
	auto EncodedSigningData = RLP::Encode(Itemize(new RLPItem[]
	{
		Itemize(trimmed_nonce), // Nonce
		Itemize(GasPrice.Trim()), // GasPrice
		Itemize(GasLimit.Trim()), // GasLimit
		Itemize(Trimmed(To)), // To
		Itemize(Value), // Value
		Itemize(Data), // Data
		Itemize(HexStringToBinary(IntToHexString(ChainID))), // V
		Itemize(HexStringToBinary("")), // R 
		Itemize(HexStringToBinary("")), // S
	}, 9));

	trimmed_nonce.Destroy();

	auto SigningHash = FHash256::New();
	Keccak256::getHash(EncodedSigningData.Arr, EncodedSigningData.GetLength(), SigningHash.Arr);

	return SigningHash;
}

FNonUniformData FEthTransaction::GetSignedTransaction(FPrivateKey PrivateKey, int ChainID)
{
	this->Sign(PrivateKey, ChainID);

	auto NonceStr = TrimHex(Nonce.ToHex());
	UE_LOG(LogTemp, Display, TEXT("Nonce: %s"), *NonceStr);
	
	return RLP::Encode(Itemize(new RLPItem[]
	{
		Itemize(HexStringToBinary(NonceStr)), // Nonce
		Itemize(GasPrice.Trim()), // GasPrice
		Itemize(GasLimit.Trim()), // GasLimit
		Itemize(Trimmed(To)), // To
		Itemize(Value), // Value
		Itemize(Data), // Data
		Itemize(V), // V
		Itemize(R), // R 
		Itemize(S), // S
	}, 9));
}

void FEthTransaction::Log()
{
	UE_LOG(LogTemp, Display, TEXT("Nonce: %s"), *Nonce.ToHex());
	UE_LOG(LogTemp, Display, TEXT("Gasprice: %s"), *GasPrice.ToHex());
	UE_LOG(LogTemp, Display, TEXT("Gaslimit: %s"), *GasLimit.ToHex());
	UE_LOG(LogTemp, Display, TEXT("Address: %s"), *To.ToHex());
	UE_LOG(LogTemp, Display, TEXT("Value: %s"), *Value.ToHex());
	UE_LOG(LogTemp, Display, TEXT("Data: %s"), *Data.ToHex());
	UE_LOG(LogTemp, Display, TEXT("V: %s"), *V.ToHex());
	UE_LOG(LogTemp, Display, TEXT("R: %s"), *R.ToHex());
	UE_LOG(LogTemp, Display, TEXT("S: %s"), *S.ToHex());
}
