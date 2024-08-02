// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "SequenceRPCManager.h"
#include "RequestHandler.h"
#include "Sequence/Close.h"
#include "ConfigFetcher.h"
#include "Types/BinaryData.h"
#include "Sequence/SequenceResponseIntent.h"

template<typename T> FString USequenceRPCManager::GenerateIntent(T Data) const
{
	const int64 Issued = FDateTime::UtcNow().ToUnixTimestamp() - 30;
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

template <typename T>
void USequenceRPCManager::SequenceRPC(const FString& Url, const FString& Content, TSuccessCallback<T> OnSuccess, FFailureCallback OnFailure)
{
	UE_LOG(LogTemp, Display, TEXT("Url: %s\nIntent: %s"), *Url, *Content);
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

FString USequenceRPCManager::BuildGetFeeOptionsIntent(const FCredentials_BE& Credentials, const TArray<TransactionUnion>& Txns) const
{
	const FGetFeeOptionsData GetFeeOptionsData(Credentials.GetNetworkString(),Txns,Credentials.GetWalletAddress());
	const FString Intent = this->GenerateIntent<FGetFeeOptionsData>(GetFeeOptionsData);
	return Intent;
}

FString USequenceRPCManager::BuildSignMessageIntent(const FCredentials_BE& Credentials, const FString& Message) const
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
	
	const FSignMessageData SignMessageData(EIP_Message,Credentials.GetNetworkString(),Credentials.GetWalletAddress());
	const FString Intent = this->GenerateIntent<FSignMessageData>(SignMessageData);

	return Intent;
}

FString USequenceRPCManager::BuildSendTransactionIntent(const FCredentials_BE& Credentials, const TArray<TransactionUnion>& Txns) const
{
	const FString Identifier = "unreal-sdk-" + FDateTime::UtcNow().ToString() + "-" + Credentials.GetWalletAddress();
	const FSendTransactionData SendTransactionData(Identifier,Credentials.GetNetworkString(),Txns,Credentials.GetWalletAddress());
	const FString Intent = this->GenerateIntent<FSendTransactionData>(SendTransactionData);
	return Intent;
}

FString USequenceRPCManager::BuildSendTransactionWithFeeIntent(const FCredentials_BE& Credentials, const TArray<TransactionUnion>& Txns, const FString& FeeQuote) const
{
	const FString Identifier = "unreal-sdk-" + FDateTime::UtcNow().ToString() + "-" + Credentials.GetWalletAddress();
	const FSendTransactionWithFeeOptionData SendTransactionWithFeeOptionData(Identifier,Credentials.GetNetworkString(),Txns,FeeQuote,Credentials.GetWalletAddress());
	const FString Intent = this->GenerateIntent<FSendTransactionWithFeeOptionData>(SendTransactionWithFeeOptionData);
	return Intent;
}

FString USequenceRPCManager::BuildListSessionIntent(const FCredentials_BE& Credentials) const
{
	const FListSessionsData ListSessionsData(Credentials.GetWalletAddress());
	const FString Intent = this->GenerateIntent<FListSessionsData>(ListSessionsData);
	return Intent;
}

FString USequenceRPCManager::BuildCloseSessionIntent() const
{
	const FCloseSessionData CloseSessionData(this->SessionWallet->GetSessionId());
	const FString Intent = this->GenerateIntent(CloseSessionData);
	return Intent;
}

FString USequenceRPCManager::BuildSessionValidationIntent() const
{
	const FString Intent = "{\\\"sessionId\\\":\\\"" + this->SessionWallet->GetSessionId() + "\\\"}";
	return Intent;
}

FString USequenceRPCManager::BuildOpenSessionIntent(const FOpenSessionData& OpenSessionData) const
{
	return this->GenerateIntent<FOpenSessionData>(OpenSessionData);
}

FString USequenceRPCManager::BuildInitiateAuthIntent(const FInitiateAuthData& InitiateAuthData) const
{
	return this->GenerateIntent<FInitiateAuthData>(InitiateAuthData);
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

FString USequenceRPCManager::BuildUrl() const
{
	return this->WaaSSettings.GetRPCServer() + this->UrlPath;
}

FString USequenceRPCManager::BuildRegisterUrl() const
{
	return this->WaaSSettings.GetRPCServer() + this->UrlRegisterPath;
}

TArray<FFeeOption> USequenceRPCManager::JsonFeeOptionListToFeeOptionList(const TArray<TSharedPtr<FJsonValue>>& FeeOptionList)
{
	TArray<FFeeOption> ParsedFeeOptionList;
	for (auto FeeOption : FeeOptionList)
	{
		ParsedFeeOptionList.Add(FFeeOption(FeeOption));
	}
	return ParsedFeeOptionList;
}

USequenceRPCManager* USequenceRPCManager::Make(UWallet* SessionWalletIn)
{
	USequenceRPCManager * SequenceRPCManager = NewObject<USequenceRPCManager>();
	SequenceRPCManager->SessionWallet = SessionWalletIn;

	FString ParsedJwt;
	FBase64::Decode(UConfigFetcher::GetConfigVar(UConfigFetcher::WaaSConfigKey),ParsedJwt);
	SequenceRPCManager->WaaSSettings = UIndexerSupport::JSONStringToStruct<FWaasJWT>(ParsedJwt);
	SequenceRPCManager->Cached_ProjectAccessKey = UConfigFetcher::GetConfigVar(UConfigFetcher::ProjectAccessKey);
	return SequenceRPCManager;
}

void USequenceRPCManager::SignMessage(const FCredentials_BE& Credentials, const FString& Message, const TSuccessCallback<FSignedMessage>& OnSuccess, const FFailureCallback& OnFailure)
{
	const TSuccessCallback<FString> OnResponse = [OnSuccess,OnFailure](const FString& Response)
	{
		const TSharedPtr<FJsonObject> Json = UIndexerSupport::JsonStringToObject(Response);
		FSignedMessageResponseObj Msg;
		if (FJsonObjectConverter::JsonObjectToUStruct<FSignedMessageResponseObj>(Json.ToSharedRef(), &Msg))
		{
			const FString ParsedResponse = UIndexerSupport::StructToString(Msg);
			OnSuccess(Msg.response);
		}
		else
		{
			OnFailure(FSequenceError(RequestFail, "Malformed Response: " + Response));
		}
	};

	if (Credentials.RegisteredValid())
	{
		this->SequenceRPC(this->BuildUrl(),this->BuildSignMessageIntent(Credentials, Message),OnResponse,OnFailure);
	}
	else
	{
		OnFailure(FSequenceError(RequestFail, "[Session Not Registered Please Register Session First]"));
	}
}

void USequenceRPCManager::SendTransaction(const FCredentials_BE& Credentials, const TArray<TransactionUnion>& Transactions, const TSuccessCallback<FTransactionResponse>& OnSuccess, const FFailureCallback& OnFailure)
{
	const TSuccessCallback<FString> OnResponse = [OnSuccess, OnFailure](const FString& Response)
	{
		TSharedPtr<FJsonObject> jsonObj;
		if(FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Response), jsonObj))
		{
			const TSharedPtr<FJsonObject> * ResponseObj = nullptr;
			if (jsonObj->TryGetObjectField(TEXT("response"),ResponseObj))
			{
				const TSharedPtr<FJsonObject> * DataObj = nullptr;
				if (ResponseObj->Get()->TryGetObjectField(TEXT("data"),DataObj))
				{
					const FString JsonString = UIndexerSupport::JsonToParsableString(*DataObj);
					FTransactionResponse TxnResponse = UIndexerSupport::JSONStringToStruct<FTransactionResponse>(JsonString);
					TxnResponse.Setup(*DataObj);
					OnSuccess(TxnResponse);
				}
				else
				{
					UE_LOG(LogTemp,Error,TEXT("Error in 2nd level parsing in Transaction"));
					OnFailure(FSequenceError(RequestFail, "Request failed: " + Response));
				}
			}
			else
			{
				UE_LOG(LogTemp,Error,TEXT("Error in 1st level parsing in TransactionResponse"));
				OnFailure(FSequenceError(RequestFail, "Request failed: " + Response));
			}
		}
		else
		{
			OnFailure(FSequenceError(RequestFail, "Request failed: " + Response));
		}
	};
	
	if (Credentials.RegisteredValid())
	{
		this->SequenceRPC(this->BuildUrl(),BuildSendTransactionIntent(Credentials, Transactions),OnResponse,OnFailure);
	}
	else
	{
		OnFailure(FSequenceError(RequestFail, "[Session Not Registered Please Register Session First]"));
	}
}

