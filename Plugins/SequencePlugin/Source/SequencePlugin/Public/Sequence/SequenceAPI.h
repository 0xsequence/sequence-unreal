#pragma once
#include "Indexer/Indexer.h"
#include "Authenticator.h"
#include "Util/Async.h"
#include "Eth/EthTransaction.h"
#include "RPCCaller.h"
#include "Util/Structs/BE_Structs.h"
#include "Types/BinaryData.h"
#include "Indexer/Indexer_Enums.h"
#include "Indexer/Structs/SortBy.h"
#include "Indexer/Structs/Page.h"
#include "Provider.h"

namespace SequenceAPI
{
	using FSignature = FUnsizedData;

	FString SortOrderToString(ESortOrder SortOrder);
	ESortOrder StringToSortOrder(FString String);

	//Sequence Specific version of Page
	struct FPage_Sequence
	{
		TOptional<uint64> PageSize;
		TOptional<uint64> PageNum;
		TOptional<uint64> TotalRecords;
		TOptional<FString> Column;
		TOptional<TArray<FSortBy>> Sort;

		FString ToJson();
		static FPage_Sequence From(TSharedPtr<FJsonObject> Json);
		static FPage_Sequence Convert(FPage Page,int64 TotalRecords);
	};

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

	struct FPartnerWallet
	{
		uint64 Number;
		uint64 PartnerId;
		uint64 WalletIndex;
		FString WalletAddress;

		static FPartnerWallet From(TSharedPtr<FJsonObject> Json);
	};

	struct FDeployWalletReturn
	{
		FString Address;
		FString TransactionHash;
	};

	struct FWalletsReturn
	{
		TArray<FPartnerWallet> Wallets;
		FPage_Sequence Page;
	};

	const static FString sequenceURL_QR = "https://api.sequence.app/qr/";
	class FSequenceWallet : public RPCCaller
	{
	private:
		FCredentials_BE Credentials;
		UIndexer* Indexer;
		FString ProviderUrl = "https://temp";
		//Replace this with credentials
		FString AuthToken = "Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJwYXJ0bmVyX2lkIjoyLCJ3YWxsZXQiOiIweDY2MDI1MDczNGYzMTY0NDY4MWFlMzJkMDViZDdlOGUyOWZlYTI5ZTEifQ.FC8WmaC_hW4svdrs4rxyKcvoekfVYFkFFvGwUOXzcHA";
		const FString Hostname = "https://next-api.sequence.app";
		const FString sequenceURL = "https://api.sequence.app/rpc/API/";
		const FString Path = "/rpc/Wallet/";
		
		//URL fetchers for sequence services
		FString Url(FString Name) const;
		FString getSequenceURL(FString endpoint);
		static FString encodeB64_URL(FString data);

		//Raw request functions
		virtual void SendRPC(FString Url, FString Content, TSuccessCallback<FString> OnSuccess, FFailureCallback OnFailure) override;

		//Response helper functions
		TArray<FContact_BE> buildFriendListFromJson(FString json);
		TArray<FItemPrice_BE> buildItemUpdateListFromJson(FString json);
	public:
		FSequenceWallet();
		FSequenceWallet(const FCredentials_BE& CredentialsIn);
		FSequenceWallet(const FCredentials_BE& CredentialsIn,const FString& ProviderURL);

		void SignMessage(const FString& Message, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure);

		void SendSequenceTransaction(FSequenceTransaction,TSuccessCallback<FString> OnSuccess, FFailureCallback OnFailure);
		void SendERC20Transaction(FERC20Transaction,TSuccessCallback<FString> OnSuccess, FFailureCallback OnFailure);
		void SendERC721Transaction(FERC721Transaction,TSuccessCallback<FString> OnSuccess, FFailureCallback OnFailure);
		void SendERC1155Transaction(FERC1155Transaction,TSuccessCallback<FString> OnSuccess, FFailureCallback OnFailure);

		void RegisterSession(const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure);
		void ListSessions(const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure);
		void CloseSession(const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure);

		//NOP
		void SessionValidation(const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure);
	private:
		FString BuildSignMessageIntent(const FString& message);
		FString BuildSendTransactionIntent(const FString& Identifier,const FString& Txns);
		FString BuildRegisterSessionIntent();
		FString BuildListSessionIntent();
		FString BuildCloseSessionIntent();
		
		//NOP
		FString BuildSessionValidationIntent();
		FString GeneratePacketSignature(const FString& Packet) const;
		FString GenerateSignedEncryptedPayload(const FString& Intent) const;
		FString GenerateSignedEncryptedRegisterSessionPayload(const FString& Intent) const;
		FString SignAndEncryptPayload(const FString& PrePayload, const FString& Intent) const;
		
