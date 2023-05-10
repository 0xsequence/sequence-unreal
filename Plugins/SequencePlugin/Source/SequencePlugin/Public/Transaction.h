#pragma once
#include "Types.h"

class Transaction
{
public:
	BlockNonce Nonce;
	FBinaryData GasPrice;
	FBinaryData GasLimit;
	Address To;
	FBinaryData Value;
	FBinaryData Data;
	FBinaryData V;
	FBinaryData R;
	FBinaryData S;

	void Sign(PrivateKey PrivateKey, int ChainID);
	FBinaryData GetSignedTransaction(PrivateKey PrivateKey, int ChainID);
};
