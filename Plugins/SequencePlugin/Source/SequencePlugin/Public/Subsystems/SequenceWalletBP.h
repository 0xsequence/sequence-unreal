// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Util/SequenceSupport.h"
#include "Sequence/SequenceWalletBPTypes.h"
#include "Sequence/Transactions.h"
#include "Sequence/SequenceResponseIntent.h"
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
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnSendTransactionWtihFeeOption, FSeqTransactionResponse_Data, Response);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnSendTransaction, FSeqTransactionResponse_Data, Response);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetIdToken, FSeqIdTokenResponse_Data, Response);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnListSessions, const TArray<FSeqListSessions_Session>&, Sessions);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnListAccounts, const FSeqListAccountsResponse_Data&, Data);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetSessionAuthProof, const FSeqGetSessionAuthProof_Data, Response);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnFailure, const FString&, Error);
	
public:
	USequenceWalletBP();

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	float GetUserReadableAmountIntDecimals(int64 Amount, int64 Decimals);

	/*
	* Used to convert a user readable amount into a usable amount value for transactions,
	* float Amount, the user readable amount they wish to use, EG) 0.01 Eth
	* int64 Decimals, the int64 amount of Decimal places used to represent the Amount
	* Return, The transaction usable amount
	*/
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	int64 GetTransactionReadableAmountIntDecimals(float Amount, int64 Decimals);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Static")
	static int64 GetNetworkIdFromName(const FString& NetworkNameIn);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Static")
	static int64 GetNetworkIdFromNetworkEnum(const ENetwork& NetworkEnumIn);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Static")
	static FString GetNetworkNameFromId(const int64 NetworkIdIn);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Static")
	static FString GetNetworkNameFromEnum(const ENetwork NetworkIdIn);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Static")
	static TArray<FIdNamePair> GetAllNetworks();

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Static")
	static TArray<FString> GetAllNetworkNames();

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Static")
	static TArray<int64> GetAllNetworkIds();
	
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	FString GetWalletAddress();

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	int64 GetNetworkId();

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void UpdateNetworkId(int64 NewNetworkId);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Functions")
	void UpdateNetworkIdFromName(FString NewNetworkName);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Functions")
	void UpdateNetworkIdFromEnum(ENetwork NewNetwork);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void UpdateProviderUrl(const FString& NewProviderUrl);
	
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void SignOut();

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void SignMessage(const FString& Message, FOnSignMessage OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Functions")
	void ValidateMessageSignature(const int64& ChainId, const FString& WalletAddress, const FString& Message, const FString& Signature, FOnValidateMessageSignature OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void GetFilteredFeeOptions(UTransactions * Transactions, FOnGetFilteredFeeOptions OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void GetUnfilteredFeeOptions(UTransactions * Transactions, FOnGetUnFilteredFeeOptions OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void SendNativeToken(const FString& RecipientAddress, const FString& Amount, FOnSendTransaction OnSuccess, FOnFailure OnFailure);
	
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void SendTransactionWithFee(UTransactions * Transactions, FOnSendTransactionWtihFeeOption OnSuccess, FOnFailure OnFailure);

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
};