void USequenceRPCManager::SendTransactionWithFeeOption(const FCredentials_BE& Credentials, TArray<TransactionUnion> Transactions, FFeeOption FeeOption, const TSuccessCallback<FTransactionResponse>& OnSuccess, const FFailureCallback& OnFailure)
{
	Transactions.Insert(FeeOption.CreateTransaction(),0);
	const TSuccessCallback<FString> OnResponse = [OnSuccess, OnFailure](const FString& Response)
	{
		TSharedPtr<FJsonObject> jsonObj;
		if(FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Response), jsonObj))
		{
			const TSharedPtr<FJsonObject> * ResponseObj = nullptr;
			if (jsonObj->TryGetObjectField(TEXT("response"),ResponseObj))
			{
				const TSharedPtr<FJsonObject> * DataObj = nullptr;
				if (ResponseObj->Get()->TryGetObjectField(TEXT("data"),DataObj))
				{
					const FString JsonString = UIndexerSupport::JsonToParsableString(*DataObj);
					FTransactionResponse TxnResponse = UIndexerSupport::JSONStringToStruct<FTransactionResponse>(JsonString);
					TxnResponse.Setup(*DataObj);
					OnSuccess(TxnResponse);
				}
				else
				{
					UE_LOG(LogTemp,Error,TEXT("Error in 2nd level parsing in Transaction"));
					OnFailure(FSequenceError(RequestFail, "Request failed: " + Response));
				}
			}
			else
			{
				UE_LOG(LogTemp,Error,TEXT("Error in 1st level parsing in TransactionResponse"));
				OnFailure(FSequenceError(RequestFail, "Request failed: " + Response));
			}
		}
		else
		{
			OnFailure(FSequenceError(RequestFail, "Request failed: " + Response));
		}
	};
	
	if (Credentials.RegisteredValid())
	{
		this->SequenceRPC(this->BuildUrl(),BuildSendTransactionWithFeeIntent(Credentials, Transactions,this->Cached_FeeQuote),OnResponse,OnFailure);
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
		TSharedPtr<FJsonObject> jsonObj;
		if(FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Response), jsonObj))
		{
			const TSharedPtr<FJsonObject> * ResponseObj = nullptr;
			if (jsonObj->TryGetObjectField(TEXT("response"),ResponseObj))
			{
				const TSharedPtr<FJsonObject> * DataObj = nullptr;
				if (ResponseObj->Get()->TryGetObjectField(TEXT("data"),DataObj))
				{
					const TArray<TSharedPtr<FJsonValue>> * FeeList = nullptr;
					if (DataObj->Get()->TryGetArrayField(TEXT("feeOptions"),FeeList) && DataObj->Get()->TryGetStringField(TEXT("feeQuote"),this->Cached_FeeQuote))
					{	
						const TArray<FFeeOption> Fees = this->JsonFeeOptionListToFeeOptionList(*FeeList);//need this
						OnSuccess(Fees);
					}
					else
					{
						UE_LOG(LogTemp, Error, TEXT("Error in 3rd level parsing GetFeeOptions"));
						OnFailure(FSequenceError(RequestFail, "Request failed: " + Response));
					}
				}
				else
				{
					UE_LOG(LogTemp,Error,TEXT("Error in 2nd level parsing in Transaction"));
					OnFailure(FSequenceError(RequestFail, "Request failed: " + Response));
				}
			}
			else
			{
				UE_LOG(LogTemp,Error,TEXT("Error in 1st level parsing in TransactionResponse"));
				OnFailure(FSequenceError(RequestFail, "Request failed: " + Response));
			}
		}
		else
		{
			OnFailure(FSequenceError(RequestFail, "Request failed: " + Response));
		}
	};
	
	if (Credentials.RegisteredValid())
	{
		this->SequenceRPC(this->BuildUrl(),BuildGetFeeOptionsIntent(Credentials, Transactions),OnResponse,OnFailure);
	}
	else
	{
		OnFailure(FSequenceError(RequestFail, "[Session Not Registered Please Register Session First]"));
	}
}

