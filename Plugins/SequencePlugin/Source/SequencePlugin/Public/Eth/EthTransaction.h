#pragma once
#include "Types/BinaryData.h"

class FEthTransaction
{
	FBlockNonce Nonce;
	FUnsizedData GasPrice;
	FUnsizedData GasLimit;
	FAddress To;
	FUnsizedData Value;
	FUnsizedData Data;
	FUnsizedData V;
	FHash256 R;
	FHash256 S;
public:
	FEthTransaction(FBlockNonce Nonce, FUnsizedData GasPrice, FUnsizedData GasLimit, FAddress To, FUnsizedData Value, FUnsizedData Data);

	void Sign(FPrivateKey PrivateKey, int ChainID);
	FHash256 GetSignedTransactionHash(FPrivateKey Key, int ChainID);
	FHash256 GetUnsignedTransactionHash(int ChainID);
	FUnsizedData GetSignedTransaction(FPrivateKey PrivateKey, int ChainID);
	void Log();
};
