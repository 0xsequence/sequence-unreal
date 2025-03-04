// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Util/SequenceSupport.h"
#include "Sequence/SequenceWalletBPTypes.h"
#include "Sequence/Transactions.h"
#include "Sequence/SequenceResponseIntent.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SequenceWalletBP.generated.h"

//ASYNC Response Delegates//

//Api//

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIApiSignMessage, FSequenceResponseStatus, ResponseStatus, FSeqSignMessageResponse_Response, SignedMessage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIApiValidateMessageSignature, FSequenceResponseStatus, ResponseStatus, FSeqValidateMessageSignatureResponse_Data, isValidMessageSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIApiGetFilteredFeeOptions, FSequenceResponseStatus, ResponseStatus, const TArray<FFeeOption>&, FeeOptions);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIApiGetUnFilteredFeeOptions, FSequenceResponseStatus, ResponseStatus, const TArray<FFeeOption>&, FeeOptions);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIApiSendTransactionWtihFeeOption, FSequenceResponseStatus, ResponseStatus, FSeqTransactionResponse_Data, Response);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIApiSendTransaction, FSequenceResponseStatus, ResponseStatus, FSeqTransactionResponse_Data, Response);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIApiGetIdToken, FSequenceResponseStatus, ResponseStatus, FSeqIdTokenResponse_Data, Response);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIApiListSessions, FSequenceResponseStatus, ResponseStatus, const TArray<FSeqListSessions_Session>&, Sessions);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIApiListAccounts, FSequenceResponseStatus, ResponseStatus, const FSeqListAccountsResponse_Data&, Data);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIApiGetSessionAuthProof, FSequenceResponseStatus, ResponseStatus, const FSeqGetSessionAuthProof_Data, Response);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIApiGetSupportedTransakCountries, FSequenceResponseStatus, ResponseStatus, const TArray<FSupportedCountry>&, SupportedCountries);

/**
 * Used for exposing portions of the USequenceWallet
 * to Blueprints for ease of use.
 */
UCLASS(Blueprintable)
class SEQUENCEPLUGIN_API USequenceWalletBP : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	USequenceWalletBP();

	//ASYNC Response Bindable Delegates//

	//Api//
	
	UPROPERTY(BlueprintAssignable, Category="0xSequence SDK - Events")
	FOnIApiSignMessage OnApiSignMessage;

	UPROPERTY(BlueprintAssignable, Category = "0xSequence SDK - Events")
	FOnIApiValidateMessageSignature OnApiValidateMessageSignature;

	UPROPERTY(BlueprintAssignable, Category="0xSequence SDK - Events")
	FOnIApiGetFilteredFeeOptions OnApiGetFilteredFeeOptions;

	UPROPERTY(BlueprintAssignable, Category="0xSequence SDK - Events")
	FOnIApiGetUnFilteredFeeOptions OnApiGetUnFilteredFeeOptions;

	UPROPERTY(BlueprintAssignable, Category="0xSequence SDK - Events")
	FOnIApiSendTransaction OnApiSendEther;
	
	UPROPERTY(BlueprintAssignable, Category="0xSequence SDK - Events")
	FOnIApiSendTransactionWtihFeeOption OnApiSendTransactionWithFeeOption;

	UPROPERTY(BlueprintAssignable, Category="0xSequence SDK - Events")
	FOnIApiSendTransaction OnApiSendTransaction;

	UPROPERTY(BlueprintAssignable, Category="0xSequence SDK - Events")
	FOnIApiGetIdToken OnApiGetIdToken;

	UPROPERTY(BlueprintAssignable, Category="0xSequence SDK - Events")
	FOnIApiListSessions OnApiListSessions;

	UPROPERTY(BlueprintAssignable, Category = "0xSequence SDK - Events")
	FOnIApiListAccounts OnApiListAccounts;

	UPROPERTY(BlueprintAssignable, Category = "0xSequence SDK - Events")
	FOnIApiGetSessionAuthProof OnApiGetSessionAuthProof;

	UPROPERTY(BlueprintAssignable, Category="0xSequence SDK - Events")
	FOnIApiGetSupportedTransakCountries OnApiGetSupportedTransakCountries;
	
	//ASYNC Response Bindable Delegates//
