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
	
	static TOptional<USequenceWallet*> Get();
	static TOptional<USequenceWallet*> Get(const FCredentials_BE& Credentials);
	static TOptional<USequenceWallet*> Get(const FCredentials_BE& Credentials, const FString& ProviderUrl);

	FString GetWalletAddress() const;
	void UpdateProviderURL(const FString& Url) const;
	void UpdateNetworkId(int64 NewNetwork);
	int64 GetNetworkId() const;
	void SignMessage(const FString& Message, const TSuccessCallback<FSignedMessage>& OnSuccess, const FFailureCallback& OnFailure);
	void SendTransaction(TArray<TUnion<FRawTransaction, FERC20Transaction, FERC721Transaction, FERC1155Transaction>> Transactions, const TSuccessCallback<FTransactionResponse>& OnSuccess, const FFailureCallback& OnFailure);
	void RegisterSession(const TSuccessCallback<FCredentials_BE>& OnSuccess, const FFailureCallback& OnFailure);
	void ListSessions(const TSuccessCallback<TArray<FSession>>& OnSuccess, const FFailureCallback& OnFailure);
	void CloseSession(const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure);
	void SignOut();
private:
	static USequenceWallet * GetSubSystem();
	void Init(const FCredentials_BE& CredentialsIn);
	void Init(const FCredentials_BE& CredentialsIn,const FString& ProviderURL);
	FString BuildSignMessageIntent(const FString& Message) const;
	FString BuildSendTransactionIntent(const FString& Txns) const;
	FString BuildRegisterSessionIntent() const;
	FString BuildListSessionIntent() const;
	FString BuildCloseSessionIntent() const;
	FString BuildSessionValidationIntent() const;
	FString GeneratePacketSignature(const FString& Packet) const;
	
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

	//deprecated//
	void EstimateDeploymentGas(const FAddress& From, const FString& Bytecode, const TFunction<void(FUnsizedData)>& OnSuccess,
	                           const TFunction<void(FSequenceError)>& OnFailure) const;

	void DeployContract(const FString& Bytecode, const FPrivateKey& PrivKey, int64 ChainId, const TFunction<void(FAddress)>& OnSuccess,
	                    const TFunction<void(FSequenceError)>& OnFailure) const;
	void DeployContractWithHash(const FString& Bytecode, const FPrivateKey& PrivKey, int64 ChainId,
	                            const TFunction<void(FAddress, FUnsizedData)>& OnSuccess,
	                            const TFunction<void(FSequenceError)>& OnFailure) const;
	//deprecated//
	
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