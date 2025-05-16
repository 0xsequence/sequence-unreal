#pragma once

#include "CoreMinimal.h"
#include "Util/SequenceSupport.h"
#include "Sequence/SequenceWalletBPTypes.h"
#include "Sequence/Transactions.h"
#include "Sequence/SequenceResponseIntent.h"
#include "Sequence/SequenceWallet.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SequenceWalletBP.generated.h"

UCLASS(Blueprintable)
class SEQUENCEPLUGIN_API USequenceWalletBP : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnSignMessage, const FString&, SignedMessage);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnValidateMessageSignature, bool, isValid);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetFilteredFeeOptions, const TArray<FFeeOption>&, FeeOptions);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetUnFilteredFeeOptions, const TArray<FFeeOption>&, FeeOptions);
	DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnSendTransaction, FString, TxnReceipt, FSeqTransactionResponse_Data, TransactionData);
	DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnGetIdToken, FString, IdToken, int32, ExpiresIn);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnListSessions, const TArray<FSeqListSessions_Session>&, Sessions);
	DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnListAccounts, const TArray<FSeqAccount>&, Accounts, FString, CurrentAccountId);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetSessionAuthProof, const FSeqGetSessionAuthProof_Data, Response);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnLinkedWallets, const FSeqLinkedWalletsResponse&, LinkedWallets);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnLinkedWalletsFailure, const FString&, Error);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnCallResponse, const FString&, EncodedData);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnFailure, const FString&, Error);
	DECLARE_DYNAMIC_DELEGATE(FOnSuccess);
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSession);
	
	USequenceWalletBP();

	UPROPERTY(BlueprintAssignable, Category = "0xSequence SDK - Wallet")
	FOnSession OnSessionClosed;
	
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Wallet")
	FString GetWalletAddress();

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Wallet")
	void UpdateProviderUrl(const FString& NewProviderUrl);
	
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Wallet")
	void SignOut();

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Wallet")
	void SignMessage(const FString& Message, FOnSignMessage OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Wallet")
	void ValidateMessageSignature(const int64& ChainId, const FString& WalletAddress, const FString& Message, const FString& Signature, FOnValidateMessageSignature OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Wallet")
	void GetFilteredFeeOptions(UTransactions* Transactions, FOnGetFilteredFeeOptions OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Wallet")
	void GetUnfilteredFeeOptions(UTransactions* Transactions, FOnGetUnFilteredFeeOptions OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Wallet")
	void SendNativeToken(const FString& RecipientAddress, const FString& Amount, FOnSendTransaction OnSuccess, FOnFailure OnFailure);
	
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Wallet")
	void SendTransactionWithFee(UTransactions * Transactions, FOnSendTransaction OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Wallet")
	void SendTransaction(UTransactions * Transactions, FOnSendTransaction OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Wallet")
	void GetIdToken(const FString& Nonce, FOnGetIdToken OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Wallet")
	void ListSessions(FOnListSessions OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Wallet")
	void ListAccounts(FOnListAccounts OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Wallet")
	void GetSessionAuthProof(const FString& Nonce, FOnGetSessionAuthProof OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Wallet")
	void GetLinkedWallets(FOnLinkedWallets OnSuccess, FOnLinkedWalletsFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Wallet")
	void RemoveLinkedWallet(const FString& LinkedWalletAddress, FOnSuccess OnSuccess, FOnLinkedWalletsFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void Call(const FContractCall& ContractCall, FOnCallResponse OnSuccess, FOnLinkedWalletsFailure OnFailure);

	USequenceWallet* GetSequenceWallet() const;
	
private:
	UPROPERTY()
	USequenceWallet* Wallet;
	
	void CallOnSessionClosed() const;
};
