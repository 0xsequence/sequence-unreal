#include "SequenceRPCManager.h"
#include "RequestHandler.h"
#include "ConfigFetcher.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Types/BinaryData.h"
#include "Misc/Base64.h"
#include "Interfaces/IPluginManager.h"
#include "Sequence/SequenceWallet.h"
#include "Sequence/SequenceAuthResponseIntent.h"
#include "Misc/DateTime.h"
#include "Util/Log.h"
#include "Sequence/SequenceSdk.h"
#include "Util/CredentialsStorage.h"

template<typename T> FString USequenceRPCManager::GenerateIntent(T Data, TOptional<int64> CurrentTime) const
{
	const int64 Issued = CurrentTime.IsSet() ? CurrentTime.GetValue() : (FDateTime::UtcNow() + TimeShift).ToUnixTimestamp() - 30;
	const int64 Expires = Issued + 86400;
	FGenericData * LocalDataPtr = &Data;
	const FString Operation = LocalDataPtr->Operation;
	const FSignatureIntent SigIntent(LocalDataPtr,Expires,Issued,Operation,this->WaaSVersion);
	const FString Signature = this->GeneratePacketSignature(SigIntent.GetJson<T>());
	const FSignatureEntry SigEntry(this->SessionWallet->GetSessionId(),Signature);
	const FSignedIntent SignedIntent(LocalDataPtr,Expires,Issued,Operation,{SigEntry},this->WaaSVersion);
	
	if (Operation.Equals(OpenSessionOP,ESearchCase::IgnoreCase))
	{
		const FRegisterFinalIntent FinalIntent(SignedIntent,FGuid::NewGuid().ToString());
		return FinalIntent.GetJson<T>();
	}
	else
	{
		const FGenericFinalIntent FinalIntent(SignedIntent);
		return FinalIntent.GetJson<T>();
	}
}

USequenceRPCManager::USequenceRPCManager()
{
	this->Validator = NewObject<UResponseSignatureValidator>();
	this->CredentialsStorage = NewObject<UCredentialsStorage>();
}

void USequenceRPCManager::SequenceRPC(const FString& Url, const FString& Content, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure) const
{
	UResponseSignatureValidator& RPCValidator = *Validator;
	SEQ_LOG_EDITOR(Display, TEXT("%s - %s"), *Url, *Content);
	
	NewObject<URequestHandler>()
		->PrepareRequest()
		->WithUrl(Url)
		->WithHeader("Content-type", "application/json")
		->WithHeader("Accept", "application/json")
		->WithHeader("X-Access-Key", this->Cached_ProjectAccessKey)
		->WithHeader("Accept-Signature", "sig=()")
		->WithVerb("POST")
		->WithContentAsString(Content)
		->ProcessAndThen(RPCValidator,OnSuccess, OnFailure);
}

void USequenceRPCManager::SequenceRPC(const FString& Url, const FString& Content, const TSuccessCallback<FHttpResponsePtr>& OnSuccess, const FFailureCallback& OnFailure) const
{
	SEQ_LOG_EDITOR(Display, TEXT("%s - %s"), *Url, *Content);
	
	NewObject<URequestHandler>()
	->PrepareRequest()
	->WithUrl(Url)
	->WithHeader("Content-type", "application/json")
	->WithHeader("Accept", "application/json")
	->WithHeader("X-Access-Key", this->Cached_ProjectAccessKey)
	->WithVerb("POST")
	->WithContentAsString(Content)
	->ProcessAndThen(OnSuccess, OnFailure);
}

void USequenceRPCManager::SendIntent(const FString& Url, TFunction<FString(TOptional<int64>)> ContentGenerator,
	const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure) const
{
	this->SequenceRPC(Url, ContentGenerator(TOptional<int64>()), [this, Url, ContentGenerator, OnSuccess, OnFailure](FHttpResponsePtr Response)
	{
		const FString Content = UTF8ToString(FUnsizedData(Response.Get()->GetContent()));
		const int32 Code = Response.Get()->GetResponseCode();
		
		SEQ_LOG_EDITOR(Display, TEXT("%d %s"), Code, *Content);

		OnSuccess(Content);
	}, [this, Url, ContentGenerator, OnSuccess, OnFailure](const FSequenceError& Error)
	{
		const FString Content = Error.Response->GetContentAsString();
		const int32 Code = Error.Response->GetResponseCode();
		SEQ_LOG_EDITOR(Error, TEXT("%d %s"), Code, *Content);
		
		if (!Content.Contains("intent is invalid: intent expired") &&
			!Content.Contains("intent is invalid: intent issued in the future"))
		{
			OnFailure(Error);
			return;
		}
		
		FDateTime Time;
		const FString Date = Error.Response->GetHeader("Date");
		if(!FDateTime::ParseHttpDate(Date, Time))
		{
			OnFailure(FSequenceError(FailedToParseIntentTime, Error.Response, "Failed to parse intent time " + Date));
			return;
		}
					
		UE_LOG(LogTemp, Display, TEXT("Resending intent with date %i"), Time.ToUnixTimestamp());
		this->SequenceRPC(Url, ContentGenerator(TOptional(Time.ToUnixTimestamp())), OnSuccess, OnFailure);
	});
}

FString USequenceRPCManager::BuildGetFeeOptionsIntent(const FCredentials_BE& Credentials, const TArray<TransactionUnion>& Transactions, TOptional<int64> CurrentTime) const
{
	const FGetFeeOptionsData GetFeeOptionsData(SequenceSdk::GetChainIdString(),Transactions,Credentials.GetWalletAddress());
	const FString Intent = this->GenerateIntent<FGetFeeOptionsData>(GetFeeOptionsData, CurrentTime);
	return Intent;
}

FString USequenceRPCManager::BuildSignMessageIntent(const FCredentials_BE& Credentials, const FString& Message, TOptional<int64> CurrentTime) const
{
	const int64 issued = FDateTime::UtcNow().ToUnixTimestamp() - 30;
	const int64 expires = issued + 86400;
	const FString issuedString = FString::Printf(TEXT("%lld"),issued);
	const FString expiresString = FString::Printf(TEXT("%lld"),expires);
	const FString Wallet = Credentials.GetWalletAddress();
	
	//eip-191 and keccak hashing the message
	const FString LeadingByte = "\x19";//leading byte
	FString Payload = LeadingByte + "Ethereum Signed Message:\n";
	Payload.AppendInt(Message.Len());
	Payload += Message;
	const FUnsizedData PayloadBytes = StringToUTF8(Payload);
	const FString EIP_Message = "0x" + BytesToHex(PayloadBytes.Ptr(),PayloadBytes.GetLength());
	
	const FSignMessageData SignMessageData(EIP_Message,SequenceSdk::GetChainIdString(),Credentials.GetWalletAddress());
	const FString Intent = this->GenerateIntent<FSignMessageData>(SignMessageData, CurrentTime);

	return Intent;
}


