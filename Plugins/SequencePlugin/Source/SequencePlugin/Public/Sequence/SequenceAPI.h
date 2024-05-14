// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once
#include "Indexer/Indexer.h"
#include "Authenticator.h"
#include "Util/Async.h"
#include "Eth/EthTransaction.h"
#include "Util/Structs/BE_Structs.h"
#include "Types/BinaryData.h"
#include "Provider.h"
#include "Session.h"
#include "SignedMessage.h"
#include "Containers/Union.h"
#include "TransactionResponse.h"
#include "SequenceAPI.generated.h"

using FSignature = FUnsizedData;
using TransactionID = FString;
	
//Sequence Specific Version of Transaction
struct FTransaction_Sequence
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

	static FTransaction_Sequence Convert(FTransaction_FE Transaction_Fe);
	const FString ToJson();
	const TransactionID ID();
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
	FCredentials_BE Credentials;
	
	FString ProviderUrl = "";
	const FString Hostname = "https://next-api.sequence.app";
	const FString SequenceURL_Qr = "https://api.sequence.app/qr/";
	const FString SequenceURL = "https://api.sequence.app/rpc/API/";
	const FString Path = "/rpc/Wallet/";
	
	//URL fetchers for sequence services
	FString Url(FString Name) const;
	FString getSequenceURL(FString endpoint);
	static FString encodeB64_URL(FString data);

	//Raw request functions
	void SendRPC(FString Url, FString Content, TSuccessCallback<FString> OnSuccess, FFailureCallback OnFailure);

	//Response helper functions
	TArray<FContact_BE> BuildFriendListFromJson(FString JSON);
	TArray<FItemPrice_BE> BuildItemUpdateListFromJson(FString JSON);
public:
	USequenceWallet();
	
	static TOptional<USequenceWallet*> Get();
	static TOptional<USequenceWallet*> Get(const FCredentials_BE& Credentials);
	static TOptional<USequenceWallet*> Get(const FCredentials_BE& Credentials, const FString& ProviderUrl);

	FString GetWalletAddress();
	void UpdateProviderURL(const FString& Url);
	void UpdateNetworkId(int64 NewNetwork);
	int64 GetNetworkId();
	void SignMessage(const FString& Message, const TSuccessCallback<FSignedMessage>& OnSuccess, const FFailureCallback& OnFailure);
	void SendTransaction(TArray<TUnion<FRawTransaction, FERC20Transaction, FERC721Transaction, FERC1155Transaction>> Transactions, TSuccessCallback<FTransactionResponse> OnSuccess, FFailureCallback OnFailure);
	void RegisterSession(const TSuccessCallback<FCredentials_BE>& OnSuccess, const FFailureCallback& OnFailure);
	void ListSessions(const TSuccessCallback<TArray<FSession>>& OnSuccess, const FFailureCallback& OnFailure);
	void CloseSession(const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure);
	void SignOut();
private:
	void Init(const FCredentials_BE& CredentialsIn);
	void Init(const FCredentials_BE& CredentialsIn,const FString& ProviderURL);
	FString BuildSignMessageIntent(const FString& Message);
	FString BuildSendTransactionIntent(const FString& Txns);
	FString BuildRegisterSessionIntent();
	FString BuildListSessionIntent();
	FString BuildCloseSessionIntent();
	FString BuildSessionValidationIntent();
	FString GeneratePacketSignature(const FString& Packet) const;
	
private:
	//these functions are meant for the UI Only and have been removed for this version
	void GetFriends(FString PublicAddress, TSuccessCallback<TArray<FContact_BE>> OnSuccess, FFailureCallback OnFailure);
	void GetUpdatedCoinPrice(FID_BE ItemToUpdate, TSuccessCallback<TArray<FItemPrice_BE>> OnSuccess, FFailureCallback OnFailure);
	void GetUpdatedCoinPrices(TArray<FID_BE> ItemsToUpdate, TSuccessCallback<TArray<FItemPrice_BE>> OnSuccess, FFailureCallback OnFailure);
	void GetUpdatedCollectiblePrice(FID_BE ItemToUpdate, TSuccessCallback<TArray<FItemPrice_BE>> OnSuccess, FFailureCallback OnFailure);
	void GetUpdatedCollectiblePrices(TArray<FID_BE> ItemsToUpdate, TSuccessCallback<TArray<FItemPrice_BE>> OnSuccess, FFailureCallback OnFailure);
	FString BuildQr_Request_URL(FString Data, int32 Size);
private:
	template <typename T> void SequenceRPC(FString Url, FString Content, TSuccessCallback<T> OnSuccess, FFailureCallback OnFailure);
