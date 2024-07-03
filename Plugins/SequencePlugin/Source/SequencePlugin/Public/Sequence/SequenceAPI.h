// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once
#include "Indexer/Structs/Struct_Data.h"
#include "Authenticator.h"
#include "Util/Async.h"
#include "Eth/EthTransaction.h"
#include "Util/Structs/BE_Structs.h"
#include "Types/BinaryData.h"
#include "Session.h"
#include "SignedMessage.h"
#include "Containers/Union.h"
#include "TransactionResponse.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Types/Header.h"
#include "Dom/JsonObject.h"
#include "Types/TransactionReceipt.h"
#include "Types/ContractCall.h"
#include "ProviderEnum.h"
#include "Sequence/FeeOption.h"
#include "TransakDataTypes.h"
#include "SequenceAPI.generated.h"

using FSignature = FUnsizedData;
using TransactionID = FString;

class UIndexer;
class UProvider;

//Sequence Specific Version of Transaction
struct SEQUENCEPLUGIN_API FTransaction_Sequence
{
	uint64 ChainId;
	FAddress From;
	FAddress To;
	TOptional<FString> AutoGas;
	TOptional<uint64> Nonce;
	TOptional<FString> Value;
	TOptional<FString> CallData;
	TOptional<FString> TokenAddress;
	TOptional<FString> TokenAmount;
	TOptional<TArray<FString>> TokenIds;
	TOptional<TArray<FString>> TokenAmounts;

	static FTransaction_Sequence Convert(const FTransaction_FE& Transaction_Fe);
	FString ToJson();
	TransactionID ID();
};

UCLASS()
class SEQUENCEPLUGIN_API USequenceWallet : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;
private:
	UPROPERTY()
	UIndexer* Indexer;

	UPROPERTY()
	UProvider * Provider;

	UPROPERTY()
	FCredentials_BE Credentials;

	FString CachedFeeQuote = "";
	
	const FString Hostname = "https://next-api.sequence.app";
	const FString SequenceURL_Qr = "https://api.sequence.app/qr/";
	const FString SequenceURL = "https://api.sequence.app/rpc/API/";
	const FString Path = "/rpc/Wallet/";
	
	//URL fetchers for sequence services
	FString Url(const FString& Name) const;
	FString getSequenceURL(const FString& endpoint) const;
	static FString encodeB64_URL(const FString& data);

	//Raw request functions
	void SendRPC(const FString& Url, const FString& Content, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure) const;

	//Response helper functions
	static TArray<FContact_BE> BuildFriendListFromJson(const FString& JSON);
	static TArray<FItemPrice_BE> BuildItemUpdateListFromJson(const FString& JSON);