FString USequenceRPCManager::BuildValidateMessageSignatureIntent(const int64& ChainId, const FString& WalletAddress, const FString& Message, const FString& Signature, TOptional<int64> CurrentTime) const
{
	const FValidateMessageSignatureData ValidateMessageSignatureData(FString::FromInt(ChainId), WalletAddress, Message, Signature);
	return ValidateMessageSignatureData.GetJson();
}


FString USequenceRPCManager::BuildSendTransactionIntent(const FCredentials_BE& Credentials, const TArray<TransactionUnion>& Transactions, TOptional<int64> CurrentTime) const
{
	const FString Identifier = "unreal-sdk-" + FDateTime::UtcNow().ToString() + "-" + Credentials.GetWalletAddress();
	const FSendTransactionData SendTransactionData(Identifier,SequenceSdk::GetChainIdString(),Transactions,Credentials.GetWalletAddress());
	const FString Intent = this->GenerateIntent<FSendTransactionData>(SendTransactionData, CurrentTime);
	return Intent;
}

FString USequenceRPCManager::BuildSendTransactionWithFeeIntent(const FCredentials_BE& Credentials, const TArray<TransactionUnion>& Transactions, const FString& FeeQuote, TOptional<int64> CurrentTime) const
{
	const FString Identifier = "unreal-sdk-" + FDateTime::UtcNow().ToString() + "-" + Credentials.GetWalletAddress();
	const FSendTransactionWithFeeOptionData SendTransactionWithFeeOptionData(Identifier,SequenceSdk::GetChainIdString(),Transactions,FeeQuote,Credentials.GetWalletAddress());
	const FString Intent = this->GenerateIntent<FSendTransactionWithFeeOptionData>(SendTransactionWithFeeOptionData, CurrentTime);
	return Intent;
}

FString USequenceRPCManager::BuildGetIdTokenIntent(const FCredentials_BE& Credentials, const FString& Nonce, TOptional<int64> CurrentTime) const
{
	const FGetIdTokenData GetIdTokenData(Credentials.GetSessionWallet()->GetSessionId(), Credentials.GetWalletAddress(), Nonce);
	const FString Intent = this->GenerateIntent<FGetIdTokenData>(GetIdTokenData, CurrentTime);
	return Intent;
}


FString USequenceRPCManager::BuildListSessionIntent(const FCredentials_BE& Credentials, TOptional<int64> CurrentTime) const
{
	const FListSessionsData ListSessionsData(Credentials.GetWalletAddress());
	const FString Intent = this->GenerateIntent<FListSessionsData>(ListSessionsData, CurrentTime);
	return Intent;
}


FString USequenceRPCManager::BuildListAccountsIntent(const FCredentials_BE& Credentials, TOptional<int64> CurrentTime) const
{
	const FListAccountsData ListAccountsData(Credentials.GetWalletAddress());
	const FString Intent = this->GenerateIntent<FListAccountsData>(ListAccountsData, CurrentTime);
	return Intent;
}


FString USequenceRPCManager::BuildGetSessionAuthProofIntent(const FCredentials_BE& Credentials, const FString& Nonce, TOptional<int64> CurrentTime) const
{
	const FGetSessionAuthProofData GetSessionAuthProofData(SequenceSdk::GetChainIdString(), Credentials.GetWalletAddress(), Nonce);
	const FString Intent = this->GenerateIntent<FGetSessionAuthProofData>(GetSessionAuthProofData, CurrentTime);
	return Intent;
}

FString USequenceRPCManager::BuildCloseSessionIntent(TOptional<int64> CurrentTime) const
{
	const FCloseSessionData CloseSessionData(this->SessionWallet->GetSessionId());
	const FString Intent = this->GenerateIntent(CloseSessionData, CurrentTime);
	return Intent;
}

FString USequenceRPCManager::BuildSessionValidationIntent(TOptional<int64> CurrentTime) const
{
	const FString Intent = "{\\\"sessionId\\\":\\\"" + this->SessionWallet->GetSessionId() + "\\\", \"\"}";
	return Intent;
}

FString USequenceRPCManager::BuildFederateAccountIntent(const FFederateAccountData& FederateAccountIntent, TOptional<int64> CurrentTime) const
{
	return this->GenerateIntent<FFederateAccountData>(FederateAccountIntent, CurrentTime);
}

FString USequenceRPCManager::BuildOpenSessionIntent(const FOpenSessionData& OpenSessionData, TOptional<int64> CurrentTime) const
{
	return this->GenerateIntent<FOpenSessionData>(OpenSessionData, CurrentTime);
}

FString USequenceRPCManager::BuildInitiateAuthIntent(const FInitiateAuthData& InitiateAuthData, TOptional<int64> CurrentTime) const
{
	return this->GenerateIntent<FInitiateAuthData>(InitiateAuthData, CurrentTime);
}

FString USequenceRPCManager::GeneratePacketSignature(const FString& Packet) const
{
	//keccakhash of the packet first
	const FHash256 SigningHash = FHash256::New();
	const FUnsizedData EncodedSigningData = StringToUTF8(Packet);
	Keccak256::getHash(EncodedSigningData.Arr.Get()->GetData(), EncodedSigningData.GetLength(), SigningHash.Ptr());
	TArray<uint8> SigningBytes;
	SigningBytes.Append(SigningHash.Ptr(),SigningHash.GetLength());
	const FString Signature = this->SessionWallet->SignMessageWithPrefix(SigningBytes, 32);
	return Signature;
}

FString USequenceRPCManager::BuildAuthenticatorIntentsUrl() const
{
	return this->WaaSSettings.GetRPCServer() + this->WaaSAuthenticatorIntentsUrlPath;
}

FString USequenceRPCManager::BuildRegisterUrl() const
{
	return this->WaaSSettings.GetRPCServer() + this->WaaSAuthenticatorRegisterUrlPath;
}

FString USequenceRPCManager::BuildAPIUrl(const FString& Endpoint) const
{
	return this->WaaSSequenceApiUrlPath + Endpoint;
}

void USequenceRPCManager::CheckAndUpdateSessionFromPreserveSessionWallet()
{
	//We need to know if we are using a random session wallet or the current one!
	if (this->PreserveSessionWallet)
	{
		this->PreserveSessionWallet = false;
	}
	else
	{
		this->UpdateWithRandomSessionWallet();
	}
}

void USequenceRPCManager::UpdateWithRandomSessionWallet()
{	
	this->SessionWallet = UCryptoWallet::Make();
}

void USequenceRPCManager::UpdateWithStoredSessionWallet()
{
	if (FStoredCredentials_BE StoredCredentials = this->CredentialsStorage->GetStoredCredentials(); StoredCredentials.GetValid())
	{
		this->SessionWallet = StoredCredentials.GetCredentials().GetSessionWallet();
	}
	else
	{
		SEQ_LOG(Error, TEXT("Stored Credentials are Invalid, Please Login"));
	}
}

