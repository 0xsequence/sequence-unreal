// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once
#include "Credentials.h"
#include "Indexer/Structs/Struct_Data.h"
#include "Util/Async.h"
#include "Eth/EthTransaction.h"
#include "Types/BinaryData.h"
#include "Containers/Union.h"
#include "Types/Header.h"
#include "Dom/JsonObject.h"
#include "Types/TransactionReceipt.h"
#include "Types/ContractCall.h"
#include "ProviderEnum.h"
#include "SequenceResponseIntent.h"
#include "Indexer/SequenceIndexer.h"
#include "Sequence/FeeOption.h"
#include "Util/CredentialsStorage.h"
#include "SequenceWallet.generated.h"
 
using FSignature = FUnsizedData;
using TransactionID = FString;

class UProvider;
class USequenceIndexer;
class USequenceRPCManager;

UENUM(Blueprintable)
enum class ESessionState : uint8
{
	Unchecked UMETA(DisplayName = "Unchecked"),
	Valid UMETA(DisplayName = "Valid"),
	Invalid UMETA(DisplayName = "Invalid"),
	Checking UMETA(DisplayName = "Checking"),
};

UCLASS()
class SEQUENCEPLUGIN_API USequenceWallet : public UObject
{
	GENERATED_BODY()

public:
	USequenceWallet();
	
	/**
	 * Check if the wallet and stored credentials are registered to a valid session with the Sequence API
	 * @return true if the wallet is signed in to Sequence API
	 */
	bool IsValidSession();
	
	/**
	 * Returns the wallet address of the currently signed in user
	 * @return wallet address of the currently signed in user
	 */
	FString GetWalletAddress() const;

	/**
	 * Get the current/active session id associated with this wallet
	 * @return the current/active session id associated with this wallet
	 */
	FString GetSessionId() const;

	/**
	 * Allows you to update the provider URL set for the SequenceWallet
	 * @param Url New provider Url you wish to use
	 */
	void UpdateProviderURL(const FString& Url) const;

	/**
	 * Allows you to sign the given message with the SequenceWallet
	 * @param Message The message you wish to sign
	 * @param OnSuccess The returned Struct from the signing process
	 * @param OnFailure If an error occurs
	 */
	void SignMessage(const FString& Message, const TSuccessCallback<FSeqSignMessageResponse_Response>& OnSuccess, const FFailureCallback& OnFailure) const;

	/**
	 * Allows you to validate the signature of the given message with the SequenceWallet
	 * @param Signature The signature you wish to validate
	 * @param Message The message that has been signed
	 * @param OnSuccess The returned Struct from the signing process
	 * @param OnFailure If an error occurs
	 */

	void ValidateMessageSignature(const int64& ChainId, const FString& WalletAddress, const FString& Message, const FString& Signature, const TSuccessCallback<FSeqValidateMessageSignatureResponse_Data>& OnSuccess, const FFailureCallback& OnFailure) const;

	/**
	 * Allows you to send native token balance to another wallet address.
	 * @param RecipientAddress Specify the recipient address you want to send Ether to
	 * @param Amount Amount of Ether to send
	 * @param OnSuccess Status if the transaction was successful
	 * @param OnFailure An error occured during the transaction
	 */
	void SendNativeToken(const FString& RecipientAddress, const FString& Amount, const TSuccessCallback<FSeqTransactionResponse_Data>& OnSuccess, const FFailureCallback& OnFailure) const;
	
	/**
	 * Allows you to send a transaction that will be automatically gassed IF the token is able to be (not all can be)
	 * @param Transactions The transaction you wish to send
	 * @param OnSuccess FSeqTransactionResponse_Data containing information about the Successful Transaction
	 * @param OnFailure An error occured during the transaction OR the token provided wasn't able to be automatically gassed
	 */
	void SendTransaction(const TArray<TransactionUnion>& Transactions, const TSuccessCallback<FSeqTransactionResponse_Data>& OnSuccess, const FFailureCallback& OnFailure) const;