private:

	//ASYNC Response Internal Calls//

	//Api//

	void CallOnApiSignMessage(const FSequenceResponseStatus& Status, const FSeqSignMessageResponse_Response& SignedMessage) const;
	void CallOnApiValidateMessageSignature(const FSequenceResponseStatus& Status, const FSeqValidateMessageSignatureResponse_Data& isValidMessageSignature) const;
	void CallOnApiGetFilteredFeeOptions(const FSequenceResponseStatus& Status, const TArray<FFeeOption>& FeeOptions) const;
	void CallOnApiGetUnFilteredFeeOptions(const FSequenceResponseStatus& Status, const TArray<FFeeOption>& FeeOptions) const;
	void CallOnApiSendEther(const FSequenceResponseStatus& Status, const FSeqTransactionResponse_Data& Response) const;
	void CallOnApiSendTransactionWithFee(const FSequenceResponseStatus& Status, const FSeqTransactionResponse_Data& Response) const;
	void CallOnApiSendTransaction(const FSequenceResponseStatus& Status, const FSeqTransactionResponse_Data& Response) const;
	void CallOnApiGetIdToken(const FSequenceResponseStatus& Status, const FSeqIdTokenResponse_Data& Response) const;
	void CallOnApiListSessions(const FSequenceResponseStatus& Status, const TArray<FSeqListSessions_Session>& Sessions) const;
	void CallOnApiListAccounts(const FSequenceResponseStatus& Status, const FSeqListAccountsResponse_Data& Response) const;
	void CallOnApiGetSessionAuthProof(const FSequenceResponseStatus& Status, const FSeqGetSessionAuthProof_Data Response) const;
	void CallOnApiGetSupportedTransakCountries(const FSequenceResponseStatus& Status, const TArray<FSupportedCountry>& SupportedCountries) const;
	
public:
	//SYNC//

	//Support//

	/*
	* Used to get the amount that users can read,
	* int64 Amount, the integer amount of Crypto
	* int64 Decimals, the integer amount of Decimal places used to represent the Amount
	* Return, The user readable Float amount
	*/
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
	
	//Support//
	
	//SequenceApi//
	
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	FString ApiGetWalletAddress();

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	int64 ApiGetNetworkId();

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void ApiUpdateNetworkId(int64 NewNetworkId);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Functions")
	void ApiUpdateNetworkIdFromName(FString NewNetworkName);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Functions")
	void ApiUpdateNetworkIdFromEnum(ENetwork NewNetwork);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void ApiUpdateProviderUrl(const FString& NewProviderUrl);
	
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void ApiOpenTransakUrl(const FTransakSettings& Settings);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void ApiSignOut();

	//SequenceApi//
	
	//SYNC//
	
	//ASYNC//

	//SequenceApi//

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void ApiSignMessage(const FString& Message);

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Functions")
	void ApiValidateMessageSignature(const int64& ChainId, const FString& WalletAddress, const FString& Message, const FString& Signature);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void ApiGetFilteredFeeOptions(UTransactions * Transactions);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void ApiGetUnfilteredFeeOptions(UTransactions * Transactions);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void ApiSendEther(const FString& RecipientAddress, const FString& Amount);
	
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void ApiSendTransactionWithFee(UTransactions * Transactions);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void ApiSendTransaction(UTransactions * Transactions);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void ApiGetIdToken(const FString& Nonce);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void ApiListSessions();

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Functions")
	void ApiListAccounts();

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Functions")
	void ApiGetSessionAuthProof(const FString& Nonce);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void ApiGetSupportedTransakCountries();
};
