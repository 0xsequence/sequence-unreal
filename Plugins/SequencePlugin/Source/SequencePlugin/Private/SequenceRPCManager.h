// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Credentials.h"
#include "Sequence/FeeOption.h"
#include "Sequence/SequenceSendIntent.h"
#include "Sequence/SequenceResponseIntent.h"
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

	UPROPERTY()
	UWallet * SessionWallet = nullptr;

	FString Cached_ProjectAccessKey = "";
	FString Cached_FeeQuote = "";
	FString Cached_Verifier = "";
	FString Cached_Challenge = "";
	FString Cached_Email = "";

	inline const static FString WaaSVersion = "1.0.0";//Still need to address this properly
	inline const static FString UrlPath = TEXT("/rpc/WaasAuthenticator/SendIntent");
	inline const static FString UrlRegisterPath = TEXT("/rpc/WaasAuthenticator/RegisterSession");
	
	//Vars//

	//Requires Credentials//
	
	FString BuildGetFeeOptionsIntent(const FCredentials_BE& Credentials, const TArray<TransactionUnion>& Transactions) const;
	FString BuildSignMessageIntent(const FCredentials_BE& Credentials, const FString& Message) const;
	FString BuildSendTransactionIntent(const FCredentials_BE& Credentials, const TArray<TransactionUnion>& Transactions) const;
	FString BuildSendTransactionWithFeeIntent(const FCredentials_BE& Credentials, const TArray<TransactionUnion>& Transactions,const FString& FeeQuote) const;	
	FString BuildListSessionIntent(const FCredentials_BE& Credentials) const;
	FString BuildCloseSessionIntent() const;
	FString BuildSessionValidationIntent() const;
	FString BuildFederateAccountIntent(const FFederateAccountData& FederateAccountIntent) const;

	//Requires Credentials//

	//Requires Bootstrap//
	
	FString BuildOpenSessionIntent(const FOpenSessionData& OpenSessionData) const;
	FString BuildInitiateAuthIntent(const FInitiateAuthData& InitiateAuthData) const;	
	FString GeneratePacketSignature(const FString& Packet) const;
	template<typename T> FString GenerateIntent(T Data) const;

	//Requires Bootstrap//

	//Url Builder//

	FString BuildUrl() const;

	FString BuildRegisterUrl() const;
	
	//Url Builder//
	
	//RPC Caller//

	void SequenceRPC(const FString& Url, const FString& Content, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure) const;
	
	//RPC Caller//
	