	private://these are all out of scope
		void SendTransaction(FTransaction_Sequence Transaction, TSuccessCallback<FHash256> OnSuccess, FFailureCallback OnFailure);
		FString GetWalletAddress();
		void SendTransactionBatch(TArray<FTransaction_Sequence> Transactions, TSuccessCallback<FHash256> OnSuccess, FFailureCallback OnFailure);
		void IsValidMessageSignature(uint64 ChainId, FAddress WalletAddress, FUnsizedData Message, FSignature Signature, TSuccessCallback<bool> OnSuccess, FFailureCallback OnFailure);
		void CreateWallet(TSuccessCallback<FAddress> OnSuccess, FFailureCallback OnFailure);
		void GetWalletAddress(TSuccessCallback<FAddress> OnSuccess, FFailureCallback OnFailure);
		void DeployWallet(uint64 ChainId, TSuccessCallback<FDeployWalletReturn> OnSuccess, FFailureCallback OnFailure);
		void Wallets(FPage_Sequence Page, TSuccessCallback<FWalletsReturn> OnSuccess, FFailureCallback OnFailure);
		void Wallets(TSuccessCallback<FWalletsReturn> OnSuccess, FFailureCallback OnFailure);
	private:
		//these functions are meant for the UI Only and have been removed for this version
		void getFriends(FString publicAddress, TSuccessCallback<TArray<FContact_BE>> OnSuccess, FFailureCallback OnFailure);
		void getUpdatedCoinPrice(FID_BE itemToUpdate, TSuccessCallback<TArray<FItemPrice_BE>> OnSuccess, FFailureCallback OnFailure);
		void getUpdatedCoinPrices(TArray<FID_BE> itemsToUpdate, TSuccessCallback<TArray<FItemPrice_BE>> OnSuccess, FFailureCallback OnFailure);
		void getUpdatedCollectiblePrice(FID_BE itemToUpdate, TSuccessCallback<TArray<FItemPrice_BE>> OnSuccess, FFailureCallback OnFailure);
		void getUpdatedCollectiblePrices(TArray<FID_BE> itemsToUpdate, TSuccessCallback<TArray<FItemPrice_BE>> OnSuccess, FFailureCallback OnFailure);
		static FString buildQR_Request_URL(FString data, int32 size);
		void SendTransactionWithCallback(FTransaction_FE Transaction, TSuccessCallback<TransactionID> OnSuccess, TFunction<void(TransactionID, FSequenceError)> OnFailure);
	private:
		template <typename T> void SequenceRPC(FString Url, FString Content, TSuccessCallback<T> OnSuccess, FFailureCallback OnFailure);
	public:
		//Indexer Specific Calls
		
		/*
		Used to get a ping back from the sequence app
		*/
		void Ping(int64 chainID, TSuccessCallback<bool> OnSuccess, FFailureCallback OnFailure);

		/*
			Used to get version data back from the sequence app
		*/
		void Version(int64 chainID, TSuccessCallback<FVersion> OnSuccess, FFailureCallback OnFailure);

		/*
			Used to get the runtime status of the sequence app
		*/
		void RunTimeStatus(int64 chainID, TSuccessCallback<FRuntimeStatus> OnSuccess, FFailureCallback OnFailure);

		/*
			Used to get the chainID from the sequence app
		*/
		void GetChainID(int64 chainID, TSuccessCallback<int64> OnSuccess, FFailureCallback OnFailure);

		/*
			Used to get the Ether balance from the sequence app
			@param 1st the ChainID
			@param 2nd the accountAddr we want to get the balance for
			@return the Balance ASYNC calls (update ether balance in the bck_mngr when done processing)
		*/
		void GetEtherBalance(int64 chainID, FString accountAddr, TSuccessCallback<FEtherBalance> OnSuccess, FFailureCallback OnFailure);

		/*
			Gets the token balances from the sequence app
		*/
		void GetTokenBalances(int64 chainID, FGetTokenBalancesArgs args, TSuccessCallback<FGetTokenBalancesReturn> OnSuccess, FFailureCallback OnFailure);

		/*
			gets the token supplies from the sequence app
		*/
		void GetTokenSupplies(int64 chainID, FGetTokenSuppliesArgs args, TSuccessCallback<FGetTokenSuppliesReturn> OnSuccess, FFailureCallback OnFailure);

		/*
			gets the token supplies map from the sequence app
		*/
		void GetTokenSuppliesMap(int64 chainID, FGetTokenSuppliesMapArgs args, TSuccessCallback<FGetTokenSuppliesMapReturn> OnSuccess, FFailureCallback OnFailure);

		/*
			Get the balance updates from the sequence app
		*/
		void GetBalanceUpdates(int64 chainID, FGetBalanceUpdatesArgs args, TSuccessCallback<FGetBalanceUpdatesReturn> OnSuccess, FFailureCallback OnFailure);

		/*
			get transaction history from the sequence app
		*/
		void GetTransactionHistory(int64 chainID, FGetTransactionHistoryArgs args, TSuccessCallback<FGetTransactionHistoryReturn> OnSuccess, FFailureCallback OnFailure);
		
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
		void NonViewCall(FEthTransaction transaction, FPrivateKey PrivateKey, int ChainID, TSuccessCallback<FUnsizedData> OnSuccess, FFailureCallback OnFailure);
	};
}