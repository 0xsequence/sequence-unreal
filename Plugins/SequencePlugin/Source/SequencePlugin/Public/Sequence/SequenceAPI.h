#pragma once
#include "Async.h"
#include "EthTransaction.h"
#include "RPCCaller.h"
#include "BE_Structs.h"
#include "Types/BinaryData.h"

//this can't be in a namespace!
//USTRUCT(BlueprintType)
struct FTransactionData
{
	//GENERATED_USTRUCT_BODY()
public:
	//UPROPERTY(BlueprintReadWrite, EditAnywhere)
		uint64 ChainId;

	//UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FAddress From;

	//UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FAddress To;

	TOptional<FString> AutoGas;
	TOptional<uint64> Nonce;

	//UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TOptional<FString> Value;

	TOptional<FString> CallData;

	//UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TOptional<FString> TokenAddress;

	//UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TOptional<FString> TokenAmount;

	//UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TOptional<TArray<FString>> TokenIds;

	//UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TOptional<TArray<FString>> TokenAmounts;

	const FString ToJson();
	const FString ID();
};

namespace SequenceAPI
{
	using FSignature = FUnsizedData;

	enum ESortOrder {
		DESC,
		ASC
	};

	FString SortOrderToString(ESortOrder SortOrder);
	ESortOrder StringToSortOrder(FString String);

	struct FSortBy
	{
		FString Column;
		ESortOrder Order;

		FString ToJson();
		static FSortBy From(TSharedPtr<FJsonObject> Json);
	};

	struct FPage
	{
		TOptional<uint64> PageSize;
		TOptional<uint64> PageNum;
		TOptional<uint64> TotalRecords;
		TOptional<FString> Column;
		TOptional<TArray<FSortBy>> Sort;

		FString ToJson();
		static FPage From(TSharedPtr<FJsonObject> Json);
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
		FPage Page;
	};
	const static FString sequenceURL_QR = "https://api.sequence.app/qr/";
	class FSequenceWallet : public RPCCaller
	{
	public:
		FString AuthToken = "Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJwYXJ0bmVyX2lkIjoyLCJ3YWxsZXQiOiIweDY2MDI1MDczNGYzMTY0NDY4MWFlMzJkMDViZDdlOGUyOWZlYTI5ZTEifQ.FC8WmaC_hW4svdrs4rxyKcvoekfVYFkFFvGwUOXzcHA";
		const FString Hostname = "https://next-api.sequence.app";
		const FString sequenceURL = "https://api.sequence.app/rpc/API/";
		const FString Path = "/rpc/Wallet/";
		
		//URL fetchers for sequence services
		FString Url(FString Name) const;
		FString getSequenceURL(FString endpoint);

		static FString encodeB64_URL(FString data);
		static FString buildQR_Request_URL(FString data,int32 size);

		//Raw request functions
		virtual void SendRPC(FString Url, FString Content, TSuccessCallback<FString> OnSuccess, FFailureCallback OnFailure) override;

		//Response helper functions
		TArray<FContact_BE> buildFriendListFromJson(FString json);
		TArray<FItemPrice_BE> buildItemUpdateListFromJson(FString json);
		
	public:
		FSequenceWallet(FString Hostname);
		FSequenceWallet();
		
		void CreateWallet(TSuccessCallback<FAddress> OnSuccess, FFailureCallback OnFailure);
		void GetWalletAddress(TSuccessCallback<FAddress> OnSuccess, FFailureCallback OnFailure);
		void DeployWallet(uint64 ChainId, TSuccessCallback<FDeployWalletReturn> OnSuccess, FFailureCallback OnFailure);

		void Wallets(FPage Page, TSuccessCallback<FWalletsReturn> OnSuccess, FFailureCallback OnFailure);
		void Wallets(TSuccessCallback<FWalletsReturn> OnSuccess, FFailureCallback OnFailure);
		void SignMessage(uint64 ChainId, FAddress AccountAddress, FUnsizedData Message, TSuccessCallback<FSignature> OnSuccess, FFailureCallback OnFailure);
		void IsValidMessageSignature(uint64 ChainId, FAddress WalletAddress, FUnsizedData Message, FSignature Signature, TSuccessCallback<bool> OnSuccess, FFailureCallback OnFailure);
		void SendTransaction(FTransactionData Transaction, TSuccessCallback<FHash256> OnSuccess, FFailureCallback OnFailure);
		void SendTransactionBatch(TArray<FTransactionData> Transactions, TSuccessCallback<FHash256> OnSuccess, FFailureCallback OnFailure);

		void SendTransactionWithCallback(FTransactionData Transaction, TSuccessCallback<FString> OnSuccess, FFailureCallback OnFailure);

		void getFriends(FString publicAddress, TSuccessCallback<TArray<FContact_BE>> OnSuccess, FFailureCallback OnFailure);

		void getUpdatedCoinPrice(FID_BE itemToUpdate, TSuccessCallback<TArray<FItemPrice_BE>> OnSuccess, FFailureCallback OnFailure);
		void getUpdatedCoinPrices(TArray<FID_BE> itemsToUpdate, TSuccessCallback<TArray<FItemPrice_BE>> OnSuccess, FFailureCallback OnFailure);

		void getUpdatedCollectiblePrice(FID_BE itemToUpdate, TSuccessCallback<TArray<FItemPrice_BE>> OnSuccess, FFailureCallback OnFailure);
		void getUpdatedCollectiblePrices(TArray<FID_BE> itemsToUpdate, TSuccessCallback<TArray<FItemPrice_BE>> OnSuccess, FFailureCallback OnFailure);
	};
}