	/**
	 * Allows you to send a transaction with a given Fee, Use GetFeeOptions Or GetUnfilteredFeeOptions
	 * @param Transactions The transaction you wish to send
	 * @param FeeOption The FeeOption you are using to pay for the transaction
	 * @param OnSuccess FSeqTransactionResponse_Data containing information about the Successful Transaction
	 * @param OnFailure An error occured during the transaction
	 */
	void SendTransactionWithFeeOption(const TArray<TransactionUnion>& Transactions, const FFeeOption& FeeOption, const TSuccessCallback<FSeqTransactionResponse_Data>& OnSuccess, const FFailureCallback& OnFailure) const;

	/**
	 * Allows you to get Valid FeeOptions for the transaction you pass in, Can be empty if none can be afforded
	 * @param Transactions List of transactions you want to determine fee data for
	 * @param OnSuccess A list of all VALID feeOptions for the presented transaction (can be empty if your wallet contains nothing that can cover any of the feeOptions)
	 * @param OnFailure An error occured
	 */
	void GetFeeOptions(const TArray<TransactionUnion>& Transactions, const TSuccessCallback<TArray<FFeeOption>>& OnSuccess, const FFailureCallback& OnFailure) const;

	/**
	 * Allows you to see all potential FeeOptions, Valid ones are marked with bCanAfford = true, in the list of FFeeOptions
	 * @param Transactions List of transactions you want to determine fee data for
	 * @param OnSuccess A list of all feeOptions with valid ones marked with bCanAfford = true. Possible none are valid if your wallet has nothing to cover the fees
	 * @param OnFailure An error occured
	 */
	void GetUnfilteredFeeOptions(const TArray<TransactionUnion>& Transactions, const TSuccessCallback<TArray<FFeeOption>>& OnSuccess, const FFailureCallback& OnFailure) const;
	
	/**
	 * Used to list all active sessions for the signed in credentials
	 * @param OnSuccess A list of all active sessions
	 * @param OnFailure An error occured
	 */
	 
	void GetIdToken(const FString& Nonce, const TSuccessCallback<FSeqIdTokenResponse_Data>&OnSuccess, const FFailureCallback& OnFailure) const;

	void ListSessions(const TSuccessCallback<TArray<FSeqListSessions_Session>>& OnSuccess, const FFailureCallback& OnFailure) const;
	
	void ListAccounts(const TSuccessCallback<FSeqListAccountsResponse_Data>& OnSuccess, const FFailureCallback& OnFailure) const;

	void GetSessionAuthProof(const FString& Nonce, const TSuccessCallback<FSeqGetSessionAuthProof_Data>& OnSuccess, const FFailureCallback& OnFailure) const;

	/**
	 * Used to close the current Session with Sequence & clears all locally stored credentials
	 */
	void SignOut() const;

	void RemoveLinkedWallet(const FString& LinkedWalletAddress, const TFunction<void()>& OnSuccess, const FFailureCallback& OnFailure) const;
	void GetLinkedWallets(const TSuccessCallback<FSeqLinkedWalletsResponse>& OnSuccess, const FFailureCallback& OnFailure) const;
	void BlockByNumber(uint64 Number, const TFunction<void(TSharedPtr<FJsonObject>)>& OnSuccess,
	                   const TFunction<void(FSequenceError)>& OnFailure) const;
	void BlockByNumber(EBlockTag Tag, const TFunction<void(TSharedPtr<FJsonObject>)>& OnSuccess,
	                   const TFunction<void(FSequenceError)>& OnFailure) const;
	void BlockByHash(const FHash256& Hash, const TFunction<void(TSharedPtr<FJsonObject>)>& OnSuccess,
	                 const TFunction<void(FSequenceError)>& OnFailure) const;
	void BlockNumber(const TFunction<void(uint64)>& OnSuccess, const TFunction<void(FSequenceError)>& OnFailure) const;