void USequenceRPCManager::ListSessions(const FCredentials_BE& Credentials, const TSuccessCallback<TArray<FSession>>& OnSuccess, const FFailureCallback& OnFailure)
{
	const TSuccessCallback<FString> OnResponse = [OnSuccess,OnFailure](const FString& Response)
	{
		TArray<FSession> Sessions;
		const TSharedPtr<FJsonObject> Json = UIndexerSupport::JsonStringToObject(Response);
		FListSessionsResponseObj ResponseStruct;
		if (FJsonObjectConverter::JsonObjectToUStruct<FListSessionsResponseObj>(Json.ToSharedRef(), &ResponseStruct))
		{
			const FString ParsedResponse = UIndexerSupport::StructToString(ResponseStruct);
			OnSuccess(ResponseStruct.response.data);
		}
		else
		{
			OnFailure(FSequenceError(RequestFail, "2nd Level Parsing: Request failed: " + Response));
		}
	};
	
	if (Credentials.RegisteredValid())
	{
		this->SequenceRPC(this->BuildUrl(),this->BuildListSessionIntent(Credentials),OnResponse,OnFailure);
	}
	else
	{
		OnFailure(FSequenceError(RequestFail, "[Session Not Registered Please Register Session First]"));
	}
}

void USequenceRPCManager::CloseSession(const FCredentials_BE& Credentials, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure)
{
	const TSuccessCallback<FString> OnResponse = [this,OnSuccess,OnFailure](const FString& Response)
	{
		const TSharedPtr<FJsonObject> Json = UIndexerSupport::JsonStringToObject(Response);
		FCloseResponseObj ResponseStruct;
		if (FJsonObjectConverter::JsonObjectToUStruct<FCloseResponseObj>(Json.ToSharedRef(), &ResponseStruct))
		{
			OnSuccess(ResponseStruct.response.code);
		}
		else
		{
			OnFailure(FSequenceError(RequestFail, "2nd Level Parsing: Request failed: " + Response));
		}
	};
	if (Credentials.RegisteredValid())
	{
		this->SequenceRPC(this->BuildUrl(),this->BuildCloseSessionIntent(),OnResponse,OnFailure);
	}
	else
	{
		OnFailure(FSequenceError(RequestFail, "[Session Not Registered Please Register Session First]"));
	}
}