USequenceRPCManager* USequenceRPCManager::Make(const bool UseStoredSessionId)
{
	USequenceRPCManager* Manager = nullptr;
	
	if (UseStoredSessionId)
	{
		const UCredentialsStorage* CredentialsStorage = NewObject<UCredentialsStorage>();
		if (FStoredCredentials_BE StoredCredentials = CredentialsStorage->GetStoredCredentials(); StoredCredentials.GetValid())
		{
			Manager = Make(StoredCredentials.GetCredentials().GetSessionWallet());
		}
	}
	
	if (!Manager)
	{
		Manager = Make(UCryptoWallet::Make());
	}
	
	return Manager;
}

USequenceRPCManager* USequenceRPCManager::Make(UCryptoWallet* SessionWalletIn)
{
	USequenceRPCManager* SequenceRPCManager = NewObject<USequenceRPCManager>();
	SequenceRPCManager->SessionWallet = SessionWalletIn;
	FString ParsedJwt;
	FBase64::Decode(UConfigFetcher::GetConfigVar(UConfigFetcher::WaaSConfigKey), ParsedJwt);
	SequenceRPCManager->WaaSSettings = USequenceSupport::JSONStringToStruct<FWaasJWT>(ParsedJwt);
	SequenceRPCManager->Cached_ProjectAccessKey = UConfigFetcher::GetConfigVar(UConfigFetcher::ProjectAccessKey);
	
	SequenceRPCManager->InitializeTimeShift();
	return SequenceRPCManager;
}

void USequenceRPCManager::SignMessage(const FCredentials_BE& Credentials, const FString& Message, const TSuccessCallback<FSeqSignMessageResponse_Response>& OnSuccess, const FFailureCallback& OnFailure) const
{
	const TSuccessCallback<FString> OnResponse = [OnSuccess,OnFailure](const FString& Response)
	{
		const FSeqSignMessageResponse ParsedResponse = USequenceSupport::JSONStringToStruct<FSeqSignMessageResponse>(Response);

		if (ParsedResponse.IsValid())
		{
			OnSuccess(ParsedResponse.Response);
		}
		else
		{
			OnFailure(FSequenceError(RequestFail, "Error Parsing Response: " + Response));
		}
	};

	if (Credentials.RegisteredValid())
	{
		this->SendIntent(this->BuildAuthenticatorIntentsUrl(),[this, Credentials, Message](const TOptional<int64>& CurrentTime)
		{
			return this->BuildSignMessageIntent(Credentials, Message, CurrentTime);
		},OnResponse,OnFailure);
	}
	else
	{
		OnFailure(FSequenceError(RequestFail, "[Session Not Registered Please Register Session First]"));
	}
}

void USequenceRPCManager::ValidateMessageSignature(const int64& ChainId, const FString& WalletAddress, const FString& Message, const FString& Signature, const TSuccessCallback<FSeqValidateMessageSignatureResponse_Data>& OnSuccess, const FFailureCallback& OnFailure) const
{
	const TSuccessCallback<FString> OnResponse = [OnSuccess, OnFailure](const FString& Response)
	{
		SEQ_LOG(Log, TEXT("Response: %s"), *Response);

		const FSeqValidateMessageSignatureResponse ParsedResponse = USequenceSupport::JSONStringToStruct<FSeqValidateMessageSignatureResponse>(Response);

		if (ParsedResponse.IsValid())
		{
			OnSuccess(ParsedResponse.Data);
		}
		else
		{
			OnFailure(FSequenceError(RequestFail, "Error Parsing Response: " + Response));
		}
	};

	this->SendIntent(this->BuildAPIUrl("IsValidMessageSignature"),[this, ChainId, WalletAddress, Message, Signature](const TOptional<int64>& CurrentTime)
	{
		return BuildValidateMessageSignatureIntent(ChainId, WalletAddress, Message, Signature, CurrentTime);
	}, OnResponse, OnFailure);

}

void USequenceRPCManager::SendTransaction(const FCredentials_BE& Credentials, const TArray<TransactionUnion>& Transactions, const TSuccessCallback<FSeqTransactionResponse_Data>& OnSuccess, const FFailureCallback& OnFailure) const
{
	const TSuccessCallback<FString> OnResponse = [OnSuccess, OnFailure](const FString& Response)
	{
		const FSeqTransactionResponse ParsedResponse = USequenceSupport::JSONStringToStruct<FSeqTransactionResponse>(Response);

		if (ParsedResponse.IsValid())
		{
			OnSuccess(ParsedResponse.Response.Data);
		}
		else
		{
			OnFailure(FSequenceError(RequestFail, "Error Parsing Response: " + Response));
		}
	};
	
	if (Credentials.RegisteredValid())
	{
		this->SendIntent(this->BuildAuthenticatorIntentsUrl(),[this, Credentials, Transactions](TOptional<int64> CurrentTime)
		{
			return BuildSendTransactionIntent(Credentials, Transactions, CurrentTime);
		},OnResponse,OnFailure);
	}
	else
	{
		OnFailure(FSequenceError(RequestFail, "[Session Not Registered Please Register Session First]"));
	}
}

void USequenceRPCManager::SendTransactionWithFeeOption(const FCredentials_BE& Credentials, TArray<TransactionUnion> Transactions, FFeeOption FeeOption, const TSuccessCallback<FSeqTransactionResponse_Data>& OnSuccess, const FFailureCallback& OnFailure) const
{
	Transactions.Insert(FeeOption.CreateTransaction(),0);
	const TSuccessCallback<FString> OnResponse = [OnSuccess, OnFailure](const FString& Response)
	{	
		const FSeqTransactionResponse ParsedResponse = USequenceSupport::JSONStringToStruct<FSeqTransactionResponse>(Response);

		if (ParsedResponse.IsValid())
		{
			OnSuccess(ParsedResponse.Response.Data);
		}
		else
		{
			OnFailure(FSequenceError(RequestFail, "Error Parsing Response: " + Response));
		}
	};
	
	if (Credentials.RegisteredValid())
	{
		this->SendIntent(this->BuildAuthenticatorIntentsUrl(),[this, Credentials, Transactions](TOptional<int64> CurrentTime)
		{
			return BuildSendTransactionWithFeeIntent(Credentials, Transactions,this->Cached_FeeQuote, CurrentTime);
		}, OnResponse, OnFailure);
	}
	else
	{
		OnFailure(FSequenceError(RequestFail, "[Session Not Registered Please Register Session First]"));
	}
}