public:
	USequenceWallet();

	/*
	 * Gets the SequenceWallet SubSystem,
	 * this call will attempt to automatically load stored credentials on disk if any.
	 */
	static TOptional<USequenceWallet*> Get();

	/*
	 * Gets the SequenceWallet SubSystem and initialize it with the given credentials
	 */
	static TOptional<USequenceWallet*> Get(const FCredentials_BE& Credentials);

	/*
	 * Gets the SequenceWallet SubSystem and initializes it with the given credentials & ProviderUrl
	 */
	static TOptional<USequenceWallet*> Get(const FCredentials_BE& Credentials, const FString& ProviderUrl);

	/*
	 * Returns the wallet address of the currently signed in user
	 */
	FString GetWalletAddress() const;

	/*
	 * Allows you to update the provider URL set for the SequenceWallet
	 */
	void UpdateProviderURL(const FString& Url) const;

	/*
	 * Allows you to update the set network for the SequenceWallet
	 */
	void UpdateNetworkId(int64 NewNetwork);

	/*
	 * Allows to get the currently set network for the SequenceWallet
	 */
	int64 GetNetworkId() const;

	/*
	 * Allows you to sign the given message with the SequenceWallet
	 * @OnSuccess The returned Struct from the signing process
	 * @OnFailure If an error occurs
	 */
	void SignMessage(const FString& Message, const TSuccessCallback<FSignedMessage>& OnSuccess, const FFailureCallback& OnFailure);

	/*
	 * Allows you to send a transaction that will be automatically gassed IF the token is able to be (not all can be)
	 * @OnSuccess The Semi Struct Parsed JSON response
	 * @OnFailure An error occured during the transaction OR the token provided wasn't able to be automatically gassed
	 */
	void SendTransaction(const TArray<TUnion<FRawTransaction, FERC20Transaction, FERC721Transaction, FERC1155Transaction>>& Transactions, const TSuccessCallback<FTransactionResponse>& OnSuccess, const FFailureCallback& OnFailure);

	/*
	 * Allows you to send a transaction with a given Fee, Use GetFeeOptions Or GetUnfilteredFeeOptions
	 * to pick a valid fee to send with this call
	 * @OnSuccess The Semi Struct Parsed JSON response
	 * @OnFailure An error occured during the transaction
	 */
	void SendTransactionWithFeeOption(TArray<TUnion<FRawTransaction, FERC20Transaction, FERC721Transaction, FERC1155Transaction>> Transactions, FFeeOption FeeOption, const TSuccessCallback<FTransactionResponse>& OnSuccess, const FFailureCallback& OnFailure);

	/*
	 * Allows you to get FeeOptions for the transaction you pass in
	 * @OnSuccess A list of all VALID feeOptions for the presented transaction (can be empty if your wallet contains nothing that can cover any of the feeOptions)
	 * @OnFailure An error occured
	 */
	void GetFeeOptions(const TArray<TUnion<FRawTransaction, FERC20Transaction, FERC721Transaction, FERC1155Transaction>>& Transactions, const TSuccessCallback<TArray<FFeeOption>>& OnSuccess, const FFailureCallback& OnFailure);

	/*
	 * Allows you to see all potential FeeOptions, Valid ones are marked with bCanAfford = true, in the list of FFeeOptions
	 * @OnSuccess A list of all feeOptions with valid ones marked with bCanAfford = true. Possible none are valid if your wallet has nothing to cover the fees
	 * @OnFailure An error occured
	 */
	void GetUnfilteredFeeOptions(const TArray<TUnion<FRawTransaction, FERC20Transaction, FERC721Transaction, FERC1155Transaction>>& Transactions, const TSuccessCallback<TArray<FFeeOption>>& OnSuccess, const FFailureCallback& OnFailure);

	/*
	 * Used to register a Session with Sequence (done automatically)
	 * @OnSuccess The session is registered
	 * @OnFailure An error occured
	 */
	void RegisterSession(const TSuccessCallback<FCredentials_BE>& OnSuccess, const FFailureCallback& OnFailure);

	/*
	 * Used to list all active sessions for the signed in credentials
	 * @OnSuccess A list of all active sessions
	 * @OnFailure An error occured
	 */
	void ListSessions(const TSuccessCallback<TArray<FSession>>& OnSuccess, const FFailureCallback& OnFailure);

	/*
	 * Used to close the current Session with Sequence
	 * @OnSuccess The Session is closed
	 * @OnFailure An error occured
	 */
	void CloseSession(const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure);

	/*
	 * Used to close the current Session with Sequence & clears all locally stored credentials
	 * @OnSuccess The Session is closed & local credentials are removed
	 * @OnFailure An error occured
	 */
	void SignOut();
	void GetSupportedTransakCountries(const TSuccessCallback<TArray<FSupportedCountry>>& OnSuccess, const FFailureCallback& OnFailure);
	void OpenTransakLink(const FString& FiatCurrency = FDefaultTransak::FiatCurrency, const FString& DefaultAmount = FDefaultTransak::FiatAmount, const FString& DefaultCryptoCurrency = FDefaultTransak::CryptoCurrency, const FString& Networks = FDefaultTransak::Networks, bool DisableWalletAddressForm = FDefaultTransak::DisableWalletAddressForm);
