#pragma once

#include "CoreMinimal.h"
#include "Credentials.h"
#include "Sequence/FeeOption.h"
#include "Sequence/SequenceSendIntent.h"
#include "Sequence/SequenceResponseIntent.h"
#include "UObject/Object.h"
#include "Types/Types.h"
#include "Types/CryptoWallet.h"
#include "Util/Async.h"
#include "Sequence/SequenceFederationSupport.h"
#include "ResponseSignatureValidator.h"
#include "Util/CredentialsStorage.h"
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

	/**
	 * Updated ONLY on Auth Failure States
	 */
	FOpenSessionData Cached_OpenSessionData;

	UPROPERTY()
	UCryptoWallet * SessionWallet = nullptr;

	FString Cached_ProjectAccessKey = "";
	FString Cached_FeeQuote = "";
	FString Cached_Verifier = "";
	FString Cached_Challenge = "";
	FString Cached_Email = "";

	/**
	 * If this flag is enabled, we will preserve the SessionWallet for the next request
	 */
	bool PreserveSessionWallet = false;
	
	inline const static FString WaaSVersion = FString(TEXT("1.0.0 (Unreal 2.0.1)"));

	inline const static FString WaaSAuthenticatorIntentsUrlPath = TEXT("/rpc/WaasAuthenticator/SendIntent");
	inline const static FString WaaSAuthenticatorRegisterUrlPath = TEXT("/rpc/WaasAuthenticator/RegisterSession");

	inline const static FString WaaSSequenceApiUrlPath = TEXT("https://api.sequence.app/rpc/API/");

	//Vars//

	//Requires Credentials//
	

	FString BuildGetFeeOptionsIntent(const FCredentials_BE& Credentials, const TArray<TransactionUnion>& Transactions, TOptional<int64> CurrentTime) const;
	FString BuildSignMessageIntent(const FCredentials_BE& Credentials, const FString& Message, TOptional<int64> CurrentTime) const;
	FString BuildValidateMessageSignatureIntent(const int64& ChainId, const FString& WalletAddress, const FString& Message, const FString& Signature, TOptional<int64> CurrentTime) const;
	FString BuildSendTransactionIntent(const FCredentials_BE& Credentials, const TArray<TransactionUnion>& Transactions, TOptional<int64> CurrentTime) const;
	FString BuildSendTransactionWithFeeIntent(const FCredentials_BE& Credentials, const TArray<TransactionUnion>& Transactions, const FString& FeeQuote, TOptional<
	                                          int64> CurrentTime) const;
	FString BuildGetIdTokenIntent(const FCredentials_BE& Credentials, const FString& nonce, TOptional<int64> CurrentTime) const;
	FString BuildListSessionIntent(const FCredentials_BE& Credentials, TOptional<int64> CurrentTime) const;
	FString BuildListAccountsIntent(const FCredentials_BE& Credentials, TOptional<int64> CurrentTime) const;
	FString BuildGetSessionAuthProofIntent(const FCredentials_BE& Credentials, const FString& Nonce, TOptional<int64> CurrentTime) const;
	FString BuildCloseSessionIntent(TOptional<int64> CurrentTime) const;
	FString BuildSessionValidationIntent(TOptional<int64> CurrentTime) const;
	FString BuildFederateAccountIntent(const FFederateAccountData& FederateAccountIntent, TOptional<int64> CurrentTime) const;

	//Requires Credentials//

	//Requires Bootstrap//
	
	FString BuildOpenSessionIntent(const FOpenSessionData& OpenSessionData, TOptional<int64> CurrentTime) const;
	FString BuildInitiateAuthIntent(const FInitiateAuthData& InitiateAuthData, TOptional<int64> CurrentTime) const;	
	FString GeneratePacketSignature(const FString& Packet) const;
	template<typename T> FString GenerateIntent(T Data, TOptional<int64> CurrentTime) const;

	//Requires Bootstrap//

	//Url Builder//

	FString BuildRegisterUrl() const;
	FString BuildAuthenticatorIntentsUrl() const;
	FString BuildAPIUrl(const FString& Endpoint) const;

	
	//Url Builder//

	

	//Session Wallet Management Code//

	void CheckAndUpdateSessionFromPreserveSessionWallet();
	
	//Session Wallet Management Code//

	//RPC Caller//
	void SequenceRPC(const FString& Url, const FString& Content, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure) const;
	void SequenceRPC(const ::FString& Url, const ::FString& Content, const TSuccessCallback<FHttpResponsePtr>& OnSuccess, const FFailureCallback& OnFailure) const;
	void SendIntent(const FString& Url, TFunction<FString (TOptional<int64>)> ContentGenerator, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure) const;
	
	FTimespan TimeShift;
	
	void InitializeTimeShift();
	static FTimespan GetTimeShiftFromResponse(const FString& DateHeader);