void USequenceRPCManager::GetFeeOptions(const FCredentials_BE& Credentials, const TArray<TransactionUnion>& Transactions, const TSuccessCallback<TArray<FFeeOption>>& OnSuccess, const FFailureCallback& OnFailure)
{
	
	const TSuccessCallback<FString> OnResponse = [this, OnSuccess, OnFailure](const FString& Response)
	{
		const FSeqGetFeeOptionsResponse ParsedResponse = USequenceSupport::JSONStringToStruct<FSeqGetFeeOptionsResponse>(Response);

		if (ParsedResponse.IsValid())
		{
			this->Cached_FeeQuote = ParsedResponse.Response.FeeQuote;

			if (ParsedResponse.Response.Data.FeeOptions.Num()>0)
			{
				OnSuccess(ParsedResponse.Response.Data.FeeOptions);
			}
			else
			{
				OnFailure(FSequenceError(RequestFail, "No fee options received, contract gas might be sponsored, check builder configs or use a non-fee options transaction. " + Response));

			}
			
		}
		else
		{
			OnFailure(FSequenceError(RequestFail, "Error Parsing Response: " + Response));
		}
	};
	
	if (Credentials.RegisteredValid())
	{
		this->SendIntent(this->BuildAuthenticatorIntentsUrl(),[this, Credentials, Transactions](TOptional<int64> CurrentTime)
		{
			return BuildGetFeeOptionsIntent(Credentials, Transactions, CurrentTime);
		},OnResponse,OnFailure);
	}
	else
	{
		OnFailure(FSequenceError(RequestFail, "[Session Not Registered Please Register Session First]"));
	}
}

void USequenceRPCManager::GetIdToken(const FCredentials_BE& Credentials, const FString& Nonce, const TSuccessCallback<FSeqIdTokenResponse_Data>& OnSuccess, const FFailureCallback& OnFailure) const
{
	const TSuccessCallback<FString> OnResponse = [OnSuccess, OnFailure](const FString& Response)
		{
			const FSeqIdTokenResponse ParsedResponse = USequenceSupport::JSONStringToStruct<FSeqIdTokenResponse>(Response);

			if (ParsedResponse.IsValid())
			{
				OnSuccess(ParsedResponse.Response.Data);
			}
			else
			{
				OnFailure(FSequenceError(RequestFail, "Error Parsing Response: " + Response));
			}
		};

	if (Credentials.RegisteredValid())
	{
		this->SendIntent(this->BuildAuthenticatorIntentsUrl(), [this, Credentials, Nonce](TOptional<int64> CurrentTime)
		{
			return this->BuildGetIdTokenIntent(Credentials, Nonce, CurrentTime);
		}, OnResponse, OnFailure);
	}
	else
	{
		OnFailure(FSequenceError(RequestFail, "[Session Not Registered Please Register Session First]"));
	}
	
}

void USequenceRPCManager::ListSessions(const FCredentials_BE& Credentials, const TSuccessCallback<TArray<FSeqListSessions_Session>>& OnSuccess, const FFailureCallback& OnFailure) const
{
	const TSuccessCallback<FString> OnResponse = [OnSuccess,OnFailure](const FString& Response)
	{
		const FSeqListSessionsResponse ParsedResponse = USequenceSupport::JSONStringToStruct<FSeqListSessionsResponse>(Response);

		if (ParsedResponse.IsValid())
		{
			OnSuccess(ParsedResponse.Response.Data);
		}
		else
		{
			OnFailure(FSequenceError(RequestFail, "Error Parsing Response: " + Response));
		}
	};

	if (Credentials.RegisteredValid())
	{
		this->SendIntent(this->BuildAuthenticatorIntentsUrl(),[this, Credentials](TOptional<int64> CurrentTime)
		{
			return this->BuildListSessionIntent(Credentials, CurrentTime);
		},OnResponse,OnFailure);
	}
	else
	{
		OnFailure(FSequenceError(RequestFail, "[Session Not Registered Please Register Session First]"));
	}
}

void USequenceRPCManager::ListAccounts(const FCredentials_BE& Credentials, const TSuccessCallback<FSeqListAccountsResponse_Data>& OnSuccess, const FFailureCallback& OnFailure) const
{
	const TSuccessCallback<FString> OnResponse = [OnSuccess, OnFailure](const FString& Response)
		{
			const FSeqListAccountsResponse ParsedResponse = USequenceSupport::JSONStringToStruct<FSeqListAccountsResponse>(Response);

			SEQ_LOG(Log, TEXT("%s"), *Response);
			if (ParsedResponse.IsValid())
			{
				OnSuccess(ParsedResponse.Response.Data);
			}
			else
			{
				OnFailure(FSequenceError(RequestFail, "Error Parsing Response: " + Response));
			}
		};

	if (Credentials.RegisteredValid())
	{
		this->SendIntent(this->BuildAuthenticatorIntentsUrl(), [this, Credentials](TOptional<int64> CurrentTime)
		{
			return this->BuildListAccountsIntent(Credentials, CurrentTime);
		}, OnResponse, OnFailure);
	}
	else
	{
		OnFailure(FSequenceError(RequestFail, "[Session Not Registered Please Register Session First]"));
	}
}

void USequenceRPCManager::GetSessionAuthProof(const FCredentials_BE& Credentials, const FString& Nonce, const TSuccessCallback<FSeqGetSessionAuthProof_Data>& OnSuccess, const FFailureCallback& OnFailure) const
{
	const TSuccessCallback<FString> OnResponse = [OnSuccess, OnFailure](const FString& Response)
		{
			const FSeqGetSessionAuthProofResponse ParsedResponse = USequenceSupport::JSONStringToStruct<FSeqGetSessionAuthProofResponse>(Response);

			if (ParsedResponse.IsValid())
			{
				OnSuccess(ParsedResponse.Response.Data);
			}
			else
			{
				OnFailure(FSequenceError(RequestFail, "Error Parsing Response: " + Response));
			}
		};

	if (Credentials.RegisteredValid())
	{
		this->SendIntent(this->BuildAuthenticatorIntentsUrl(),[this, Nonce, Credentials](TOptional<int64> CurrentTime)
		{
			return this->BuildGetSessionAuthProofIntent(Credentials, Nonce, CurrentTime);
		},OnResponse,OnFailure);
	}
	else
	{
		OnFailure(FSequenceError(RequestFail, "[Session Not Registered Please Register Session First]"));
	}
}

void USequenceRPCManager::CloseSession(const FCredentials_BE& Credentials, const TFunction<void()>& OnSuccess, const FFailureCallback& OnFailure) const
{
	const TSuccessCallback<FString> OnResponse = [this,OnSuccess,OnFailure](const FString& Response)
	{
		const FSeqCloseSessionResponse ParsedResponse = USequenceSupport::JSONStringToStruct<FSeqCloseSessionResponse>(Response);

		if (ParsedResponse.IsValid())
		{
			OnSuccess();
		}
		else
		{
			OnFailure(FSequenceError(RequestFail, "Error Parsing Response: " + Response));
		}
	};
	if (Credentials.RegisteredValid())
	{
			this->SendIntent(this->BuildAuthenticatorIntentsUrl(), [this](TOptional<int64> Time)
			{
				return this->BuildCloseSessionIntent(Time);
			}, OnResponse, OnFailure);
	}
	else
	{
		OnFailure(FSequenceError(RequestFail, "[Session Not Registered Please Register Session First]"));
	}
}