private:
	static USequenceWallet * GetSubSystem();
	void Init(const FCredentials_BE& CredentialsIn);
	void Init(const FCredentials_BE& CredentialsIn,const FString& ProviderURL);

	static TArray<FFeeOption> MarkValidFeeOptions(TArray<FFeeOption> FeeOptions, TArray<FFeeOption> BalanceOptions);
	static TArray<FFeeOption> FindValidFeeOptions(const TArray<FFeeOption>& FeeOptions, const TArray<FFeeOption>& BalanceOptions);
	static TArray<FFeeOption> JsonFeeOptionListToFeeOptionList(const TArray<TSharedPtr<FJsonValue>>& FeeOptionList);
	static TArray<FFeeOption> BalancesListToFeeOptionList(const TArray<FTokenBalance>& BalanceList);
	
	FString BuildGetFeeOptionsIntent(const TArray<TUnion<FRawTransaction, FERC20Transaction, FERC721Transaction, FERC1155Transaction>>& Txns) const;
	FString BuildSignMessageIntent(const FString& Message) const;
	FString BuildSendTransactionIntent(const TArray<TUnion<FRawTransaction, FERC20Transaction, FERC721Transaction, FERC1155Transaction>>& Txns) const;
	FString BuildSendTransactionWithFeeIntent(const TArray<TUnion<FRawTransaction, FERC20Transaction, FERC721Transaction, FERC1155Transaction>>& Txns,const FString& FeeQuote) const;
	FString BuildRegisterSessionIntent() const;
	FString BuildListSessionIntent() const;
	FString BuildCloseSessionIntent() const;
	FString BuildSessionValidationIntent() const;
	FString GeneratePacketSignature(const FString& Packet) const;
	template<typename T> FString GenerateIntent(T Data) const;
	
private:
	//these functions are meant for the UI Only and have been removed for this version
	void GetFriends(FString PublicAddress, TSuccessCallback<TArray<FContact_BE>> OnSuccess, const FFailureCallback& OnFailure) const;
	void GetUpdatedCoinPrice(const FID_BE& ItemToUpdate, const TSuccessCallback<TArray<FItemPrice_BE>>& OnSuccess, const FFailureCallback& OnFailure) const;
	void GetUpdatedCoinPrices(TArray<FID_BE> ItemsToUpdate, TSuccessCallback<TArray<FItemPrice_BE>> OnSuccess, const FFailureCallback& OnFailure) const;
	void GetUpdatedCollectiblePrice(const FID_BE& ItemToUpdate, const TSuccessCallback<TArray<FItemPrice_BE>>& OnSuccess, const FFailureCallback& OnFailure) const;
	void GetUpdatedCollectiblePrices(TArray<FID_BE> ItemsToUpdate, TSuccessCallback<TArray<FItemPrice_BE>> OnSuccess, const FFailureCallback& OnFailure) const;
	FString BuildQr_Request_URL(const FString& Data, int32 Size) const;
private:
	template <typename T> void SequenceRPC(FString Url, FString Content, TSuccessCallback<T> OnSuccess, FFailureCallback OnFailure);
public:
	//Indexer Specific Calls
	
	/*
	Used to get a ping back from the Chain
	*/
	void Ping(const TSuccessCallback<bool>& OnSuccess, const FFailureCallback& OnFailure) const;

	/*
		Used to get version data back from the Chain
	*/
	void Version(const TSuccessCallback<FVersion>& OnSuccess, const FFailureCallback& OnFailure) const;

	/*
		Used to get the runtime status of the Chain
	*/
	void RunTimeStatus(const TSuccessCallback<FRuntimeStatus>& OnSuccess, const FFailureCallback& OnFailure) const;

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
	void GetEtherBalance(const FString& AccountAddr, const TSuccessCallback<FEtherBalance>& OnSuccess, const FFailureCallback& OnFailure) const;

	/*
		Gets the token balances from the Chain
	*/
	void GetTokenBalances(const FGetTokenBalancesArgs& Args, const TSuccessCallback<FGetTokenBalancesReturn>& OnSuccess, const FFailureCallback& OnFailure) const;

	/*
		gets the token supplies from the Chain
	*/
	void GetTokenSupplies(const FGetTokenSuppliesArgs& Args, const TSuccessCallback<FGetTokenSuppliesReturn>& OnSuccess, const FFailureCallback& OnFailure) const;

	/*
		gets the token supplies map from the Chain
	*/
	void GetTokenSuppliesMap(const FGetTokenSuppliesMapArgs& Args, const TSuccessCallback<FGetTokenSuppliesMapReturn>& OnSuccess, const FFailureCallback& OnFailure) const;

	/*
		Get the balance updates from the Chain
	*/
	void GetBalanceUpdates(const FGetBalanceUpdatesArgs& Args, const TSuccessCallback<FGetBalanceUpdatesReturn>& OnSuccess, const FFailureCallback& OnFailure) const;

	/*
		get transaction history from the Chain
	*/
	void GetTransactionHistory(const FGetTransactionHistoryArgs& Args, const TSuccessCallback<FGetTransactionHistoryReturn>& OnSuccess, const FFailureCallback& OnFailure) const;
	
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