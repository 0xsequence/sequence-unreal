#pragma once
#include "Async.h"
#include "EthTransaction.h"
#include "RPCCaller.h"
#include "Types/BinaryData.h"

namespace SequenceAPI
{
	
	using FSignature = FUnsizedData;

	enum ESortOrder {
		DESC,
		ASC
	};

	FString SortOrderToString(ESortOrder SortOrder);

	struct FSortBy
	{
		FString Column;
		ESortOrder Order;
	};

	struct FPage
	{
		TOptional<uint64> PageSize;
		TOptional<uint64> PageNum;
		TOptional<uint64> TotalRecords;
		TOptional<FString> Column;
		TOptional<FSortBy> Sort; 
	};

	struct FTransaction
	{
		uint64 ChainId;
		FString From;
		FAddress To;
		TOptional<FString> AutoGas;
		TOptional<uint64> Nonce;
		TOptional<FString> Value;
		TOptional<FString> CallData;
		TOptional<FString> TokenAddress;
		TOptional<FString> TokenAmount;
		TOptional<TArray<FString>> TokenIds;
		TOptional<TArray<FString>> TokenAmounts;
	};

	struct FPartnerWallet
	{
		uint64 Number;
		uint64 PartnerId;
		uint64 WalletIndex;
		FString WalletAddress;
	};

	struct FDeployWalletReturn
	{
		FString Address;
		FString TransactionHash;
	};

	struct FWalletsReturn
	{
		TArray<FPartnerWallet> Wallets;
		FPage Page;
	};

	class FSequenceWallet : public RPCCaller
	{
		FString Hostname;
		const FString Path = "/rpc/Wallet/";
		FString Url(FString Name) const;
		
	public:
		FSequenceWallet(FString Hostname);
		
		void CreateWallet(uint64 AccountIndex, TSuccessCallback<FAddress> OnSuccess, FFailureCallback OnFailure);
		void GetWalletAddress(uint64 AccountIndex, TSuccessCallback<FAddress> OnSuccess, FFailureCallback OnFailure);
		void DeployWallet(uint64 ChainId, uint64 AccountIndex, TSuccessCallback<FDeployWalletReturn> OnSuccess, FFailureCallback OnFailure);
		void Wallets(FPage Page, TSuccessCallback<FWalletsReturn> OnSuccess, FFailureCallback OnFailure);
		void Wallets(TSuccessCallback<FWalletsReturn> OnSuccess, FFailureCallback OnFailure);
		void SignMessage(uint64 ChainId, FAddress AccountAddress, FUnsizedData Message, TSuccessCallback<FSignature> OnSuccess, FFailureCallback OnFailure);
		void IsValidMessageSignature(uint64 ChainId, FAddress WalletAddress, FUnsizedData Message, FSignature Signature, TSuccessCallback<bool> OnSuccess, FFailureCallback OnFailure);
		void SendTransaction(FTransaction Transaction, TSuccessCallback<FHash256> OnSuccess, FFailureCallback OnFailure);
		void SendTransactionBatch(TArray<FTransaction> Transactions, TSuccessCallback<FHash256> OnSuccess, FFailureCallback OnFailure);
	};

}