void USequenceRPCManager::InitEmailAuth(const FString& EmailIn, const TFunction<void()>& OnSuccess, const FFailureCallback& OnFailure)
{
	this->CheckAndUpdateSessionFromPreserveSessionWallet();
	
	const TSuccessCallback<FString> OnResponse = [this, OnSuccess, OnFailure](const FString& Response)
	{
		const FSeqInitiateAuthResponse ParsedInitResponse = USequenceSupport::JSONStringToStruct<FSeqInitiateAuthResponse>(Response);
		
		if (ParsedInitResponse.IsValid())
		{
			this->Cached_Challenge = ParsedInitResponse.Response.Data.Challenge;
			OnSuccess();
		}
		else
		{
			const FString ErrorMessage = FString::Printf(TEXT("Failed to Initiate Email Auth: %s"), *Response);
			OnFailure(FSequenceError(EErrorType::RequestFail, ErrorMessage));
		}
	};
	
	FInitiateAuthData InitiateAuthData;
	InitiateAuthData.InitForEmail(this->SessionWallet->GetSessionId(), EmailIn);
	this->Cached_Verifier = InitiateAuthData.verifier;
	this->Cached_Email = EmailIn;

	this->SendIntent(this->BuildAuthenticatorIntentsUrl(), [this, InitiateAuthData](TOptional<int64> CurrentTime)
	{
		return this->BuildInitiateAuthIntent(InitiateAuthData, CurrentTime);
	}, OnResponse, OnFailure);
}

void USequenceRPCManager::InitEmailFederation(const FString& EmailIn, const TFunction<void()>& OnSuccess, const FFailureCallback& OnFailure)
{
	this->UpdateWithStoredSessionWallet();
	const TSuccessCallback<FString> OnResponse = [this, OnSuccess, OnFailure](const FString& Response)
	{
		const FSeqInitiateAuthResponse ParsedInitResponse = USequenceSupport::JSONStringToStruct<FSeqInitiateAuthResponse>(Response);
		
		if (ParsedInitResponse.IsValid())
		{
			this->Cached_Challenge = ParsedInitResponse.Response.Data.Challenge;
			OnSuccess();
		}
		else
		{
			const FString ErrorMessage = FString::Printf(TEXT("Failed to Initiate Email Auth: %s"), *Response);
			OnFailure(FSequenceError(EErrorType::RequestFail, ErrorMessage));
		}
	};
	
	FInitiateAuthData InitiateAuthData;
	InitiateAuthData.InitForEmail(this->SessionWallet->GetSessionId(), EmailIn);
	this->Cached_Verifier = InitiateAuthData.verifier;
	this->Cached_Email = EmailIn;

	this->SendIntent(this->BuildAuthenticatorIntentsUrl(), [this, InitiateAuthData](TOptional<int64> CurrentTime)
	{
		return this->BuildInitiateAuthIntent(InitiateAuthData, CurrentTime);
	}, OnResponse, OnFailure);
}

void USequenceRPCManager::OpenGuestSession(const bool ForceCreateAccountIn, const TSuccessCallback<FCredentials_BE>& OnSuccess, const FFailureCallback& OnFailure)
{
	this->UpdateWithRandomSessionWallet();
	const TSuccessCallback<FString> OnInitResponse = [this, ForceCreateAccountIn, OnSuccess, OnFailure](const FString& InitResponse)
	{
		const FSeqInitiateAuthResponse ParsedInitResponse = USequenceSupport::JSONStringToStruct<FSeqInitiateAuthResponse>(InitResponse);
		
		if (ParsedInitResponse.IsValid())
		{
			const TSuccessCallback<FString> OnOpenResponse = [this, OnSuccess, OnFailure](const FString& OpenResponse)
			{
				const FSeqOpenSessionResponse ParsedOpenResponse = USequenceSupport::JSONStringToStruct<FSeqOpenSessionResponse>(OpenResponse);
				
				const FCredentials_BE Credentials(
				this->SessionWallet->GetWalletPrivateKeyString(), TEXT(""),
				ParsedOpenResponse.Session.Identity.Email,
				ParsedOpenResponse.Response.Data.Wallet, TEXT(""),
				ParsedOpenResponse.Session.Identity.Type,
				ParsedOpenResponse.Session.Identity.Sub,
				ParsedOpenResponse.Session.UserId,
				ParsedOpenResponse.GetCreatedAt(),
				ParsedOpenResponse.GetRefreshedAt(),
				ParsedOpenResponse.GetExpiresAt());
				
				if (ParsedOpenResponse.IsValid())
				{
					OnSuccess(Credentials);
				}
				else
				{
					const FString ErrorMessage = FString::Printf(TEXT("Error in validation of Response: %s"), *OpenResponse);
					OnFailure(FSequenceError(EErrorType::RequestFail,ErrorMessage));
				}
			};
			
			this->Cached_Challenge = ParsedInitResponse.Response.Data.Challenge;
			FOpenSessionData OpenSessionData;
			OpenSessionData.InitForGuest(this->Cached_Challenge, this->SessionWallet->GetSessionId(), ForceCreateAccountIn);
			this->SendIntent(this->BuildRegisterUrl(), [this, OpenSessionData](TOptional<int64> CurrentTime)
			{
				return this->BuildOpenSessionIntent(OpenSessionData, CurrentTime);
			}, OnOpenResponse, OnFailure);
		}
		else
		{
			const FString ErrorMessage = FString::Printf(TEXT("Failed to Initiate Guest Auth: %s"), *InitResponse);
			OnFailure(FSequenceError(EErrorType::RequestFail, ErrorMessage));
		}
	};
	
	FInitiateAuthData InitiateAuthData;
	InitiateAuthData.InitForGuest(this->SessionWallet->GetSessionId());
	this->Cached_Verifier = this->SessionWallet->GetSessionId();

	this->SendIntent(this->BuildAuthenticatorIntentsUrl(), [this, InitiateAuthData](TOptional<int64> CurrentTime)
	{
		return this->BuildInitiateAuthIntent(InitiateAuthData, CurrentTime);
	}, OnInitResponse, OnFailure);
}