void USequenceRPCManager::InitEmailAuth(const FString& EmailIn, const TFunction<void()>& OnSuccess, const FFailureCallback& OnFailure)
{
	const TSuccessCallback<FString> OnResponse = [this, OnSuccess, OnFailure](const FString& Response)
	{
		UE_LOG(LogTemp, Display, TEXT("Response: %s"), *Response);
		
		const FInitiateSqcAuthResponse StructResponse = UIndexerSupport::JSONStringToStruct<FInitiateSqcAuthResponse>(Response);
		
		if (StructResponse.IsValid())
		{
			this->Cached_Challenge = StructResponse.Response.Data.Challenge;
			OnSuccess();
		}
		else
		{
			OnFailure(FSequenceError(EErrorType::RequestFail, TEXT("Error Initiating Email Auth")));
		}
	};
	
	FInitiateAuthData InitiateAuthData;
	InitiateAuthData.InitForEmail(this->SessionWallet->GetSessionId(), EmailIn);
	this->Cached_Verifier = InitiateAuthData.verifier;
	this->Cached_Email = EmailIn;
	this->SequenceRPC(this->BuildUrl(), this->BuildInitiateAuthIntent(InitiateAuthData), OnResponse, OnFailure);
}

void USequenceRPCManager::OpenGuestSession(const bool ForceCreateAccountIn, const TSuccessCallback<FCredentials_BE>& OnSuccess, const FFailureCallback& OnFailure)
{
	const TSuccessCallback<FString> OnInitResponse = [this, ForceCreateAccountIn, OnSuccess, OnFailure](const FString& Response)
	{
		UE_LOG(LogTemp, Display, TEXT("Response: %s"), *Response);
		
		const FInitiateSqcAuthResponse StructResponse = UIndexerSupport::JSONStringToStruct<FInitiateSqcAuthResponse>(Response);
		
		if (StructResponse.IsValid())
		{
			const TSuccessCallback<FString> OnOpenResponse = [ForceCreateAccountIn, OnSuccess, OnFailure](const FString& Response)
			{
				const FOpenSessionResponse OpenSessionResponse = UIndexerSupport::JSONStringToStruct<FOpenSessionResponse>(Response);

				UE_LOG(LogTemp, Display, TEXT("Response: %s"), *UIndexerSupport::StructToString(OpenSessionResponse));
			};
			
			this->Cached_Challenge = StructResponse.Response.Data.Challenge;
			FOpenSessionData OpenSessionData;
			OpenSessionData.InitForGuest(this->Cached_Challenge, this->SessionWallet->GetSessionId(), ForceCreateAccountIn);
			this->SequenceRPC(this->BuildRegisterUrl(), this->BuildOpenSessionIntent(OpenSessionData), OnOpenResponse, OnFailure);
		}
		else
		{
			OnFailure(FSequenceError(EErrorType::RequestFail, TEXT("Error Initiating Guest Auth")));
		}
	};
	
	FInitiateAuthData InitiateAuthData;
	InitiateAuthData.InitForGuest(this->SessionWallet->GetSessionId());
	this->Cached_Verifier = this->SessionWallet->GetSessionId();
	this->SequenceRPC(this->BuildUrl(), this->BuildInitiateAuthIntent(InitiateAuthData), OnInitResponse, OnFailure);
}