	void HeaderByNumber(uint64 Id, const TFunction<void(FHeader)>& OnSuccess, const TFunction<void(FSequenceError)>& OnFailure) const;
	void HeaderByNumber(EBlockTag Tag, const TFunction<void(FHeader)>& OnSuccess, const TFunction<void(FSequenceError)>& OnFailure) const;
	void HeaderByHash(const FHash256& Hash, const TFunction<void(FHeader)>& OnSuccess, const TFunction<void(FSequenceError)>& OnFailure) const;

	void TransactionByHash(const FHash256& Hash, const TFunction<void(TSharedPtr<FJsonObject>)>& OnSuccess,
	                       const TFunction<void(FSequenceError)>& OnFailure) const;
	void TransactionCount(const FAddress& Addr, uint64 Number, const TFunction<void(uint64)>& OnSuccess,
	                      const TFunction<void(FSequenceError)>& OnFailure) const;
	void TransactionCount(const FAddress& Addr, EBlockTag Tag, const TFunction<void(uint64)>& OnSuccess,
	                      const TFunction<void(FSequenceError)>& OnFailure) const;
	void TransactionReceipt(const FHash256& Hash, const TFunction<void(FTransactionReceipt)>& OnSuccess,
	                        const TFunction<void(FSequenceError)>& OnFailure) const;

	void GetGasPrice(const TFunction<void(FUnsizedData)>& OnSuccess, const TFunction<void(FSequenceError)>& OnFailure) const;
	void EstimateContractCallGas(const FContractCall& ContractCall, const TFunction<void(FUnsizedData)>& OnSuccess,
	                             const TFunction<void(FSequenceError)>& OnFailure) const;
	
	void NonceAt(uint64 Number, const TFunction<void(FBlockNonce)>& OnSuccess, const TFunction<void(FSequenceError)>& OnFailure) const;
	void NonceAt(EBlockTag Tag, const TFunction<void(FBlockNonce)>& OnSuccess, const TFunction<void(FSequenceError)>& OnFailure) const;
	void SendRawTransaction(const FString& Data, const TFunction<void(FUnsizedData)>& OnSuccess,
	                        const TFunction<void(FSequenceError)>& OnFailure) const;

	void ChainId(const TFunction<void(uint64)>& OnSuccess, const TFunction<void(FSequenceError)>& OnFailure) const;

	void Call(const FContractCall& ContractCall, uint64 Number, const TFunction<void(FUnsizedData)>& OnSuccess,
	          const TFunction<void(FSequenceError)>& OnFailure) const;
	
	void Call(const FContractCall& ContractCall, EBlockTag Number, const TFunction<void(FUnsizedData)>& OnSuccess,
	          const TFunction<void(FSequenceError)>& OnFailure) const;
	
	void NonViewCall(const FEthTransaction& Transaction, const FPrivateKey& PrivateKey, int ChainID,
	                 const TFunction<void(FUnsizedData)>& OnSuccess, const TFunction<void(FSequenceError)>& OnFailure) const;

private:
	static TArray<FFeeOption> MarkValidFeeOptions(TArray<FFeeOption> FeeOptions, TArray<FFeeOption> BalanceOptions);
	static TArray<FFeeOption> FindValidFeeOptions(const TArray<FFeeOption>& FeeOptions, const TArray<FFeeOption>& BalanceOptions);
	static TArray<FFeeOption> BalancesListToFeeOptionList(const TArray<FSeqTokenBalance>& BalanceList);

	/*
	 * Gets the current instance of RPCManager and calls the UpdateWithStoredSessionWallet function
	 * to ensure that the RPCManager has the correct session data from this wallet instance
	 */
	USequenceRPCManager* GetRpcManager() const;

	/*
	 * Wrapper function to get the currently stored credentials
	 */
	FCredentials_BE GetCredentials() const;
	
	UPROPERTY()
	USequenceRPCManager* SequenceRPCManager;
	
	UPROPERTY()
	USequenceIndexer* Indexer;

	UPROPERTY()
	UProvider * Provider;

	UPROPERTY()
	UCredentialsStorage* CredentialsStorage;

	UPROPERTY()
	ESessionState SessionState = ESessionState::Unchecked;
};