void USequenceRPCManager::OpenEmailSession(const FString& CodeIn, const bool ForceCreateAccountIn, const TSuccessCallback<FCredentials_BE>& OnSuccess, const FFailureCallback& OnFailure, const TFunction<void(FFederationSupportData)>& OnFederationRequired)
{
	FOpenSessionData OpenSessionData;
	OpenSessionData.InitForEmail(this->Cached_Challenge, CodeIn, this->SessionWallet->GetSessionId(), this->Cached_Verifier, ForceCreateAccountIn);
	
	const TSuccessCallback<FString> OnResponse = [this, OpenSessionData, OnSuccess, OnFailure, OnFederationRequired](const FString& Response)
	{
		const FSeqOpenSessionResponse ParsedOpenResponse = USequenceSupport::JSONStringToStruct<FSeqOpenSessionResponse>(Response);

		const FCredentials_BE Credentials(
		this->SessionWallet->GetWalletPrivateKeyString(), TEXT(""),
		ParsedOpenResponse.Session.Identity.Email,
		ParsedOpenResponse.Response.Data.Wallet, TEXT(""),
		ParsedOpenResponse.Session.Identity.Type,
		ParsedOpenResponse.Session.Identity.Sub,
		ParsedOpenResponse.Session.UserId,
		ParsedOpenResponse.GetCreatedAt(),
		ParsedOpenResponse.GetRefreshedAt(),
		ParsedOpenResponse.GetExpiresAt());
		
		if (ParsedOpenResponse.IsValid() && Credentials.RegisteredValid())
		{
			OnSuccess(Credentials);
		}
		else
		{
			const FErrorResponse ErrorResponse = USequenceSupport::JSONStringToStruct<FErrorResponse>(Response);
			if (ErrorResponse.IsEmailInUseError())
			{
				this->Cached_OpenSessionData = OpenSessionData;
				const FFederationSupportData FederationData(ErrorResponse.ParseCauseForRequiredEmail(), ErrorResponse.ParseCauseForAccountUsage());
				this->PreserveSessionWallet = true;
				OnFederationRequired(FederationData);
			}
			else
			{
				const FString ErrorMessage = FString::Printf(TEXT("Error in validation of Response: %s"), *Response);
				OnFailure(FSequenceError(EErrorType::RequestFail,ErrorMessage));
			}
		}
	};
	
	this->SendIntent(this->BuildRegisterUrl(), [this, OpenSessionData](TOptional<int64> CurrentTime)
	{
		return this->BuildOpenSessionIntent(OpenSessionData, CurrentTime);
	}, OnResponse, OnFailure);
}

void USequenceRPCManager::OpenOIDCSession(const FString& IdTokenIn, const bool ForceCreateAccountIn, const TSuccessCallback<FCredentials_BE>& OnSuccess, const FFailureCallback& OnFailure, const TFunction<void(FFederationSupportData)>& OnFederationRequired)
{
	this->CheckAndUpdateSessionFromPreserveSessionWallet();
	
	const TSuccessCallback<FString> OnInitResponse = [this, IdTokenIn, ForceCreateAccountIn, OnSuccess, OnFailure, OnFederationRequired](const FString& InitResponse)
	{		
		const FSeqInitiateAuthResponse ParsedInitResponse = USequenceSupport::JSONStringToStruct<FSeqInitiateAuthResponse>(InitResponse);

		if (ParsedInitResponse.IsValid())
		{
			FOpenSessionData OpenSessionData;
			OpenSessionData.InitForOIDC(IdTokenIn,this->SessionWallet->GetSessionId(),ForceCreateAccountIn);
			
			const TSuccessCallback<FString> OnOpenResponse = [this, OpenSessionData, IdTokenIn, OnSuccess, OnFailure, OnFederationRequired](const FString& OpenResponse)
			{		
				const FSeqOpenSessionResponse ParsedOpenResponse = USequenceSupport::JSONStringToStruct<FSeqOpenSessionResponse>(OpenResponse);
				const FCredentials_BE Credentials(
				this->SessionWallet->GetWalletPrivateKeyString(),
				IdTokenIn,
				ParsedOpenResponse.Session.Identity.Email,
				ParsedOpenResponse.Response.Data.Wallet,
				ParsedOpenResponse.Session.Identity.Iss,
				ParsedOpenResponse.Session.Identity.Type,
				ParsedOpenResponse.Session.Identity.Sub,
				ParsedOpenResponse.Session.UserId,
				ParsedOpenResponse.GetCreatedAt(),
				ParsedOpenResponse.GetRefreshedAt(),
				ParsedOpenResponse.GetExpiresAt());
				
				if (ParsedOpenResponse.IsValid() && Credentials.RegisteredValid())
				{
					OnSuccess(Credentials);
				}
				else
				{
					const FErrorResponse ErrorResponse = USequenceSupport::JSONStringToStruct<FErrorResponse>(OpenResponse);
					if (ErrorResponse.IsEmailInUseError())
					{
						this->Cached_OpenSessionData = OpenSessionData;
						const FFederationSupportData FederationData(ErrorResponse.ParseCauseForRequiredEmail(), ErrorResponse.ParseCauseForAccountUsage());
						this->PreserveSessionWallet = true;
						OnFederationRequired(FederationData);
					}
					else
					{
						const FString ErrorMessage = FString::Printf(TEXT("Error in validation of Response: %s"), *OpenResponse);
						OnFailure(FSequenceError(EErrorType::RequestFail,ErrorMessage));
					}
				}
			};
			
			this->SendIntent(this->BuildRegisterUrl(), [this, OpenSessionData](TOptional<int64> CurrentTime)
			{
				return this->BuildOpenSessionIntent(OpenSessionData, CurrentTime);
			}, OnOpenResponse, OnFailure);
		}
		else
		{
			const FString ErrorMessage = FString::Printf(TEXT("Failed to Initiate OIDC Auth: %s"), *InitResponse);
			OnFailure(FSequenceError(EErrorType::RequestFail, ErrorMessage));
		}
	};
	
	FInitiateAuthData InitiateAuthData;
	InitiateAuthData.InitForOIDC(IdTokenIn, this->SessionWallet->GetSessionId());
	
	this->SendIntent(this->BuildAuthenticatorIntentsUrl(), [this, InitiateAuthData](TOptional<int64> CurrentTime)
	{
		return this->BuildInitiateAuthIntent(InitiateAuthData, CurrentTime);
	}, OnInitResponse, OnFailure);
}

