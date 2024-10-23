// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once
#include "Indexer/Structs/Struct_Data.h"
#include "SequenceAuthenticator.h"
#include "Util/Async.h"
#include "Eth/EthTransaction.h"
#include "Types/BinaryData.h"
#include "Containers/Union.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Types/Header.h"
#include "Dom/JsonObject.h"
#include "Types/TransactionReceipt.h"
#include "Types/ContractCall.h"
#include "ProviderEnum.h"
#include "SequenceResponseIntent.h"
#include "Sequence/FeeOption.h"
#include "TransakDataTypes.h"
#include "Util/SequenceSupport.h"
#include "Util/Structs/BE_Enums.h"
#include "SequenceAPI.generated.h"

class UIndexer;
class UProvider;
class USequenceRPCManager;

UCLASS()
class SEQUENCEPLUGIN_API USequenceWallet : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;
private:
	UPROPERTY()
	USequenceRPCManager * SequenceRPCManager;
	
	UPROPERTY()
	UIndexer * Indexer;

	UPROPERTY()
	UProvider * Provider;

	UPROPERTY()
	FCredentials_BE Credentials;

public:
	USequenceWallet();

	/**
	 * Gets the SequenceWallet SubSystem,
	 * this call will attempt to automatically load stored credentials on disk if any.
	 * @return TOptional<USequenceWallet*> Initialized USequenceWallet
	 */
	static TOptional<USequenceWallet*> Get();
	
	/**
	 * Gets the SequenceWallet SubSystem and initialize it with the given credentials
	 * @param Credentials Credentials used to Initiate USequenceWallet
	 * @return TOptional<USequenceWallet*> Initialized USequenceWallet
	 */
	static TOptional<USequenceWallet*> Get(const FCredentials_BE& Credentials);

	/**
	 * Gets the SequenceWallet SubSystem and initializes it with the given credentials & ProviderUrl
	 * @param Credentials Credentials used to Initiate USequenceWallet
	 * @param ProviderUrl ProviderUrl used to Initiate USequenceWallet
	 * @return TOptional<USequenceWallet*> Initialized USequenceWallet
	 */
	static TOptional<USequenceWallet*> Get(const FCredentials_BE& Credentials, const FString& ProviderUrl);

	/**
	 * Gets all networks in the system
	 * @return TArray of all networks
	 */
	static TArray<FIdNamePair> GetAllNetworks();

	/**
	 * Gets all Network Names supported by sequence
	 * @return A list of all network names in the system
	 */
	static TArray<FString> GetAllNetworkNames();

	/**
	 * Gets a list of all network Ids support by sequence
	 * @return A list of all network ids in the system
	 */
	static TArray<int64> GetAllNetworkIds();

	/**
	 * Pass in a network Id and receive the name associated with that Id
	 * @param NetworkIdIn The Id associated with the Network Name
	 * @return The matching network name, OR an empty FString if nothing was found
	 */
	static FString GetNetworkName(const int64 NetworkIdIn);

	/**
	 * Gets the associated Network Id with the given Network Name
	 * @param NetworkNameIn The network's name
	 * @return The associated Id, if none was found returns -1
	 */
	static int64 GetNetworkId(const FString& NetworkNameIn);

	/**
	 * Converts a decimal value like 3.2 and converts it into an amount that the system uses to sendTransactions with
	 * @param AmountIn The User readable decimal amount ie) 3.2
	 * @param DecimalsIn The Decimals value associated with the token, IE) USDC is 6
	 * @return The Converted Decimals amount
	 */
	static int64 GetSystemReadableAmount(const float AmountIn, const int64 DecimalsIn);

	/**
	 * Takes a Received amount and converts it into a user readable amount
	 * @param AmountIn The received System amount
	 * @param DecimalsIn The decimal value associated with the Token IE) USDC is 6
	 * @return The user readable amount
	 */
	static float GetUserReadableAmount(const int64 AmountIn, const int64 DecimalsIn);

	/**
	 * Returns the wallet address of the currently signed in user
	 * @return wallet address of the currently signed in user
	 */
	FString GetWalletAddress() const;

	/**
	 * Allows you to update the provider URL set for the SequenceWallet
	 * @param Url New provider Url you wish to use
	 */
	void UpdateProviderURL(const FString& Url) const;

	/**
	 * Allows you to update the set network for the SequenceWallet
	 * @param NewNetwork New network you wish to use
	 */
	void UpdateNetworkId(int64 NewNetwork);

	void UpdateNetworkId(FString NewNetworkName);

	void UpdateNetworkId(ENetwork NewNetwork);

	/**
	 * Allows to get the currently set network for the SequenceWallet
	 * @return currently set network for the SequenceWallet
	 */
	int64 GetNetworkId() const;

	/**
	 * Allows you to sign the given message with the SequenceWallet
	 * @param Message The message you wish to sign
	 * @param OnSuccess The returned Struct from the signing process
	 * @param OnFailure If an error occurs
	 */
	void SignMessage(const FString& Message, const TSuccessCallback<FSeqSignMessageResponse_Response>& OnSuccess, const FFailureCallback& OnFailure) const;

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
	void ListSessions(const TSuccessCallback<TArray<FSeqListSessions_Session>>& OnSuccess, const FFailureCallback& OnFailure) const;
	
	/**
	 * Used to close the current Session with Sequence & clears all locally stored credentials
	 */
	void SignOut() const;

	/**
	 * Gets a list of supported Transak Countries
	 * @param OnSuccess TArray<FSupportedCountry> containing supported countries
	 * @param OnFailure An FSequenceError
	 */
	void GetSupportedTransakCountries(const TSuccessCallback<TArray<FSupportedCountry>>& OnSuccess, const FFailureCallback& OnFailure);

	/**
	 * Opens a link in Transak with the parameters set
	 * @param FiatCurrency FiatCurrency to use in Transak
	 * @param DefaultAmount Default amount to use in Transak
	 * @param DefaultCryptoCurrency Default CryptoCurrency to use in Transak
	 * @param Networks List of networks used in Transak
	 * @param DisableWalletAddressForm DisableWalletAddressForm in Transak
	 */
	void OpenTransakLink(const FString& FiatCurrency = FDefaultTransak::FiatCurrency, const FString& DefaultAmount = FDefaultTransak::FiatAmount, const FString& DefaultCryptoCurrency = FDefaultTransak::CryptoCurrency, const FString& Networks = FDefaultTransak::Networks, bool DisableWalletAddressForm = FDefaultTransak::DisableWalletAddressForm);

