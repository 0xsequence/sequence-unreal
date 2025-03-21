// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

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
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnFailure, const FString&, Error);
	DECLARE_DYNAMIC_DELEGATE(FOnSuccess);
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSession);
	
public:
	USequenceWalletBP();

	UPROPERTY(BlueprintAssignable, Category = "0xSequence SDK - Events")
	FOnSession OnSessionClosed;
	
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	FString GetWalletAddress();

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void UpdateProviderUrl(const FString& NewProviderUrl);
	
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void SignOut();

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void SignMessage(const FString& Message, FOnSignMessage OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Functions")
	void ValidateMessageSignature(const int64& ChainId, const FString& WalletAddress, const FString& Message, const FString& Signature, FOnValidateMessageSignature OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void GetFilteredFeeOptions(UTransactions* Transactions, FOnGetFilteredFeeOptions OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void GetUnfilteredFeeOptions(UTransactions* Transactions, FOnGetUnFilteredFeeOptions OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void SendNativeToken(const FString& RecipientAddress, const FString& Amount, FOnSendTransaction OnSuccess, FOnFailure OnFailure);
	
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void SendTransactionWithFee(UTransactions * Transactions, FOnSendTransaction OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void SendTransaction(UTransactions * Transactions, FOnSendTransaction OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void GetIdToken(const FString& Nonce, FOnGetIdToken OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void ListSessions(FOnListSessions OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Functions")
	void ListAccounts(FOnListAccounts OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Functions")
	void GetSessionAuthProof(const FString& Nonce, FOnGetSessionAuthProof OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void GetLinkedWallets(FOnLinkedWallets OnSuccess, FOnLinkedWalletsFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void RemoveLinkedWallet(const FString& LinkedWalletAddress, FOnSuccess OnSuccess, FOnLinkedWalletsFailure OnFailure);

private:
	UPROPERTY()
	USequenceWallet* Wallet;
	
	void CallOnSessionClosed() const;
};