public:
	//Indexer Specific Calls
	
	/*
	Used to get a ping back from the Chain
	*/
	void Ping(TSuccessCallback<bool> OnSuccess, FFailureCallback OnFailure);

	/*
		Used to get version data back from the Chain
	*/
	void Version(TSuccessCallback<FVersion> OnSuccess, FFailureCallback OnFailure);

	/*
		Used to get the runtime status of the Chain
	*/
	void RunTimeStatus(TSuccessCallback<FRuntimeStatus> OnSuccess, FFailureCallback OnFailure);

	/*
		Used to get the chainID from the Chain
	*/
	void GetChainID(TSuccessCallback<int64> OnSuccess, FFailureCallback OnFailure);

	/*
		Used to get the Ether balance from the Chain
		@param 1st the ChainID
		@param 2nd the accountAddr we want to get the balance for
		@return the Balance ASYNC calls (update ether balance in the bck_mngr when done processing)
	*/
	void GetEtherBalance(FString AccountAddr, TSuccessCallback<FEtherBalance> OnSuccess, FFailureCallback OnFailure);

	/*
		Gets the token balances from the Chain
	*/
	void GetTokenBalances(FGetTokenBalancesArgs Args, TSuccessCallback<FGetTokenBalancesReturn> OnSuccess, FFailureCallback OnFailure);

	/*
		gets the token supplies from the Chain
	*/
	void GetTokenSupplies(FGetTokenSuppliesArgs Args, TSuccessCallback<FGetTokenSuppliesReturn> OnSuccess, FFailureCallback OnFailure);

	/*
		gets the token supplies map from the Chain
	*/
	void GetTokenSuppliesMap(FGetTokenSuppliesMapArgs Args, TSuccessCallback<FGetTokenSuppliesMapReturn> OnSuccess, FFailureCallback OnFailure);

	/*
		Get the balance updates from the Chain
	*/
	void GetBalanceUpdates(FGetBalanceUpdatesArgs Args, TSuccessCallback<FGetBalanceUpdatesReturn> OnSuccess, FFailureCallback OnFailure);

	/*
		get transaction history from the Chain
	*/
	void GetTransactionHistory(FGetTransactionHistoryArgs Args, TSuccessCallback<FGetTransactionHistoryReturn> OnSuccess, FFailureCallback OnFailure);
	
	//Provider calls

	void BlockByNumber(uint64 Number, TFunction<void(TSharedPtr<FJsonObject>)> OnSuccess,
	                   TFunction<void(FSequenceError)> OnFailure);
	void BlockByNumber(EBlockTag Tag, TFunction<void(TSharedPtr<FJsonObject>)> OnSuccess,
	                   TFunction<void(FSequenceError)> OnFailure);
	void BlockByHash(FHash256 Hash, TFunction<void(TSharedPtr<FJsonObject>)> OnSuccess,
	                 TFunction<void(FSequenceError)> OnFailure);
	void BlockNumber(TFunction<void(uint64)> OnSuccess, TFunction<void(FSequenceError)> OnFailure);

	void HeaderByNumber(uint64 Id, TFunction<void(FHeader)> OnSuccess, TFunction<void(FSequenceError)> OnFailure);
	void HeaderByNumber(EBlockTag Tag, TFunction<void(FHeader)> OnSuccess, TFunction<void(FSequenceError)> OnFailure);
	void HeaderByHash(FHash256 Hash, TFunction<void(FHeader)> OnSuccess, TFunction<void(FSequenceError)> OnFailure);

	void TransactionByHash(FHash256 Hash, TFunction<void(TSharedPtr<FJsonObject>)> OnSuccess,
	                       TFunction<void(FSequenceError)> OnFailure);
	void TransactionCount(FAddress Addr, uint64 Number, TFunction<void(uint64)> OnSuccess,
	                      TFunction<void(FSequenceError)> OnFailure);
	void TransactionCount(FAddress Addr, EBlockTag Tag, TFunction<void(uint64)> OnSuccess,
	                      TFunction<void(FSequenceError)> OnFailure);
	void TransactionReceipt(FHash256 Hash, TFunction<void(FTransactionReceipt)> OnSuccess,
	                        TFunction<void(FSequenceError)> OnFailure);

	void GetGasPrice(TFunction<void(FUnsizedData)> OnSuccess, TFunction<void(FSequenceError)> OnFailure);
	void EstimateContractCallGas(FContractCall ContractCall, TFunction<void(FUnsizedData)> OnSuccess,
	                             TFunction<void(FSequenceError)> OnFailure);

	//deprecated//
	void EstimateDeploymentGas(FAddress From, FString Bytecode, TFunction<void(FUnsizedData)> OnSuccess,
	                           TFunction<void(FSequenceError)> OnFailure);

	void DeployContract(FString Bytecode, FPrivateKey PrivKey, int64 ChainId, TFunction<void(FAddress)> OnSuccess,
	                    TFunction<void(FSequenceError)> OnFailure);
	void DeployContractWithHash(FString Bytecode, FPrivateKey PrivKey, int64 ChainId,
	                            TFunction<void(FAddress, FUnsizedData)> OnSuccess,
	                            TFunction<void(FSequenceError)> OnFailure);
	//deprecated//
	
	void NonceAt(uint64 Number, TFunction<void(FBlockNonce)> OnSuccess, TFunction<void(FSequenceError)> OnFailure);
	void NonceAt(EBlockTag Tag, TFunction<void(FBlockNonce)> OnSuccess, TFunction<void(FSequenceError)> OnFailure);
	void SendRawTransaction(FString Data, TFunction<void(FUnsizedData)> OnSuccess,
	                        TFunction<void(FSequenceError)> OnFailure);

	void ChainId(TFunction<void(uint64)> OnSuccess, TFunction<void(FSequenceError)> OnFailure);

	void Call(FContractCall ContractCall, uint64 Number, TFunction<void(FUnsizedData)> OnSuccess,
	          TFunction<void(FSequenceError)> OnFailure);
	void Call(FContractCall ContractCall, EBlockTag Number, TFunction<void(FUnsizedData)> OnSuccess,
	          TFunction<void(FSequenceError)> OnFailure);
	void NonViewCall(FEthTransaction Transaction, FPrivateKey PrivateKey, int ChainID,
	                 TFunction<void(FUnsizedData)> OnSuccess, TFunction<void(FSequenceError)> OnFailure);
};