private:
	static USequenceWallet * GetSubSystem();
	void Init(const FCredentials_BE& CredentialsIn);
	void Init(const FCredentials_BE& CredentialsIn,const FString& ProviderURL);

	static TArray<FFeeOption> MarkValidFeeOptions(TArray<FFeeOption> FeeOptions, TArray<FFeeOption> BalanceOptions);
	static TArray<FFeeOption> FindValidFeeOptions(const TArray<FFeeOption>& FeeOptions, const TArray<FFeeOption>& BalanceOptions);
	static TArray<FFeeOption> BalancesListToFeeOptionList(const TArray<FSeqTokenBalance>& BalanceList);

public:
	//Indexer Specific Calls
	
	/*
	Used to get a ping back from the Chain
	*/
	void Ping(const TSuccessCallback<bool>& OnSuccess, const FFailureCallback& OnFailure) const;

	/*
		Used to get version data back from the Chain
	*/
	void Version(const TSuccessCallback<FSeqVersion>& OnSuccess, const FFailureCallback& OnFailure) const;

	/*
		Used to get the runtime status of the Chain
	*/
	void RunTimeStatus(const TSuccessCallback<FSeqRuntimeStatus>& OnSuccess, const FFailureCallback& OnFailure) const;

	/*
		Used to get the chainID from the Chain
	*/
	void GetChainID(const TSuccessCallback<int64>& OnSuccess, const FFailureCallback& OnFailure) const;

	/*
		Used to get the Ether balance from the Chain
		@param 1st the ChainID
		@param 2nd the accountAddr we want to get the balance for
		@return the Balance ASYNC calls (update ether balance in the bck_mngr when done processing)
	*/
	void GetEtherBalance(const FString& AccountAddr, const TSuccessCallback<FSeqEtherBalance>& OnSuccess, const FFailureCallback& OnFailure) const;

	/*
		Gets the token balances from the Chain
	*/
	void GetTokenBalances(const FSeqGetTokenBalancesArgs& Args, const TSuccessCallback<FSeqGetTokenBalancesReturn>& OnSuccess, const FFailureCallback& OnFailure) const;

	/*
		gets the token supplies from the Chain
	*/
	void GetTokenSupplies(const FSeqGetTokenSuppliesArgs& Args, const TSuccessCallback<FSeqGetTokenSuppliesReturn>& OnSuccess, const FFailureCallback& OnFailure) const;

	/*
		gets the token supplies map from the Chain
	*/
	void GetTokenSuppliesMap(const FSeqGetTokenSuppliesMapArgs& Args, const TSuccessCallback<FSeqGetTokenSuppliesMapReturn>& OnSuccess, const FFailureCallback& OnFailure) const;

	/*
		Get the balance updates from the Chain
	*/
	UE_DEPRECATED(5.4, "GetBalanceUpdates is obsolete.")
	void GetBalanceUpdates(const FSeqGetBalanceUpdatesArgs& Args, const TSuccessCallback<FSeqGetBalanceUpdatesReturn>& OnSuccess, const FFailureCallback& OnFailure) const;

	/*
		get transaction history from the Chain
	*/
	void GetTransactionHistory(const FSeqGetTransactionHistoryArgs& Args, const TSuccessCallback<FSeqGetTransactionHistoryReturn>& OnSuccess, const FFailureCallback& OnFailure) const;
	
	//Provider calls

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
};