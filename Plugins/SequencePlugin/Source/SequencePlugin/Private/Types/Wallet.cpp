#include "Types/Wallet.h"
#include "EthTransaction.h"

FWallet::FWallet(const FPrivateKey PrivateKey) : PrivateKey(PrivateKey)
{
}

FNonUniformData FWallet::GetSignedTransaction(FEthTransaction Transaction, const int ChainId) const
{
	return Transaction.GetSignedTransaction(PrivateKey, ChainId);
}
