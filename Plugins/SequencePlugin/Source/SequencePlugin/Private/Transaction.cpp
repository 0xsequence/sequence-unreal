#include "Transaction.h"

#include "BinaryData.h"
#include "Crypto.h"
#include "HexUtility.h"
#include "RLP.h"
#include "Bitcoin-Cryptography-Library/cpp/Ecdsa.hpp"
#include "Bitcoin-Cryptography-Library/cpp/Keccak256.hpp"
#include "Bitcoin-Cryptography-Library/cpp/Sha256Hash.hpp"
#include "Bitcoin-Cryptography-Library/cpp/Uint256.hpp"

Transaction::Transaction(FBlockNonce Nonce, FNonUniformData GasPrice, FNonUniformData GasLimit, FAddress To,
	FNonUniformData Value, FNonUniformData Data) : Nonce(Nonce), GasPrice(GasPrice), GasLimit(GasLimit), To(To), Value(Value), Data(Data), V(nullptr, 0)
{
}

void Transaction::Sign(FPrivateKey PrivateKey, int ChainID)
{
	auto NonceStr = TrimHex(Nonce.ToHex());
	UE_LOG(LogTemp, Display, TEXT("Nonce: %s"), *NonceStr);

	auto trimmed_nonce = Trimmed(Nonce);
	
	auto EncodedSigningData = RLP::Encode(Itemize(new RLPItem[]
	{
		Itemize(trimmed_nonce), // Nonce
		Itemize(GasPrice), // GasPrice
		Itemize(GasLimit), // GasLimit
		Itemize(To), // To
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
	Uint256 BigR, BigS;

	auto IsSuccess = Ecdsa::signWithHmacNonce(Uint256(PrivateKey.Arr), Sha256Hash(SigningHash.Arr, FHash256::Size), BigR, BigS);
	BigR.getBigEndianBytes(MyR.Arr);
	BigS.getBigEndianBytes(MyS.Arr);
	uint16 BigV = (MyR.Arr[FHash256::Size - 1] % 2) + ChainID * 2 + 35;

	UE_LOG(LogTemp, Display, TEXT("ENCODED SIGNING DATA: %s"), *EncodedSigningData.ToHex());
	UE_LOG(LogTemp, Display, TEXT("SIGNING HASH: %s"), *SigningHash.ToHex());
	UE_LOG(LogTemp, Display, TEXT("R: %s"), *MyR.ToHex());
	UE_LOG(LogTemp, Display, TEXT("S: %s"), *MyS.ToHex());

	this->V = HexStringToBinary(IntToHexString(BigV));
	this->R = MyR;
	this->S = MyS;
}

FNonUniformData Transaction::GetSignedTransaction(FPrivateKey PrivateKey, int ChainID)
{
	this->Sign(PrivateKey, ChainID);

	auto NonceStr = TrimHex(Nonce.ToHex());
	UE_LOG(LogTemp, Display, TEXT("Nonce: %s"), *NonceStr);
	
	return RLP::Encode(Itemize(new RLPItem[]
	{
		Itemize(HexStringToBinary(NonceStr)), // Nonce
		Itemize(GasPrice), // GasPrice
		Itemize(GasLimit), // GasLimit
		Itemize(To), // To
		Itemize(Value), // Value
		Itemize(Data), // Data
		Itemize(V), // V
		Itemize(R), // R 
		Itemize(S), // S
	}, 9));
}