void USequenceRPCManager::OpenPlayFabSession(const FString& SessionTicketIn, const bool ForceCreateAccountIn, const TSuccessCallback<FCredentials_BE>& OnSuccess, const FFailureCallback& OnFailure, const TFunction<void(FFederationSupportData)>& OnFederationRequired)
{
	this->CheckAndUpdateSessionFromPreserveSessionWallet();
	
	const TSuccessCallback<FString> OnInitResponse = [this, SessionTicketIn, ForceCreateAccountIn, OnSuccess, OnFailure, OnFederationRequired] (const FString& InitResponse)
	{
		const FSeqInitiateAuthResponse ParsedInitResponse = USequenceSupport::JSONStringToStruct<FSeqInitiateAuthResponse>(InitResponse);

		if (ParsedInitResponse.IsValid())
		{
			FOpenSessionData OpenSessionData;
			OpenSessionData.InitForPlayFab(SessionTicketIn,this->SessionWallet->GetSessionId(),ForceCreateAccountIn);
			
			const TSuccessCallback<FString> OnOpenResponse = [this, OpenSessionData, OnSuccess, OnFailure, OnFederationRequired] (const FString& OpenResponse)
			{
				const FSeqOpenSessionResponse ParsedOpenResponse = USequenceSupport::JSONStringToStruct<FSeqOpenSessionResponse>(OpenResponse);

				const FCredentials_BE Credentials(
				this->SessionWallet->GetWalletPrivateKeyString(),TEXT(""),
				ParsedOpenResponse.Session.Identity.Email,
				ParsedOpenResponse.Response.Data.Wallet,
				ParsedOpenResponse.Session.Identity.Iss,
				ParsedOpenResponse.Session.Identity.Type,
				ParsedOpenResponse.Session.Identity.Sub,
				ParsedOpenResponse.Session.UserId,
				ParsedOpenResponse.GetCreatedAt(),
				ParsedOpenResponse.GetRefreshedAt(),
				ParsedOpenResponse.GetExpiresAt());
				
				if (ParsedOpenResponse.IsValid())
				{
					OnSuccess(Credentials);
				}
				else
				{
					const FErrorResponse ErrorResponse = USequenceSupport::JSONStringToStruct<FErrorResponse>(OpenResponse);
					if (ErrorResponse.IsEmailInUseError())
					{
						this->Cached_OpenSessionData = OpenSessionData;
						const FFederationSupportData FederationData(ErrorResponse.ParseCauseForRequiredEmail(), ErrorResponse.ParseCauseForAccountUsage());
						this->PreserveSessionWallet = true;
						OnFederationRequired(FederationData);
					}
					else
					{
						const FString ErrorMessage = FString::Printf(TEXT("Error in validation of Response: %s"), *OpenResponse);
						OnFailure(FSequenceError(EErrorType::RequestFail,ErrorMessage));
					}
				}
			};

			this->SendIntent(this->BuildRegisterUrl(), [this, OpenSessionData](TOptional<int64> CurrentTime)
			{
				return this->BuildOpenSessionIntent(OpenSessionData, CurrentTime);
			}, OnOpenResponse, OnFailure);
		}
		else
		{
			const FString ErrorMessage = FString::Printf(TEXT("Failed to Initiate PlayFab Auth: %s"), *InitResponse);
			OnFailure(FSequenceError(EErrorType::RequestFail, ErrorMessage));
		}
	};
	
	FInitiateAuthData InitiateAuthData;
	InitiateAuthData.InitForPlayFab(SessionTicketIn, this->SessionWallet->GetSessionId());


	this->SendIntent(this->BuildAuthenticatorIntentsUrl(), [this, InitiateAuthData](TOptional<int64> CurrentTime)
	{
		return this->BuildInitiateAuthIntent(InitiateAuthData, CurrentTime);
	}, OnInitResponse, OnFailure);
}

void USequenceRPCManager::ForceOpenSessionInUse(const TSuccessCallback<FCredentials_BE>& OnSuccess, const FFailureCallback& OnFailure)
{
	this->CheckAndUpdateSessionFromPreserveSessionWallet();

	const TSuccessCallback<FString> OnOpenResponse = [this, OnSuccess, OnFailure](const FString& OnResponse)
	{
		const FSeqOpenSessionResponse OpenSessionResponse = USequenceSupport::JSONStringToStruct<FSeqOpenSessionResponse>(OnResponse);
		if (OpenSessionResponse.IsValid())
		{
			const FCredentials_BE Credentials(
				this->SessionWallet->GetWalletPrivateKeyString(),TEXT(""),
				OpenSessionResponse.Session.Identity.Email,
				OpenSessionResponse.Response.Data.Wallet,
				OpenSessionResponse.Session.Identity.Iss,
				OpenSessionResponse.Session.Identity.Type,
				OpenSessionResponse.Session.Identity.Sub,
				OpenSessionResponse.Session.UserId,
				OpenSessionResponse.GetCreatedAt(),
				OpenSessionResponse.GetRefreshedAt(),
				OpenSessionResponse.GetExpiresAt());
			OnSuccess(Credentials);
		}
		else
		{
			OnFailure(FSequenceError(EErrorType::RequestFail,OnResponse));
		}
	};

	this->Cached_OpenSessionData.forceCreateAccount = true;
	this->SendIntent(this->BuildRegisterUrl(), [this](TOptional<int64> CurrentTime)
	{
		return this->BuildOpenSessionIntent(this->Cached_OpenSessionData, CurrentTime);
	}, OnOpenResponse, OnFailure);
}

void USequenceRPCManager::GetLinkedWallets(const FSeqLinkedWalletRequest& Request, const TSuccessCallback<FSeqLinkedWalletsResponse>& OnSuccess, const FFailureCallback& OnFailure) const
{
	const TSuccessCallback<FString> OnResponse = [this, OnSuccess](const FString& OnResponse)
	{
		const FSeqLinkedWalletsResponse LinkedWallets = USequenceSupport::JSONStringToStruct<FSeqLinkedWalletsResponse>(OnResponse);
		OnSuccess(LinkedWallets);
	};
	
	this->SendIntent(this->BuildAPIUrl("GetLinkedWallets"),[this, Request](const TOptional<int64>& CurrentTime)
	{
		return USequenceSupport::StructToString(Request);
	}, OnResponse, OnFailure);
}

void USequenceRPCManager::RemoveLinkedWallet(const FSeqLinkedWalletRequest& Request, const TFunction<void()>& OnSuccess, const FFailureCallback& OnFailure) const
{
	const TSuccessCallback<FString> OnResponse = [this, OnSuccess](const FString& OnResponse)
	{
		OnSuccess();
	};
	
	this->SendIntent(this->BuildAPIUrl("RemoveLinkedWallet"),[this, Request](const TOptional<int64>& CurrentTime)
	{
		return USequenceSupport::StructToString(Request);
	}, OnResponse, OnFailure);
}

void USequenceRPCManager::FederateEmailSession(const FString& WalletIn, const FString& CodeIn, const TFunction<void()>& OnSuccess, const FFailureCallback& OnFailure)
{
	this->UpdateWithStoredSessionWallet();
	const TSuccessCallback<FString> OnFederateResponse = [OnSuccess, OnFailure](const FString& FederateResponse)
	{
		const FFederateAccountResponse ParsedFederateResponse = USequenceSupport::JSONStringToStruct<FFederateAccountResponse>(FederateResponse);

		if (ParsedFederateResponse.IsValid())
		{
			OnSuccess();
		}
		else
		{
			const FString ErrorMessage = FString::Printf(TEXT("Failed to Federate PlayFab Auth: %s"), *FederateResponse);
			OnFailure(FSequenceError(EErrorType::RequestFail, ErrorMessage));
		}
	};
	
	FFederateAccountData FederateAccount;
	FederateAccount.InitForEmail(WalletIn, this->Cached_Challenge, CodeIn,this->SessionWallet->GetSessionId(), this->Cached_Verifier);

	this->SendIntent(this->BuildAuthenticatorIntentsUrl(), [this, FederateAccount](TOptional<int64> CurrentTime)
	{
		return this->BuildFederateAccountIntent(FederateAccount, CurrentTime);
	}, OnFederateResponse, OnFailure);
}