void USequenceRPCManager::OpenEmailSession(const FString& CodeIn, const bool ForceCreateAccountIn, const TSuccessCallback<FCredentials_BE>& OnSuccess, const FFailureCallback& OnFailure)
{
	const TSuccessCallback<FString> OnResponse = [this, OnSuccess, OnFailure](const FString& Response)
	{
		UE_LOG(LogTemp, Display, TEXT("Response: %s"), *Response);
		const FOpenSessionResponse ParsedResponse = UIndexerSupport::JSONStringToStruct<FOpenSessionResponse>(Response);

		const FCredentials_BE Credentials(
		this->SessionWallet->GetWalletPrivateKeyString(), TEXT(""),
		ParsedResponse.Session.Identity.Email,
		ParsedResponse.Response.Data.Wallet, TEXT(""),
		ParsedResponse.Session.Identity.Type,
		ParsedResponse.Session.Identity.Sub,
		ParsedResponse.Session.UserId,
		ParsedResponse.GetCreatedAt(),
		ParsedResponse.GetRefreshedAt(),
		ParsedResponse.GetExpiresAt());
		
		if (ParsedResponse.IsValid() && Credentials.RegisteredValid())
		{
			OnSuccess(Credentials);
		}
		else
		{
			const FString ErrorMessage = FString::Printf(TEXT("Error in validation of Response: %s"), *Response);
			OnFailure(FSequenceError(EErrorType::RequestFail,ErrorMessage));
		}
	};

	FOpenSessionData OpenSessionData;
	OpenSessionData.InitForEmail(this->Cached_Challenge, CodeIn, this->SessionWallet->GetSessionId(), this->Cached_Verifier, ForceCreateAccountIn);
	this->SequenceRPC(this->BuildRegisterUrl(), this->BuildOpenSessionIntent(OpenSessionData), OnResponse, OnFailure);
}