public:

	USequenceRPCManager();

	UPROPERTY()
	UResponseSignatureValidator* Validator;

	/**
	 * Allows you to create a new Manager with a session wallet that's either random or set by on disk credentials
	 * @param UseStoredSessionId If True set sessionWallet to on disk credentials, If False use random session wallet
	 * @return Returns the USequenceRPCManager with the SessionWallet initialized accordingly
	 */
	static USequenceRPCManager * Make(const bool UseStoredSessionId);

	/**
	 * Creates a USequenceRPCManager and sets the SessionWallet to the given one
	 * @param SessionWalletIn The SessionWallet to Initialize with
	 * @return The Initialized USequenceRPCManager
	 */
	static USequenceRPCManager * Make(UCryptoWallet * SessionWalletIn);
	
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
	 * Allows you to send a signature for validation using the sequence rpc api
	 * @param Signature The signature you wish to validate
	 * @param Message The message that has been signed
	 * @param OnSuccess The returned Struct from the signing process
	 * @param OnFailure If an error occurs
	 */


	void ValidateMessageSignature(const int64& ChainId, const FString& WalletAddress, const FString& Signature, const FString& Message, const TSuccessCallback<FSeqValidateMessageSignatureResponse_Data>& OnSuccess, const FFailureCallback& OnFailure) const;


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

	void GetIdToken(const FCredentials_BE& Credentials, const FString& Nonce, const TSuccessCallback<FSeqIdTokenResponse_Data>&OnSuccess, const FFailureCallback& OnFailure) const;


	void ListSessions(const FCredentials_BE& Credentials, const TSuccessCallback<TArray<FSeqListSessions_Session>>& OnSuccess, const FFailureCallback& OnFailure) const;

	void ListAccounts(const FCredentials_BE& Credentials, const TSuccessCallback<FSeqListAccountsResponse_Data>& OnSuccess, const FFailureCallback& OnFailure) const;

	void GetSessionAuthProof(const FCredentials_BE& Credentials, const FString& Nonce,  const TSuccessCallback<FSeqGetSessionAuthProof_Data>& OnSuccess, const FFailureCallback& OnFailure) const;
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
	 * Used to Initiate Email Base Federation
	 * @param EmailIn The Email we are Federating with
	 * @param OnSuccess If InitiateAuth Succeeds, this will fire and indicate we need a Code to finish Email Login
	 * @param OnFailure If something went wrong
	 */
	void InitEmailFederation(const FString& EmailIn, const TFunction<void()>& OnSuccess, const FFailureCallback& OnFailure);
	
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
	void OpenEmailSession(const FString& CodeIn, const bool ForceCreateAccountIn, const TSuccessCallback<FCredentials_BE>& OnSuccess, const FFailureCallback& OnFailure, const TFunction<void(FFederationSupportData)>& OnFederationRequired);

	/**
	 * Used to Login with OIDC (Social Signin)
	 * @param IdTokenIn IdToken Collected from UI
	 * @param ForceCreateAccountIn Used to Force Account Creation
	 * @param OnSuccess Fires with Credentials on a Successful Login
	 * @param OnFailure Fires if there's an Authentication Issue
	 */
	void OpenOIDCSession(const FString& IdTokenIn, const bool ForceCreateAccountIn, const TSuccessCallback<FCredentials_BE>& OnSuccess, const FFailureCallback& OnFailure, const TFunction<void(FFederationSupportData)>& OnFederationRequired);

	/**
	 * Used to Authenticate a User via PlayFab
	 * @param SessionTicketIn SessionTicket Acquired from PlayFab
	 * @param ForceCreateAccountIn Used to Force Account Creation
	 * @param OnSuccess Fires with Credentials on a Successful Login
	 * @param OnFailure Fires if there's an Authentication Issue
	 */
	void OpenPlayFabSession(const FString& SessionTicketIn, const bool ForceCreateAccountIn, const TSuccessCallback<FCredentials_BE>& OnSuccess, const FFailureCallback& OnFailure, const TFunction<void(FFederationSupportData)>& OnFederationRequired);

	/**
	 * Uses cached OpenSession Intent from last failed login attempt to force open a session
	 * @param OnSuccess Fires If the session was successfully forced open
	 * @param OnFailure Fires if there's an Authentication Issue
	 */
	void ForceOpenSessionInUse(const TSuccessCallback<FCredentials_BE>& OnSuccess, const FFailureCallback& OnFailure);

	/**
	 * GetLinkedWallets
	 * @param OnSuccess 
	 * @param OnFailure 
	 */
	void GetLinkedWallets(const FSeqLinkedWalletRequest& Request, const TSuccessCallback<FSeqLinkedWalletsResponse>& OnSuccess, const FFailureCallback& OnFailure) const;

	/**
	 * GetLinkedWallets
	 * @param OnSuccess 
	 * @param OnFailure 
	 */
	void RemoveLinkedWallet(const FSeqLinkedWalletRequest& Request, const TFunction<void()>& OnSuccess, const FFailureCallback& OnFailure) const;
	
	//Auth Calls//

	//Federation Calls//

	void FederateEmailSession(const FString& WalletIn, const FString& CodeIn, const TFunction<void()>& OnSuccess, const FFailureCallback& OnFailure);

	void FederateOIDCSession(const FString& WalletIn, const FString& IdTokenIn, const TFunction<void()>& OnSuccess, const FFailureCallback& OnFailure);

	void FederatePlayFabSession(const FString& WalletIn, const FString& SessionTicketIn, const TFunction<void()>& OnSuccess, const FFailureCallback& OnFailure);

	void FederateSessionInUse(const FString& WalletIn, const TFunction<void()>& OnSuccess, const FFailureCallback& OnFailure) const;
	
	//Federation Calls//

	/**
	 * Updates the SessionWallet with a random one
	 */
	void UpdateWithRandomSessionWallet();

	/**
	 * Updates the SessionWallet with one stored on disk if possible,
	 * does nothing if credentials on disk are invalid
	 */
	void UpdateWithStoredSessionWallet();
	
	//RPC Calls//

private:
	UPROPERTY()
	UCredentialsStorage* CredentialsStorage;
};