void USequenceRPCManager::FederateOIDCSession(const FString& WalletIn, const FString& IdTokenIn, const TFunction<void()>& OnSuccess, const FFailureCallback& OnFailure)
{
	this->UpdateWithStoredSessionWallet();
	const TSuccessCallback<FString> OnInitiateResponse = [this, WalletIn, IdTokenIn, OnSuccess, OnFailure](const FString& InitResponse)
	{
		const FSeqInitiateAuthResponse ParsedInitResponse = USequenceSupport::JSONStringToStruct<FSeqInitiateAuthResponse>(InitResponse);

		if (ParsedInitResponse.IsValid())
		{
			const TSuccessCallback<FString> OnFederateResponse = [OnSuccess, OnFailure](const FString& FederateResponse)
			{
				const FFederateAccountResponse ParsedFederateResponse = USequenceSupport::JSONStringToStruct<FFederateAccountResponse>(FederateResponse);

				if (ParsedFederateResponse.IsValid())
				{
					OnSuccess();
				}
				else
				{
					const FString ErrorMessage = FString::Printf(TEXT("Failed to Federate OIDC Auth: %s"), *FederateResponse);
					OnFailure(FSequenceError(EErrorType::RequestFail, ErrorMessage));
				}
			};
	
			FFederateAccountData FederateAccount;
			FederateAccount.InitForOIDC(WalletIn, IdTokenIn,this->SessionWallet->GetSessionId());

			this->SendIntent(this->BuildAuthenticatorIntentsUrl(), [this, FederateAccount](TOptional<int64> CurrentTime)
			{
				return this->BuildFederateAccountIntent(FederateAccount, CurrentTime);
			}, OnFederateResponse, OnFailure);
		}
		else
		{
			const FString ErrorMessage = FString::Printf(TEXT("Failed to Initiate OIDC Auth: %s"), *InitResponse);
			OnFailure(FSequenceError(EErrorType::RequestFail, ErrorMessage));
		}
	};

	FInitiateAuthData AuthData;
	AuthData.InitForOIDC(IdTokenIn, this->SessionWallet->GetSessionId());

	this->SendIntent(this->BuildAuthenticatorIntentsUrl(), [this, AuthData](TOptional<int64> CurrentTime)
	{
		return this->BuildInitiateAuthIntent(AuthData, CurrentTime);
	}, OnInitiateResponse, OnFailure);
}

void USequenceRPCManager::FederatePlayFabSession(const FString& WalletIn, const FString& SessionTicketIn, const TFunction<void()>& OnSuccess, const FFailureCallback& OnFailure)
{
	this->UpdateWithStoredSessionWallet();
	const TSuccessCallback<FString> OnInitiateResponse = [this, WalletIn, SessionTicketIn, OnSuccess, OnFailure](const FString& InitResponse)
	{
		const FSeqInitiateAuthResponse ParsedInitResponse = USequenceSupport::JSONStringToStruct<FSeqInitiateAuthResponse>(InitResponse);

		if (ParsedInitResponse.IsValid())
		{		
			const TSuccessCallback<FString> OnFederateResponse = [this, SessionTicketIn, OnSuccess, OnFailure](const FString& FederateResponse)
			{
				const FFederateAccountResponse ParsedFederateResponse = USequenceSupport::JSONStringToStruct<FFederateAccountResponse>(FederateResponse);

				if (ParsedFederateResponse.IsValid())
				{
					OnSuccess();
				}
				else
				{
					const FString ErrorMessage = FString::Printf(TEXT("Failed to Federate PlayFab Auth: %s"), *FederateResponse);
					OnFailure(FSequenceError(EErrorType::RequestFail, ErrorMessage));
				}
			};
	
			FFederateAccountData FederateAccount;
			FederateAccount.InitForPlayFab(WalletIn, SessionTicketIn,this->SessionWallet->GetSessionId());

			this->SendIntent(this->BuildAuthenticatorIntentsUrl(), [this, FederateAccount](TOptional<int64> CurrentTime)
			{
				return this->BuildFederateAccountIntent(FederateAccount, CurrentTime);
			}, OnFederateResponse, OnFailure);
		}
		else
		{
			const FString ErrorMessage = FString::Printf(TEXT("Failed to Initiate PlayFab Auth: %s"), *InitResponse);
			OnFailure(FSequenceError(EErrorType::RequestFail, ErrorMessage));
		}
	};

	FInitiateAuthData AuthData;
	AuthData.InitForPlayFab(SessionTicketIn, this->SessionWallet->GetSessionId());

	this->SendIntent(this->BuildAuthenticatorIntentsUrl(), [this, AuthData](TOptional<int64> CurrentTime)
	{
		return this->BuildInitiateAuthIntent(AuthData, CurrentTime);
	}, OnInitiateResponse, OnFailure);
}

void USequenceRPCManager::FederateSessionInUse(const FString& WalletIn, const TFunction<void()>& OnSuccess, const FFailureCallback& OnFailure) const
{
	const TSuccessCallback<FString> OnFederateResponse = [this, OnSuccess, OnFailure](const FString& FederateResponse)
	{
		const FFederateAccountResponse FederateAccountResponse = USequenceSupport::JSONStringToStruct<FFederateAccountResponse>(FederateResponse);

		if (FederateAccountResponse.IsValid())
		{
			OnSuccess();
		}
		else
		{
			OnFailure(FSequenceError(EErrorType::RequestFail, FederateResponse));
		}
	};
	
	FFederateAccountData FederateAccountData;
	FederateAccountData.InitForFederation(this->Cached_OpenSessionData, WalletIn);

	this->SendIntent(this->BuildAuthenticatorIntentsUrl(), [this, FederateAccountData](TOptional<int64> CurrentTime)
	{
		return this->BuildFederateAccountIntent(FederateAccountData, CurrentTime);
	}, OnFederateResponse, OnFailure);
}

void USequenceRPCManager::InitializeTimeShift()
{
	const FString WaasUrl = this->WaaSSettings.GetRPCServer();
	const FString StatusUrl = WaasUrl.EndsWith(TEXT("/")) ? WaasUrl + TEXT("status") : WaasUrl + TEXT("/status");

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetURL(StatusUrl);
	
	HttpRequest->OnProcessRequestComplete().BindLambda([this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
	{
		if (bSuccess && Response.IsValid())
		{
			const FString DateHeader = Response->GetHeader(TEXT("date"));
			if (!DateHeader.IsEmpty())
			{
				TimeShift = GetTimeShiftFromResponse(DateHeader);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("No date header in response from status endpoint"));
				TimeShift = FTimespan::Zero();
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to get server time for time shift calculation"));
			TimeShift = FTimespan::Zero();
		}
	});

	HttpRequest->ProcessRequest();
}

FTimespan USequenceRPCManager::GetTimeShiftFromResponse(const FString& DateHeader)
{
	FDateTime ServerTime;
	if (FDateTime::ParseHttpDate(DateHeader, ServerTime))
	{
		const FDateTime LocalTime = FDateTime::UtcNow();
		return ServerTime - LocalTime;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Failed to parse server time from date header: %s"), *DateHeader);
	return FTimespan::Zero();
}
