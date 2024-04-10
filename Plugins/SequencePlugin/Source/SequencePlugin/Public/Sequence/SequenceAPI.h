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
class SEQUENCEPLUGIN_API USequenceWallet : public UObject
{
	GENERATED_BODY()
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
	
	static USequenceWallet* Make(const FCredentials_BE& CredentialsIn);
	static USequenceWallet* Make(const FCredentials_BE& CredentialsIn,const FString& ProviderURL);

	FString GetWalletAddress();
	void UpdateProviderURL(const FString& Url);
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
	void Ping(int64 ChainID, TSuccessCallback<bool> OnSuccess, FFailureCallback OnFailure);

	/*
		Used to get version data back from the Chain
	*/
	void Version(int64 ChainID, TSuccessCallback<FVersion> OnSuccess, FFailureCallback OnFailure);

	/*
		Used to get the runtime status of the Chain
	*/
	void RunTimeStatus(int64 ChainID, TSuccessCallback<FRuntimeStatus> OnSuccess, FFailureCallback OnFailure);

	/*
		Used to get the chainID from the Chain
	*/
	void GetChainID(int64 ChainID, TSuccessCallback<int64> OnSuccess, FFailureCallback OnFailure);

	/*
		Used to get the Ether balance from the Chain
		@param 1st the ChainID
		@param 2nd the accountAddr we want to get the balance for
		@return the Balance ASYNC calls (update ether balance in the bck_mngr when done processing)
	*/
	void GetEtherBalance(int64 ChainID, FString AccountAddr, TSuccessCallback<FEtherBalance> OnSuccess, FFailureCallback OnFailure);

	/*
		Gets the token balances from the Chain
	*/
	void GetTokenBalances(int64 ChainID, FGetTokenBalancesArgs Args, TSuccessCallback<FGetTokenBalancesReturn> OnSuccess, FFailureCallback OnFailure);

	/*
		gets the token supplies from the Chain
	*/
	void GetTokenSupplies(int64 ChainID, FGetTokenSuppliesArgs Args, TSuccessCallback<FGetTokenSuppliesReturn> OnSuccess, FFailureCallback OnFailure);

	/*
		gets the token supplies map from the Chain
	*/
	void GetTokenSuppliesMap(int64 ChainID, FGetTokenSuppliesMapArgs Args, TSuccessCallback<FGetTokenSuppliesMapReturn> OnSuccess, FFailureCallback OnFailure);

	/*
		Get the balance updates from the Chain
	*/
	void GetBalanceUpdates(int64 ChainID, FGetBalanceUpdatesArgs Args, TSuccessCallback<FGetBalanceUpdatesReturn> OnSuccess, FFailureCallback OnFailure);

	/*
		get transaction history from the Chain
	*/
	void GetTransactionHistory(int64 ChainID, FGetTransactionHistoryArgs Args, TSuccessCallback<FGetTransactionHistoryReturn> OnSuccess, FFailureCallback OnFailure);
	
	//Provider calls

	void BlockByNumber(uint64 Number, TSuccessCallback<TSharedPtr<FJsonObject>> OnSuccess, FFailureCallback OnFailure);
	void BlockByNumber(EBlockTag Tag, TSuccessCallback<TSharedPtr<FJsonObject>> OnSuccess, FFailureCallback OnFailure);
	void BlockByHash(FHash256 Hash, TSuccessCallback<TSharedPtr<FJsonObject>> OnSuccess, FFailureCallback OnFailure);
	void BlockNumber(TSuccessCallback<uint64> OnSuccess, FFailureCallback OnFailure);

	void HeaderByNumber(uint64 Id, TSuccessCallback<FHeader> OnSuccess, FFailureCallback OnFailure);
	void HeaderByNumber(EBlockTag Tag, TSuccessCallback<FHeader> OnSuccess, FFailureCallback OnFailure);
	void HeaderByHash(FHash256 Hash, TSuccessCallback<FHeader> OnSuccess, FFailureCallback OnFailure);

	void TransactionByHash(FHash256 Hash, TSuccessCallback<TSharedPtr<FJsonObject>> OnSuccess, FFailureCallback OnFailure);
	void TransactionCount(FAddress Addr, uint64 Number, TSuccessCallback<uint64> OnSuccess, FFailureCallback OnFailure);
	void TransactionCount(FAddress Addr, EBlockTag Tag, TSuccessCallback<uint64> OnSuccess, FFailureCallback OnFailure);
	void TransactionReceipt(FHash256 Hash, TSuccessCallback<FTransactionReceipt> OnSuccess, FFailureCallback OnFailure);

	void GetGasPrice(TSuccessCallback<FUnsizedData> OnSuccess, FFailureCallback OnFailure);
	void EstimateContractCallGas(FContractCall ContractCall, TSuccessCallback<FUnsizedData> OnSuccess, FFailureCallback OnFailure);
	void EstimateDeploymentGas(FAddress From, FString Bytecode, TSuccessCallback<FUnsizedData> OnSuccess, FFailureCallback OnFailure);

	void DeployContract(FString Bytecode, FPrivateKey PrivKey, int64 ChainId, TSuccessCallback<FAddress> OnSuccess, FFailureCallback OnFailure);
	void DeployContractWithHash(FString Bytecode, FPrivateKey PrivKey, int64 ChainId, TSuccessCallbackTuple<FAddress, FUnsizedData> OnSuccess, FFailureCallback OnFailure);

	void NonceAt(uint64 Number, TSuccessCallback<FBlockNonce> OnSuccess, FFailureCallback OnFailure);
	void NonceAt(EBlockTag Tag, TSuccessCallback<FBlockNonce> OnSuccess, FFailureCallback OnFailure);
	void SendRawTransaction(FString Data, TSuccessCallback<FUnsizedData> OnSuccess, FFailureCallback OnFailure);

	void ChainId(TSuccessCallback<uint64> OnSuccess, FFailureCallback OnFailure);

	void Call(FContractCall ContractCall, uint64 Number, TSuccessCallback<FUnsizedData> OnSuccess, FFailureCallback OnFailure);
	void Call(FContractCall ContractCall, EBlockTag Number, TSuccessCallback<FUnsizedData> OnSuccess, FFailureCallback OnFailure);
	void NonViewCall(FEthTransaction Transaction, FPrivateKey PrivateKey, int ChainID, TSuccessCallback<FUnsizedData> OnSuccess, FFailureCallback OnFailure);
};