public:
	static USequenceRPCManager * Make(UWallet * SessionWalletIn);
	
	//RPC Calls//

	/**
	 * Used to Sign a given Message with the Sequence Api
	 * @param Credentials Credentials used to build Intent
	 * @param Message The message you're signing
	 * @param OnSuccess Called if the operation succeeds with your signed message
	 * @param OnFailure Called if the operation fails with an Error
	 */
	void SignMessage(const FCredentials_BE& Credentials, const FString& Message, const TSuccessCallback<FSeqSignMessageResponse_Response>& OnSuccess, const FFailureCallback& OnFailure) const;

	/**
	 * Used to send transactions via the sequence rpc api
	 * @param Credentials Credentials Credentials used to build Intent
	 * @param Transactions List of Transaction Items you wish to send
	 * @param OnSuccess Called if the operation succeeds with Transaction Details
	 * @param OnFailure Called if the operation fails with an Error
	 */
	void SendTransaction(const FCredentials_BE& Credentials, const TArray<TransactionUnion>& Transactions, const TSuccessCallback<FSeqTransactionResponse_Data>& OnSuccess, const FFailureCallback& OnFailure) const;

	/**
	 * Used to send transactions via the sequence rpc api with a set fee
	 * @param Credentials Credentials Credentials used to build Intent
	 * @param Transactions List of Transaction Items you wish to send
	 * @param FeeOption The Fee option you are using for this transaction
	 * @param OnSuccess Called if the operation succeeds with Transaction Details
	 * @param OnFailure Called if the operation fails with an Error
	 */
	void SendTransactionWithFeeOption(const FCredentials_BE& Credentials, TArray<TransactionUnion> Transactions, FFeeOption FeeOption, const TSuccessCallback<FSeqTransactionResponse_Data>& OnSuccess, const FFailureCallback& OnFailure) const;

	/**
	 * Allows you to get FeeOptions for the transactions you pass in
	 * @param Credentials Credentials Credentials used to build Intent
	 * @param Transactions List of Transaction Items you wish to send
	 * @param OnSuccess Called if the operation succeeds with FeeOptions
	 * @param OnFailure Called if the operation fails with an Error
	 */
	void GetFeeOptions(const FCredentials_BE& Credentials, const TArray<TransactionUnion>& Transactions, const TSuccessCallback<TArray<FFeeOption>>& OnSuccess, const FFailureCallback& OnFailure);

	/**
	 * Used to fetch a list of active sessions
	 * @param Credentials Credentials used to build Intent
	 * @param OnSuccess Called if the operation succeeds with your Sessions
	 * @param OnFailure Called if the operation fails with an Error
	 */
	void ListSessions(const FCredentials_BE& Credentials, const TSuccessCallback<TArray<FSeqListSessions_Session>>& OnSuccess, const FFailureCallback& OnFailure) const;

	/**
	 * Used to close the Current Session
	 * @param Credentials Credentials used to build Intent
	 * @param OnSuccess Called if the operation succeeds
	 * @param OnFailure Called if the operation fails with an Error
	 */
	void CloseSession(const FCredentials_BE& Credentials, const TFunction<void()>& OnSuccess, const FFailureCallback& OnFailure) const;
	
	//Auth Calls//

	/**
	 * Used to Initiate Email based Login
	 * @param EmailIn Email to Login with
	 * @param OnSuccess If InitiateAuth Succeeds, this will fire and indicate we need a Code to finish Email Login
	 * @param OnFailure If something went wrong
	 */
	void InitEmailAuth(const FString& EmailIn, const TFunction<void()>& OnSuccess, const FFailureCallback& OnFailure);

	/**
	 * Used to open a guest session with the SequenceApi
	 * @param ForceCreateAccountIn Used to Force Account Creation
	 * @param OnSuccess Fires with Credentials on a Successful Login
	 * @param OnFailure Fires if there's an Authentication Issue
	 */
	void OpenGuestSession(const bool ForceCreateAccountIn, const TSuccessCallback<FCredentials_BE>& OnSuccess, const FFailureCallback& OnFailure);

	/**
	 * Used to Login with Email
	 * @param CodeIn The Email Auth Code collected from the UI
	 * @param ForceCreateAccountIn Used to Force Account Creation
	 * @param OnSuccess Fires with Credentials on a Successful Login
	 * @param OnFailure Fires if there's an Authentication Issue
	 */
	void OpenEmailSession(const FString& CodeIn, const bool ForceCreateAccountIn, const TSuccessCallback<FCredentials_BE>& OnSuccess, const FFailureCallback& OnFailure) const;

	/**
	 * Used to Login with OIDC (Social Signin)
	 * @param IdTokenIn IdToken Collected from UI
	 * @param ForceCreateAccountIn Used to Force Account Creation
	 * @param OnSuccess Fires with Credentials on a Successful Login
	 * @param OnFailure Fires if there's an Authentication Issue
	 */
	void OpenOIDCSession(const FString& IdTokenIn, const bool ForceCreateAccountIn, const TSuccessCallback<FCredentials_BE>& OnSuccess, const FFailureCallback& OnFailure) const;

	/**
	 * Used to Authenticate a User via PlayFab
	 * @param SessionTicketIn SessionTicket Acquired from PlayFab
	 * @param ForceCreateAccountIn Used to Force Account Creation
	 * @param OnSuccess Fires with Credentials on a Successful Login
	 * @param OnFailure Fires if there's an Authentication Issue
	 */
	void OpenPlayFabSession(const FString& SessionTicketIn, const bool ForceCreateAccountIn, const TSuccessCallback<FCredentials_BE>& OnSuccess, const FFailureCallback& OnFailure) const;
	
	//Auth Calls//

	//Federation Calls//

	void FederateEmailSession(const FString& CodeIn, const TFunction<void()>& OnSuccess, const FFailureCallback& OnFailure) const;

	void FederateOIDCSession(const FString& IdTokenIn, const TFunction<void()>& OnSuccess, const FFailureCallback& OnFailure) const;

	void FederatePlayFabSession(const FString& WalletIn, const FString& SessionTicketIn, const TFunction<void()>& OnSuccess, const FFailureCallback& OnFailure) const;
	
	//Federation Calls//
	
	//RPC Calls//
};
