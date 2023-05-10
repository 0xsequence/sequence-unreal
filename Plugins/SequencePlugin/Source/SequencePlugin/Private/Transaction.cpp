#include "Transaction.h"

#include "Crypto.h"
#include "HexUtility.h"
#include "RLP.h"
#include "Bitcoin-Cryptography-Library/cpp/Ecdsa.hpp"
#include "Bitcoin-Cryptography-Library/cpp/Keccak256.hpp"
#include "Bitcoin-Cryptography-Library/cpp/Sha256Hash.hpp"
#include "Bitcoin-Cryptography-Library/cpp/Uint256.hpp"

void Transaction::Sign(PrivateKey PrivateKey, int ChainID)
{
	auto NonceStr = TrimHex(HashToHexString(GBlockNonceByteLength, Nonce));
	UE_LOG(LogTemp, Display, TEXT("Nonce: %s"), *NonceStr);
	
	auto EncodedSigningData = RLP::Encode(Itemize(new RLPItem[]
	{
		Itemize(HexStringtoBinary(NonceStr)), // Nonce
		Itemize(GasPrice), // GasPrice
		Itemize(GasLimit), // GasLimit
		Itemize(FBinaryData{To, GAddressByteLength}), // To
		Itemize(Value), // Value
		Itemize(Data), // Data
		Itemize(HexStringtoBinary("0x1")), // V
		Itemize(HexStringtoBinary("0x")), // R 
		Itemize(HexStringtoBinary("0x")), // S
	}, 9));

	Hash256 SigningHash = new uint8[GHash256ByteLength];
	Keccak256::getHash(EncodedSigningData.Data, EncodedSigningData.ByteLength, SigningHash);

	PublicKey PublicKey = GetPublicKey(PrivateKey);
	Address Addr = GetAddress(PublicKey);
	uint8 MyR[GHash256ByteLength];
	uint8 MyS[GHash256ByteLength];
	Uint256 BigR, BigS;

	auto IsSuccess = Ecdsa::signWithHmacNonce(Uint256(PrivateKey), Sha256Hash(SigningHash, GHash256ByteLength), BigR, BigS);
	BigR.getBigEndianBytes(MyR);
	BigS.getBigEndianBytes(MyS);
	uint16 BigV = (MyR[GHash256ByteLength - 1] % 2) + ChainID * 2 + 35;

	this->V = HexStringtoBinary(IntToHexString(BigV));
	this->R = HexStringtoBinary(Hash256ToHexString(MyR));
	this->S = HexStringtoBinary(Hash256ToHexString(MyS));
}

FBinaryData Transaction::GetSignedTransaction(PrivateKey PrivateKey, int ChainID)
{
	this->Sign(PrivateKey, ChainID);

	auto NonceStr = TrimHex(HashToHexString(GBlockNonceByteLength, Nonce));
	UE_LOG(LogTemp, Display, TEXT("Nonce: %s"), *NonceStr);
	
	return RLP::Encode(Itemize(new RLPItem[]
	{
		Itemize(HexStringtoBinary(NonceStr)), // Nonce
		Itemize(GasPrice), // GasPrice
		Itemize(GasLimit), // GasLimit
		Itemize(FBinaryData{To, GAddressByteLength}), // To
		Itemize(Value), // Value
		Itemize(Data), // Data
		Itemize(V), // V
		Itemize(R), // R 
		Itemize(S), // S
	}, 9));
}