void USequenceRPCManager::OpenOIDCSession(const FString& IdTokenIn, const bool ForceCreateAccountIn, const TSuccessCallback<FCredentials_BE>& OnSuccess, const FFailureCallback& OnFailure)
{
	const TSuccessCallback<FString> OnInitResponse = [this, IdTokenIn, ForceCreateAccountIn, OnSuccess, OnFailure](const FString& Response)
	{
		UE_LOG(LogTemp, Display, TEXT("Response: %s"), *Response);
		
		const FInitiateSqcAuthResponse StructResponse = UIndexerSupport::JSONStringToStruct<FInitiateSqcAuthResponse>(Response);

		if (StructResponse.IsValid())
		{
			const TSuccessCallback<FString> OnOpenResponse = [this, IdTokenIn, OnSuccess, OnFailure](const FString& Response)
			{		
				const FOpenSessionResponse ParsedResponse = UIndexerSupport::JSONStringToStruct<FOpenSessionResponse>(Response);
				UE_LOG(LogTemp, Display, TEXT("Response: %s"), *Response);
				const FCredentials_BE Credentials(
				this->SessionWallet->GetWalletPrivateKeyString(),
				IdTokenIn,
				ParsedResponse.Session.Identity.Email,
				ParsedResponse.Response.Data.Wallet,
				ParsedResponse.Session.Identity.Iss,
				ParsedResponse.Session.Identity.Type,
				ParsedResponse.Session.Identity.Sub,
				ParsedResponse.Session.UserId,
				ParsedResponse.GetCreatedAt(),
				ParsedResponse.GetRefreshedAt(),
				ParsedResponse.GetExpiresAt());
				
				if (ParsedResponse.IsValid() && Credentials.RegisteredValid())
				{
					OnSuccess(Credentials);
				}
				else
				{
					const FString ErrorMessage = FString::Printf(TEXT("Error in validation of Response: %s"), *Response);
					OnFailure(FSequenceError(EErrorType::RequestFail,ErrorMessage));
				}
			};

			FOpenSessionData OpenSessionData;
			OpenSessionData.InitForOIDC(IdTokenIn,this->SessionWallet->GetSessionId(),ForceCreateAccountIn);
			this->SequenceRPC(this->BuildRegisterUrl(), this->BuildOpenSessionIntent(OpenSessionData), OnOpenResponse, OnFailure);
		}
		else
		{
			OnFailure(FSequenceError(EErrorType::RequestFail, TEXT("Failed to Initiate OIDC Auth")));
		}
	};
	
	FInitiateAuthData InitiateAuthData;
	InitiateAuthData.InitForOIDC(IdTokenIn, this->SessionWallet->GetSessionId());
	this->SequenceRPC(this->BuildUrl(), this->BuildInitiateAuthIntent(InitiateAuthData), OnInitResponse, OnFailure);
}

void USequenceRPCManager::OpenPlayFabSession(const FString& SessionTicketIn, const bool ForceCreateAccountIn, const TSuccessCallback<FCredentials_BE>& OnSuccess, const FFailureCallback& OnFailure)
{
	//We read title ID from Sequence Config
}
