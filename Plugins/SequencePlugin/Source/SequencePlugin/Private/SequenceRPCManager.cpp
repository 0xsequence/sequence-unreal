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

FString USequenceRPCManager::BuildCloseSessionIntent(const FCredentials_BE& Credentials) const
{
	const FCloseSessionData CloseSessionData(Credentials.GetSessionId());
	const FString Intent = this->GenerateIntent(CloseSessionData);
	return Intent;
}

FString USequenceRPCManager::BuildSessionValidationIntent(const FCredentials_BE& Credentials) const
{
	const FString Intent = "{\\\"sessionId\\\":\\\"" + Credentials.GetSessionId() + "\\\"}";
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
		this->SequenceRPC(this->BuildUrl(),this->BuildCloseSessionIntent(Credentials),OnResponse,OnFailure);
	}
	else
	{
		OnFailure(FSequenceError(RequestFail, "[Session Not Registered Please Register Session First]"));
	}
}

void USequenceRPCManager::InitEmailAuth(const FString& EmailIn, const TFunction<void()>& OnSuccess, const FFailureCallback& OnFailure)
{
	const TSuccessCallback<FString> OnResponse = [this, OnSuccess, OnFailure] (const FString& Response)
	{
		UE_LOG(LogTemp, Display, TEXT("Response: %s"), *Response);
		
		const TSharedPtr<FJsonObject> JsonObj = UIndexerSupport::JsonStringToObject(Response);
		const TSharedPtr<FJsonObject> * ResponseObj;
		if (JsonObj.Get()->TryGetObjectField(TEXT("response"), ResponseObj))
		{
			const TSharedPtr<FJsonObject> * DataObj;
			if (ResponseObj->Get()->TryGetObjectField(TEXT("data"), DataObj))
			{
				if (DataObj->Get()->TryGetStringField(TEXT("challenge"), this->Cached_Challenge))
				{
					OnSuccess();
				}
				else
				{
					OnFailure(FSequenceError(RequestFail, "3rd Level Parsing: Request failed: " + Response));
				}
			}
			else
			{
				OnFailure(FSequenceError(RequestFail, "2nd Level Parsing: Request failed: " + Response));
			}
		}
		else
		{
			OnFailure(FSequenceError(RequestFail, "1st Level Parsing: Request failed: " + Response));
		}
	};
	
	FInitiateAuthData InitiateAuthData;
	InitiateAuthData.InitForEmail(this->SessionWallet->GetSessionId(), EmailIn);
	this->Cached_Verifier = InitiateAuthData.verifier;
	this->SequenceRPC(this->BuildUrl(), this->BuildInitiateAuthIntent(InitiateAuthData), OnResponse, OnFailure);
}

void USequenceRPCManager::InitGuestAuth(const TSuccessCallback<FCredentials_BE>& OnSuccess, const FFailureCallback& OnFailure)
{

}

/*
 *
Url: https://waas.sequence.app/rpc/WaasAuthenticator/SendIntent
Intent: {"intent":{"data":{"identityType":"Email","metadata":"","sessionId":"0x00dea18e2a0e3a840a2650573e6abb06cb137214e8","verifier":"calvin.kork@zemind.ca;0x00dea18e2a0e3a840a2650573e6abb06cb137214e8"},"expiresAt":1722540041,"issuedAt":1722453641,"name":"initiateAuth","signatures":[{"sessionId":"0x00dea18e2a0e3a840a2650573e6abb06cb137214e8","signature":"0xa896f7a2b68bed8c9e96a87c06424747181a831b38d01487505d56225bc122900c9d47e12b764df6fd34e16c84bbd779571daf38fdba9456d36bd45d4b4feffa01"}],"version":"1.0.0"}}
Response: {"response":{"code":"authInitiated","data":{"sessionId":"0x00dea18e2a0e3a840a2650573e6abb06cb137214e8","identityType":"Email","expiresIn":-1799,"challenge":"0x1a5f10965e3c68f7715e294a"}}}

Url: https://waas.sequence.app/rpc/WaasAuthenticator/SendIntent
Intent: {"intent":{"data":{"answer":"0x7EBA217A6ACF0106795723A73FA7E115511C9D7A343B0C61F0F031F8209BC205","forceCreateAccount":false,"identityType":"Email","sessionId":"0x00dea18e2a0e3a840a2650573e6abb06cb137214e8","verifier":"calvin.kork@zemind.ca;0x00dea18e2a0e3a840a2650573e6abb06cb137214e8"},"expiresAt":1722540055,"issuedAt":1722453655,"name":"openSession","signatures":[{"sessionId":"0x00dea18e2a0e3a840a2650573e6abb06cb137214e8","signature":"0x4a93bf7a7171dacd29dfc85a7a51a14c5951e805ce3bca806cfd0deb25479eba516b22a90233f268223364aea11fbe28f215aa4cfb3d47af52a52f6af0ff909401"}],"version":"1.0.0"},"friendlyName":"D8ED258341AD3161F709468A14152BAB"}
Response: {"error":"WebrpcEndpoint","code":0,"msg":"endpoint error","cause":"session invalid or not found","status":400}
* 
*/

//Create a struct for this
//{"session":{"id":"0x001eac35765645a0edef0d80b7b8118d4c062d74a6","projectId":37066,"userId":"37066|0x821bddcca74286cc900e1bbbd6842fd7d0ff0f559506404ab441afd14e828236","identity":{"type":"Email","sub":"calvin.kork@zemind.ca","email":"calvin.kork@zemind.ca"},"friendlyName":"9DD606234E75151D62D8109D003EBEE3","createdAt":"2024-07-31T20:12:05.875997447Z","refreshedAt":"2024-07-31T20:12:05.89449899Z","expiresAt":"2124-07-07T20:12:05.875997607Z"},"response":{"code":"sessionOpened","data":{"sessionId":"0x001eac35765645a0edef0d80b7b8118d4c062d74a6","wallet":"0x1d3e1EDa51be0E96819cDAD4A3b1586Ea7Ffa388"}}}

void USequenceRPCManager::OpenEmailSession(const FString& CodeIn, const bool ForceCreateAccountIn, const TSuccessCallback<FCredentials_BE>& OnSuccess, const FFailureCallback& OnFailure)
{
	const TSuccessCallback<FString> OnResponse = [OnSuccess, OnFailure](const FString& Response)
	{
		const FOpenSessionResponse ParsedResponse = UIndexerSupport::JSONStringToStruct<FOpenSessionResponse>(Response);

		//we may want a better test of usability but this is a start
		if (ParsedResponse.IsValid())
		{//At this point we can construct our FCredentials_BE and return them
			
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
}

void USequenceRPCManager::OpenGuestSession(const FString& ChallengeIn, const FString& VerifierIn, const bool ForceCreateAccountIn, const TSuccessCallback<FCredentials_BE>& OnSuccess, const FFailureCallback& OnFailure)
{
}

void USequenceRPCManager::OpenPlayFabSession(const FString& SessionTicketIn, const bool ForceCreateAccountIn, const TSuccessCallback<FCredentials_BE>& OnSuccess, const FFailureCallback& OnFailure)
{
	//We read title ID from Sequence Config
}
