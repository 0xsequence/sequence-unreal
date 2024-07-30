// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Credentials.h"
#include "Sequence/FeeOption.h"
#include "Sequence/SequenceIntent.h"
#include "Sequence/Session.h"
#include "Sequence/SignedMessage.h"
#include "Sequence/TransactionResponse.h"
#include "UObject/Object.h"
#include "Types/Types.h"
#include "Types/Wallet.h"
#include "Util/Async.h"
#include "SequenceRPCManager.generated.h"

/**
 * Used to manage all of the Sequence RPC Calls
 */
UCLASS()
class SEQUENCEPLUGIN_API USequenceRPCManager : public UObject
{
	GENERATED_BODY()
private:

	//Vars//

	FWaasJWT WaaSSettings;

	FString ProjectAccessKey = "";

	UPROPERTY()
	UWallet * SessionWallet = nullptr;

	inline static const FString WaaSVersion = "1.0.0";//Still need to address this properly

	FString CachedFeeQuote = "";
	
	//Vars//

	//Requires Credentials//
	
	FString BuildGetFeeOptionsIntent(const FCredentials_BE& Credentials, const TArray<TransactionUnion>& Txns) const;
	FString BuildSignMessageIntent(const FCredentials_BE& Credentials, const FString& Message) const;
	FString BuildSendTransactionIntent(const FCredentials_BE& Credentials, const TArray<TransactionUnion>& Txns) const;
	FString BuildSendTransactionWithFeeIntent(const FCredentials_BE& Credentials, const TArray<TransactionUnion>& Txns,const FString& FeeQuote) const;	
	FString BuildListSessionIntent(const FCredentials_BE& Credentials) const;
	FString BuildCloseSessionIntent(const FCredentials_BE& Credentials) const;
	FString BuildSessionValidationIntent(const FCredentials_BE& Credentials) const;

	//Requires Credentials//

	//Requires Bootstrap//
	
	FString BuildOpenSessionIntent(const FOpenSessionData& OpenSessionData) const;
	FString BuildInitiateAuthIntent(const FInitiateAuthData& InitiateAuthData) const;	
	FString GeneratePacketSignature(const FString& Packet) const;
	template<typename T> FString GenerateIntent(T Data) const;

	//Requires Bootstrap//

	//RPC Caller//

	template <typename T> void SequenceRPC(const FString& Url, const FString& Content, TSuccessCallback<T> OnSuccess, FFailureCallback OnFailure);
	
	//RPC Caller//

	//Support functions//

	static TArray<FFeeOption> JsonFeeOptionListToFeeOptionList(const TArray<TSharedPtr<FJsonValue>>& FeeOptionList);

	//Support functions//
	
public:
	static USequenceRPCManager * Make(UWallet * SessionWalletIn);
	
	//RPC Calls//

	/*
	 * Allows you to sign the given message with the SequenceWallet
	 * @OnSuccess The returned Struct from the signing process
	 * @OnFailure If an error occurs
	 */
	void SignMessage(const FCredentials_BE& Credentials, const FString& Message, const TSuccessCallback<FSignedMessage>& OnSuccess, const FFailureCallback& OnFailure);

	/*
	 * Allows you to send a transaction that will be automatically gassed IF the token is able to be (not all can be)
	 * @OnSuccess The Semi Struct Parsed JSON response
	 * @OnFailure An error occured during the transaction OR the token provided wasn't able to be automatically gassed
	 */
	void SendTransaction(const FCredentials_BE& Credentials, const TArray<TransactionUnion>& Transactions, const TSuccessCallback<FTransactionResponse>& OnSuccess, const FFailureCallback& OnFailure);

	/*
	 * Allows you to send a transaction with a given Fee, Use GetFeeOptions Or GetUnfilteredFeeOptions
	 * to pick a valid fee to send with this call
	 * @OnSuccess The Semi Struct Parsed JSON response
	 * @OnFailure An error occured during the transaction
	 */
	void SendTransactionWithFeeOption(const FCredentials_BE& Credentials, TArray<TransactionUnion> Transactions, FFeeOption FeeOption, const TSuccessCallback<FTransactionResponse>& OnSuccess, const FFailureCallback& OnFailure);

	/*
	 * Allows you to get FeeOptions for the transaction you pass in
	 * @OnSuccess A list of all VALID feeOptions for the presented transaction (can be empty if your wallet contains nothing that can cover any of the feeOptions)
	 * @OnFailure An error occured
	 */
	void GetFeeOptions(const FCredentials_BE& Credentials, const TArray<TransactionUnion>& Transactions, const TSuccessCallback<TArray<FFeeOption>>& OnSuccess, const FFailureCallback& OnFailure);
	
	/*
	 * Used to list all active sessions for the signed in credentials
	 * @OnSuccess A list of all active sessions
	 * @OnFailure An error occured
	 */
	void ListSessions(const FCredentials_BE& Credentials, const TSuccessCallback<TArray<FSession>>& OnSuccess, const FFailureCallback& OnFailure);

	/*
	 * Used to close the current Session with Sequence
	 * @OnSuccess The Session is closed
	 * @OnFailure An error occured
	 */
	void CloseSession(const FCredentials_BE& Credentials, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure);

	//Auth Calls//
	
	void InitEmailAuth(const FString& EmailIn, const TSuccessCallback<FCredentials_BE>& OnSuccess, const FFailureCallback& OnFailure);

	void InitGuestAuth(const TSuccessCallback<FCredentials_BE>& OnSuccess, const FFailureCallback& OnFailure);

	void OpenEmailSession(const FString& ChallengeIn, const FString& CodeIn, const FString& VerifierIn, const bool ForceCreateAccountIn, const TSuccessCallback<FCredentials_BE>& OnSuccess, const FFailureCallback& OnFailure);

	void OpenOIDCSession(const FString& IdTokenIn, const bool ForceCreateAccountIn, const TSuccessCallback<FCredentials_BE>& OnSuccess, const FFailureCallback& OnFailure);

	void OpenGuestSession(const FString& ChallengeIn, const FString& VerifierIn, const bool ForceCreateAccountIn, const TSuccessCallback<FCredentials_BE>& OnSuccess, const FFailureCallback& OnFailure);

	void OpenPlayFabSession(const FString& TitleIdIn, const FString& SessionTicketIn, const bool ForceCreateAccountIn, const TSuccessCallback<FCredentials_BE>& OnSuccess, const FFailureCallback& OnFailure);
	
	//Auth Calls//
	
	//RPC Calls//
};
