// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#include "Sequence/SequenceAPI.h"
#include "RequestHandler.h"
#include "Indexer/IndexerSupport.h"
#include "Dom/JsonObject.h"
#include "JsonObjectConverter.h"
#include "Bitcoin-Cryptography-Library/cpp/Keccak256.hpp"
#include "Eth/Crypto.h"
#include "Types/ContractCall.h"
#include "Misc/Base64.h"
#include "Native/NativeOAuth.h"
#include "Sequence/Close.h"

FTransaction_Sequence FTransaction_Sequence::Convert(FTransaction_FE Transaction_Fe)
{
	return FTransaction_Sequence{
		static_cast<uint64>(Transaction_Fe.chainId),
		FAddress::From(Transaction_Fe.From),
		FAddress::From(Transaction_Fe.To),
		Transaction_Fe.AutoGas == "" ? TOptional<FString>() : TOptional(Transaction_Fe.AutoGas),
		Transaction_Fe.Nonce < 0 ? TOptional<uint64>() : TOptional(static_cast<uint64>(Transaction_Fe.Nonce)),
		Transaction_Fe.Value == "" ? TOptional<FString>() : TOptional(Transaction_Fe.Value),
		Transaction_Fe.CallData == "" ? TOptional<FString>() : TOptional(Transaction_Fe.CallData),
		Transaction_Fe.TokenAddress == "" ? TOptional<FString>() : TOptional(Transaction_Fe.TokenAddress),
		Transaction_Fe.TokenAmount == "" ? TOptional<FString>() : TOptional(Transaction_Fe.TokenAmount),
		Transaction_Fe.TokenIds.Num() == 0 ? TOptional<TArray<FString>>() : TOptional(Transaction_Fe.TokenIds),
		Transaction_Fe.TokenAmounts.Num() == 0 ? TOptional<TArray<FString>>() : TOptional(Transaction_Fe.TokenAmounts),
	};
}

const FString FTransaction_Sequence::ToJson()
{
	FJsonBuilder Json = FJsonBuilder();

	Json.AddInt("chainId", ChainId);
	Json.AddString("from", "0x" + From.ToHex());
	Json.AddString("to", "0x" + To.ToHex());

	if(this->Value.IsSet()) Json.AddString("value", this->Value.GetValue());

	return Json.ToString();
}

const TransactionID FTransaction_Sequence::ID()
{
	FUnsizedData Data = StringToUTF8(ToJson());
	return GetKeccakHash(Data).ToHex();
}

FString USequenceWallet::Url(const FString Name) const
{
	return this->Hostname + this->Path + Name;
}

void USequenceWallet::SendRPC(FString Url, FString Content, TSuccessCallback<FString> OnSuccess, FFailureCallback OnFailure)
{
	NewObject<URequestHandler>()
			->PrepareRequest()
			->WithUrl(Url)
			->WithHeader("Content-type", "application/json")
			->WithHeader("Authorization", "Bearer " + this->Credentials.GetIDToken())
			->WithVerb("POST")
			->WithContentAsString(Content)
			->ProcessAndThen(OnSuccess, OnFailure);
}

USequenceWallet::USequenceWallet()
{
	this->Indexer = NewObject<UIndexer>();
}

USequenceWallet* USequenceWallet::Make(const FCredentials_BE& CredentialsIn)
{
	Ptr = NewObject<USequenceWallet>();
	Ptr->Init(CredentialsIn);
	return Ptr;
}

USequenceWallet* USequenceWallet::Make(const FCredentials_BE& CredentialsIn, const FString& ProviderURL)
{
	Ptr = NewObject<USequenceWallet>();
	Ptr->Init(CredentialsIn,ProviderURL);
	return Ptr;
}

TOptional<USequenceWallet*> USequenceWallet::Get()
{
	if(Ptr != nullptr) return Ptr;
	return TOptional<USequenceWallet*>();
}

void USequenceWallet::Init(const FCredentials_BE& CredentialsIn)
{
	this->Credentials = CredentialsIn;
	this->Indexer = NewObject<UIndexer>();
}

void USequenceWallet::Init(const FCredentials_BE& CredentialsIn,const FString& ProviderURL)
{
	this->Credentials = CredentialsIn;
	this->Indexer = NewObject<UIndexer>();
	this->ProviderUrl = ProviderURL;
}

FString USequenceWallet::GetWalletAddress()
{
	FString Addr;

	if (this->Credentials.IsRegistered())
	{
		Addr = this->Credentials.GetWalletAddress();
	}
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("[Please Login first before trying to use credentials]"));
	}
	
	return Addr;
}

void USequenceWallet::RegisterSession(const TSuccessCallback<FCredentials_BE>& OnSuccess, const FFailureCallback& OnFailure)
{
	const TSuccessCallback<FString> OnResponse = [this,OnSuccess,OnFailure](const FString& Response)
	{
#if PLATFORM_ANDROID
		NativeOAuth::AndroidLog(Response);
#endif
		const TSharedPtr<FJsonObject> Json = UIndexerSupport::JsonStringToObject(Response);
		const TSharedPtr<FJsonObject> * SessionObj = nullptr;
		const TSharedPtr<FJsonObject> * ResponseObj = nullptr;
		
		UE_LOG(LogTemp,Display,TEXT("Pre Processing, Response: %s"),*Response);
		if (Json.Get()->TryGetObjectField("session",SessionObj) && Json.Get()->TryGetObjectField("response",ResponseObj))
		{
			//we can createdAt, issuedAt, refreshedAt, userId
			const TSharedPtr<FJsonObject> * DataObj = nullptr;
			const TSharedPtr<FJsonObject> * IdentityObj = nullptr;

			FString CreatedAt = "", RefreshedAt = "", ExpiresAt = "", UserId = "";
			if (ResponseObj->Get()->TryGetObjectField("data", DataObj) && SessionObj->Get()->TryGetObjectField("identity", IdentityObj) && SessionObj->Get()->TryGetStringField("userId",UserId) && SessionObj->Get()->TryGetStringField("createdAt",CreatedAt) && SessionObj->Get()->TryGetStringField("refreshedAt", RefreshedAt) && SessionObj->Get()->TryGetStringField("expiresAt", ExpiresAt))
			{
				FString Wallet = "", Type = "", ISS = "", Sub = "", Email = "";
				if (IdentityObj->Get()->TryGetStringField("email", Email) && IdentityObj->Get()->TryGetStringField("sub", Sub) && IdentityObj->Get()->TryGetStringField("iss",ISS) && IdentityObj->Get()->TryGetStringField("type",Type) && DataObj->Get()->TryGetStringField("wallet",Wallet))
				{
					FDateTime ParsedCreatedAt, ParsedRefreshedAt, ParsedExpiresAt;
					FDateTime::ParseIso8601(*CreatedAt,ParsedCreatedAt);
					FDateTime::ParseIso8601(*RefreshedAt,ParsedRefreshedAt);
					FDateTime::ParseIso8601(*ExpiresAt,ParsedExpiresAt);
					const int64 UnixIssuedAt = ParsedCreatedAt.ToUnixTimestamp();
					const int64 UnixRefreshedAt = ParsedRefreshedAt.ToUnixTimestamp();
					const int64 UnixExpiresAt = ParsedExpiresAt.ToUnixTimestamp();
					this->Credentials.RegisterCredentials(Wallet,Email,ISS,Type,Sub,UserId,UnixIssuedAt,UnixRefreshedAt,UnixExpiresAt);
					const UAuthenticator * TUAuth = NewObject<UAuthenticator>();
					TUAuth->StoreCredentials(this->Credentials);
					OnSuccess(this->Credentials);
				}
				else
				{
					OnFailure(FSequenceError(RequestFail, "3rd Level parsing: Request failed: " + Response));
				}
			}
			else
			{
				OnFailure(FSequenceError(RequestFail, "2nd Level parsing: Request failed: " + Response));
			}
		}
		else
		{
			OnFailure(FSequenceError(RequestFail, "1st Level parsing: Request failed: " + Response));
		}
	};
	
	if (this->Credentials.UnRegisteredValid())
	{
		const FString URL = this->Credentials.GetRPCServer() + "/rpc/WaasAuthenticator/RegisterSession";
		this->SequenceRPC(URL,this->BuildRegisterSessionIntent(),OnResponse,OnFailure);
	}
	else
	{
		OnFailure(FSequenceError(RequestFail, "[Invalid Credentials please login first]"));
	}
}

void USequenceWallet::ListSessions(const TSuccessCallback<TArray<FSession>>& OnSuccess, const FFailureCallback& OnFailure)
{
	const TSuccessCallback<FString> OnResponse = [this,OnSuccess,OnFailure](const FString& Response)
	{
#if PLATFORM_ANDROID
		NativeOAuth::AndroidLog(Response);
#endif
		TArray<FSession> Sessions;
		const TSharedPtr<FJsonObject> Json = UIndexerSupport::JsonStringToObject(Response);
		FListSessionsResponseObj ResponseStruct;
		if (FJsonObjectConverter::JsonObjectToUStruct<FListSessionsResponseObj>(Json.ToSharedRef(), &ResponseStruct))
		{
			const FString ParsedResponse = UIndexerSupport::StructToString(ResponseStruct);
			UE_LOG(LogTemp,Display,TEXT("Response: %s"), *ParsedResponse);
			OnSuccess(ResponseStruct.response.data);
		}
		else
		{
			OnFailure(FSequenceError(RequestFail, "2nd Level Parsing: Request failed: " + Response));
		}
	};
	
	if (this->Credentials.RegisteredValid())
	{
		const FString URL = this->Credentials.GetRPCServer() + "/rpc/WaasAuthenticator/SendIntent";
		this->SequenceRPC(URL,this->BuildListSessionIntent(),OnResponse,OnFailure);
	}
	else
	{
		OnFailure(FSequenceError(RequestFail, "[Session Not Registered Please Register Session First]"));
	}
}

void USequenceWallet::SignOut()
{
	const UAuthenticator * auth = NewObject<UAuthenticator>();
	if (this->Credentials.IsRegistered())
	{
		const TFunction<void (FString)> OnSuccess = [this,auth](FString State)
		{
			auth->ClearStoredCredentials();
		};

		const TFunction<void (FSequenceError)> OnFailure = [this,auth](FSequenceError Err)
		{
			auth->ClearStoredCredentials();
		};
	
		this->CloseSession(OnSuccess, OnFailure);
	}
	else
	{
		auth->ClearStoredCredentials();
	}
}

void USequenceWallet::CloseSession(const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure)
{
	const TSuccessCallback<FString> OnResponse = [this,OnSuccess,OnFailure](const FString& Response)
	{
#if PLATFORM_ANDROID
		NativeOAuth::AndroidLog(Response);
#endif
		UE_LOG(LogTemp,Display,TEXT("Response: %s"), *Response);
		const TSharedPtr<FJsonObject> Json = UIndexerSupport::JsonStringToObject(Response);
		FCloseResponseObj ResponseStruct;
		if (FJsonObjectConverter::JsonObjectToUStruct<FCloseResponseObj>(Json.ToSharedRef(), &ResponseStruct))
		{
			const FString ParsedResponse = UIndexerSupport::StructToString(ResponseStruct);
			UE_LOG(LogTemp,Display,TEXT("Response: %s"), *ParsedResponse);
			if (ResponseStruct.response.code.Compare("sessionClosed")==0)
			{
				this->Credentials.UnRegisterCredentials();
				const UAuthenticator * TUAuth = NewObject<UAuthenticator>();
				TUAuth->StoreCredentials(this->Credentials);
				OnSuccess(ResponseStruct.response.code);
			}
			else
			{
				OnFailure(FSequenceError(RequestFail, "2nd Level Parsing: Request failed: " + Response));
			}
		}
		else
		{
			OnFailure(FSequenceError(RequestFail, "2nd Level Parsing: Request failed: " + Response));
		}
	};
	if (this->Credentials.RegisteredValid())
	{
		const FString URL = this->Credentials.GetRPCServer() + "/rpc/WaasAuthenticator/SendIntent";
		this->SequenceRPC(URL,this->BuildCloseSessionIntent(),OnResponse,OnFailure);
	}
	else
	{
		OnFailure(FSequenceError(RequestFail, "[Session Not Registered Please Register Session First]"));
	}
}

FString USequenceWallet::GeneratePacketSignature(const FString& Packet) const
{
	//keccakhash of the packet first
	const FHash256 SigningHash = FHash256::New();
	const FUnsizedData EncodedSigningData = StringToUTF8(Packet);
	Keccak256::getHash(EncodedSigningData.Arr.Get()->GetData(), EncodedSigningData.GetLength(), SigningHash.Ptr());
	TArray<uint8> SigningBytes;
	SigningBytes.Append(SigningHash.Ptr(),SigningHash.GetLength());
	const FString Signature = "0x" + this->Credentials.SignMessageWithSessionWallet(SigningBytes,32);
	return Signature;
}

int64 USequenceWallet::GetNetworkId()
{
	return this->Credentials.GetNetwork();
}

void USequenceWallet::UpdateNetworkId(int64 NewNetwork)
{
	this->Credentials.UpdateNetwork(NewNetwork);
	const UAuthenticator * auth = NewObject<UAuthenticator>();
	auth->StoreCredentials(this->Credentials);
}

void USequenceWallet::UpdateProviderURL(const FString& Url)
{
	this->ProviderUrl = Url;
}

void USequenceWallet::SignMessage(const FString& Message, const TSuccessCallback<FSignedMessage>& OnSuccess, const FFailureCallback& OnFailure)
{
	const TSuccessCallback<FString> OnResponse = [this,OnSuccess,OnFailure](const FString& Response)
	{
#if PLATFORM_ANDROID
		NativeOAuth::AndroidLog(Response);
#endif
		const TSharedPtr<FJsonObject> Json = UIndexerSupport::JsonStringToObject(Response);
		FSignedMessageResponseObj Msg;
		if (FJsonObjectConverter::JsonObjectToUStruct<FSignedMessageResponseObj>(Json.ToSharedRef(), &Msg))
		{
			const FString ParsedResponse = UIndexerSupport::StructToString(Msg);
			UE_LOG(LogTemp,Display,TEXT("Response: %s"), *ParsedResponse);
			OnSuccess(Msg.response);
		}
		else
		{
			OnFailure(FSequenceError(RequestFail, "Malformed Response: " + Response));
		}
		
	};
	if (this->Credentials.RegisteredValid())
	{
		const FString URL = this->Credentials.GetRPCServer() + "/rpc/WaasAuthenticator/SendIntent";
		this->SequenceRPC(URL,this->BuildSignMessageIntent(Message),OnResponse,OnFailure);
	}
	else
	{
		OnFailure(FSequenceError(RequestFail, "[Session Not Registered Please Register Session First]"));
	}
}

void USequenceWallet::SendTransaction(TArray<TUnion<FRawTransaction, FERC20Transaction, FERC721Transaction, FERC1155Transaction>> Transactions, TSuccessCallback<FTransactionResponse> OnSuccess, FFailureCallback OnFailure)
{
	FString TransactionsPayload = "[";
	
	for (TUnion<FRawTransaction, FERC20Transaction, FERC721Transaction, FERC1155Transaction> Transaction : Transactions)
	{
		switch(Transaction.GetCurrentSubtypeIndex())
		{
		case 0: //RawTransaction
			TransactionsPayload += Transaction.GetSubtype<FRawTransaction>().GetJsonString() + ",";
			break;
		case 1: //ERC20
			TransactionsPayload += Transaction.GetSubtype<FERC20Transaction>().GetJsonString() + ",";
			break;
		case 2: //ERC721
			TransactionsPayload += Transaction.GetSubtype<FERC721Transaction>().GetJsonString() + ",";
			break;
		case 3: //ERC1155
			TransactionsPayload += Transaction.GetSubtype<FERC1155Transaction>().GetJsonString() + ",";
			break;
		default: //Doesn't match
			break;
		}
	}
	TransactionsPayload.RemoveAt(TransactionsPayload.Len() - 1);
	TransactionsPayload += "]";
	TSuccessCallback<FString> OnResponse = [=](FString Response)
	{
#if PLATFORM_ANDROID
		NativeOAuth::AndroidLog(Response);
#endif
		TSharedPtr<FJsonObject> jsonObj;
		if(FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Response), jsonObj))
		{
			//parse out what we want and wrap it nicely for ease of use
			UE_LOG(LogTemp,Display,TEXT("Response: %s"), *Response);
			const TSharedPtr<FJsonObject> * ResponseObj = nullptr;
			if (jsonObj->TryGetObjectField("response",ResponseObj))
			{
				const TSharedPtr<FJsonObject> * DataObj = nullptr;
				FString Code = "";
				if (ResponseObj->Get()->TryGetObjectField("data",DataObj) && ResponseObj->Get()->TryGetStringField("code",Code))
				{
					FString TxHash = "";
					FString MetaTxHash = "";

					const TSharedPtr<FJsonObject> * NativeReceiptObj = nullptr;
					const TSharedPtr<FJsonObject> * ReceiptObj = nullptr;
					const TSharedPtr<FJsonObject> * RequestObj = nullptr;
					const TArray<TSharedPtr<FJsonValue>> * SimulationsObj = nullptr;
					
					
					if (DataObj->Get()->TryGetStringField("txHash",TxHash) &&
						DataObj->Get()->TryGetStringField("metaTxHash",MetaTxHash) &&
						DataObj->Get()->TryGetObjectField("nativeReceipt",NativeReceiptObj) &&
						DataObj->Get()->TryGetObjectField("receipt",ReceiptObj) &&
						DataObj->Get()->TryGetObjectField("request",RequestObj) &&
						DataObj->Get()->TryGetArrayField("simulations", SimulationsObj))
					{						
						const FTransactionResponse TxnResponse(Code,TxHash,MetaTxHash,*RequestObj,*NativeReceiptObj,*ReceiptObj,*SimulationsObj,jsonObj);
						OnSuccess(TxnResponse);
					}
					else
					{
						UE_LOG(LogTemp,Error,TEXT("Error in 3rd level parsing in Transaction"));
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
	
	if (this->Credentials.RegisteredValid())
	{
		const FString URL = this->Credentials.GetRPCServer() + "/rpc/WaasAuthenticator/SendIntent";
		this->SequenceRPC(URL, BuildSendTransactionIntent(TransactionsPayload), OnResponse, OnFailure);
	}
	else
	{
		OnFailure(FSequenceError(RequestFail, "[Session Not Registered Please Register Session First]"));
	}
}

FString USequenceWallet::BuildSignMessageIntent(const FString& Message)
{
	const int64 issued = FDateTime::UtcNow().ToUnixTimestamp() - 30;
	const int64 expires = issued + 86400;
	const FString issuedString = FString::Printf(TEXT("%lld"),issued);
	const FString expiresString = FString::Printf(TEXT("%lld"),expires);
	
	const FString Wallet = this->Credentials.GetWalletAddress();
	//eip-191 and keccak hashing the message
	const FString LeadingByte = "\x19";//leading byte
	FString Payload = LeadingByte + "Ethereum Signed Message:\n";
	Payload.AppendInt(Message.Len());
	Payload += Message;
	const FUnsizedData PayloadBytes = StringToUTF8(Payload);
	const FString EIP_Message = "0x" + BytesToHex(PayloadBytes.Ptr(),PayloadBytes.GetLength());
	UE_LOG(LogTemp,Display,TEXT("EIP_191: %s"),*EIP_Message);
	
	//EIP-191
	const FString Data = "{\"message\":\""+EIP_Message+"\",\"network\":\""+this->Credentials.GetNetworkString()+"\",\"wallet\":\""+this->Credentials.GetWalletAddress()+"\"}";
	const FString SigIntent = "{\"data\":"+Data+",\"expiresAt\":"+expiresString+",\"issuedAt\":"+issuedString+",\"name\":\"signMessage\",\"version\":\""+this->Credentials.GetWaasVersion()+"\"}";
	const FString Signature = this->GeneratePacketSignature(SigIntent);
	const FString Intent = "{\"intent\":{\"data\":"+Data+",\"expiresAt\":"+expiresString+",\"issuedAt\":"+issuedString+",\"name\":\"signMessage\",\"signatures\":[{\"sessionId\":\""+this->Credentials.GetSessionId()+"\",\"signature\":\""+Signature+"\"}],\"version\":\""+this->Credentials.GetWaasVersion()+"\"}}";
	UE_LOG(LogTemp,Display,TEXT("SignMessage Intent: %s"),*Intent);
	return Intent;
}

FString USequenceWallet::BuildSendTransactionIntent(const FString& Txns)
{
	const int64 issued = FDateTime::UtcNow().ToUnixTimestamp() - 30;
	const int64 expires = issued + 86400;
	const FString issuedString = FString::Printf(TEXT("%lld"),issued);
	const FString expiresString = FString::Printf(TEXT("%lld"),expires);
	const FString identifier = "unreal-sdk-" + FDateTime::UtcNow().ToString() + "-" + this->Credentials.GetWalletAddress();
	const FString SigIntent = "{\"data\":{\"identifier\":\""+identifier+"\",\"network\":\""+this->Credentials.GetNetworkString()+"\",\"transactions\":"+Txns+",\"wallet\":\""+this->Credentials.GetWalletAddress()+"\"},\"expiresAt\":"+expiresString+",\"issuedAt\":"+issuedString+",\"name\":\"sendTransaction\",\"version\":\""+this->Credentials.GetWaasVersion()+"\"}";
	const FString Signature = this->GeneratePacketSignature(SigIntent);
	FString Intent = "{\"intent\":{\"data\":{\"identifier\":\""+identifier+"\",\"network\":\""+this->Credentials.GetNetworkString()+"\",\"transactions\":"+Txns+",\"wallet\":\""+this->Credentials.GetWalletAddress()+"\"},\"expiresAt\":"+expiresString+",\"issuedAt\":"+issuedString+",\"name\":\"sendTransaction\",\"signatures\":[{\"sessionId\":\""+this->Credentials.GetSessionId()+"\",\"signature\":\""+Signature+"\"}],\"version\":\""+this->Credentials.GetWaasVersion()+"\"}}";
	UE_LOG(LogTemp,Display,TEXT("SendTransactionIntent: %s"),*Intent);
	return Intent;
}

FString USequenceWallet::BuildRegisterSessionIntent()
{
	const int64 issued = FDateTime::UtcNow().ToUnixTimestamp() - 30;
	const int64 expires = issued + 86400;
	const FString issuedString = FString::Printf(TEXT("%lld"),issued);
	const FString expiresString = FString::Printf(TEXT("%lld"),expires);
	const FString GUID = FGuid::NewGuid().ToString();
	const FString Data = "{\"idToken\":\""+this->Credentials.GetIDToken()+"\",\"sessionId\":\""+this->Credentials.GetSessionId()+"\"}";
	const FString SigIntent = "{\"data\":"+Data+",\"expiresAt\":"+expiresString+",\"issuedAt\":"+issuedString+",\"name\":\"openSession\",\"version\":\""+this->Credentials.GetWaasVersion()+"\"}";
	const FString Signature = this->GeneratePacketSignature(SigIntent);
	const FString Intent = "{\"intent\":{\"data\":"+Data+",\"expiresAt\":"+expiresString+",\"issuedAt\":"+issuedString+",\"name\":\"openSession\",\"signatures\":[{\"sessionId\":\""+this->Credentials.GetSessionId()+"\",\"signature\":\""+Signature+"\"}],\"version\":\""+this->Credentials.GetWaasVersion()+"\"},\"friendlyName\":\""+GUID+"\"}";
	UE_LOG(LogTemp,Display,TEXT("RegisterSession Intent: %s"),*Intent);
	return Intent;
}

FString USequenceWallet::BuildListSessionIntent()
{
	const int64 issued = FDateTime::UtcNow().ToUnixTimestamp() - 30;
	const int64 expires = issued + 86400;
	const FString issuedString = FString::Printf(TEXT("%lld"),issued);
	const FString expiresString = FString::Printf(TEXT("%lld"),expires);
	const FString SigIntent = "{\"data\":{\"wallet\":\""+this->Credentials.GetWalletAddress()+"\"},\"expiresAt\":"+expiresString+",\"issuedAt\":"+issuedString+",\"name\":\"listSessions\",\"version\":\""+this->Credentials.GetWaasVersion()+"\"}";
	const FString Signature = this->GeneratePacketSignature(SigIntent);
	const FString Intent = "{\"intent\":{\"data\":{\"wallet\":\""+this->Credentials.GetWalletAddress()+"\"},\"expiresAt\":"+expiresString+",\"issuedAt\":"+issuedString+",\"name\":\"listSessions\",\"signatures\":[{\"sessionId\":\""+this->Credentials.GetSessionId()+"\",\"signature\":\""+Signature+"\"}],\"version\":\""+this->Credentials.GetWaasVersion()+"\"}}";
	UE_LOG(LogTemp,Display,TEXT("ListSessionIntent: %s"),*Intent);
	return Intent;
}

FString USequenceWallet::BuildCloseSessionIntent()
{
	const int64 issued = FDateTime::UtcNow().ToUnixTimestamp() - 30;
	const int64 expires = issued + 86400;
	const FString issuedString = FString::Printf(TEXT("%lld"),issued);
	const FString expiresString = FString::Printf(TEXT("%lld"),expires);
	const FString SigIntent = "{\"data\":{\"sessionId\":\""+this->Credentials.GetSessionId()+"\"},\"expiresAt\":"+expiresString+",\"issuedAt\":"+issuedString+",\"name\":\"closeSession\",\"version\":\""+this->Credentials.GetWaasVersion()+"\"}";
	const FString Signature = this->GeneratePacketSignature(SigIntent);
	const FString Intent = "{\"intent\":{\"data\":{\"sessionId\":\""+this->Credentials.GetSessionId()+"\"},\"expiresAt\":"+expiresString+",\"issuedAt\":"+issuedString+",\"name\":\"closeSession\",\"signatures\":[{\"sessionId\":\""+this->Credentials.GetSessionId()+"\",\"signature\":\""+Signature+"\"}],\"version\":\""+this->Credentials.GetWaasVersion()+"\"}}";
	UE_LOG(LogTemp,Display,TEXT("CloseSessionIntent: %s"),*Intent);
	return Intent;
}

FString USequenceWallet::BuildSessionValidationIntent()
{
	const FString Intent = "{\\\"sessionId\\\":\\\""+this->Credentials.GetSessionId()+"\\\"}";
	UE_LOG(LogTemp,Display,TEXT("SessionValidationIntent: %s"),*Intent);
	return Intent;
}

template <typename T> void USequenceWallet::SequenceRPC(FString Url, FString Content, TSuccessCallback<T> OnSuccess, FFailureCallback OnFailure)
{
	UE_LOG(LogTemp,Display,TEXT("Dest: %s"),*Url);
	NewObject<URequestHandler>()
	->PrepareRequest()
	->WithUrl(Url)
	->WithHeader("Content-type", "application/json")
	->WithHeader("Accept", "application/json")
	->WithHeader("X-Access-Key", Credentials.GetProjectAccessKey())
	->WithVerb("POST")
	->WithContentAsString(Content)
	->ProcessAndThen(OnSuccess, OnFailure);
}

FString USequenceWallet::getSequenceURL(FString endpoint)
{
	return this->SequenceURL + endpoint;
}

TArray<FContact_BE> USequenceWallet::BuildFriendListFromJson(FString JSON)
{
	TArray<FContact_BE> friendList;
	TSharedPtr<FJsonObject> jsonObj;

	if (FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(JSON), jsonObj))
	{
		const TArray<TSharedPtr<FJsonValue>>* storedFriends;
		if (jsonObj.Get()->TryGetArrayField("friends", storedFriends))
		{
			for (TSharedPtr<FJsonValue> friendData : *storedFriends)
			{
				const TSharedPtr<FJsonObject>* fJsonObj;
				if (friendData.Get()->TryGetObject(fJsonObj))//need it as an object
				{
					FContact_BE newFriend;
					newFriend.Public_Address = fJsonObj->Get()->GetStringField("userAddress");
					newFriend.Nickname = fJsonObj->Get()->GetStringField("nickname");
					friendList.Add(newFriend);
				}
			}
		}
	}
	else
	{//failure
		UE_LOG(LogTemp, Error, TEXT("Failed to convert String: %s to Json object"), *JSON);
	}
	return friendList;
}

/*
* Gets the friend data from the given username!
* This function appears to require some form of authentication (perhaps all of the sequence api does)
* @Deprecated
*/
void USequenceWallet::GetFriends(FString username, TSuccessCallback<TArray<FContact_BE>> OnSuccess, FFailureCallback OnFailure)
{
	FString json_arg = "{}";
	
	SendRPC(getSequenceURL("friendList"), json_arg, [this,OnSuccess](FString Content)
		{
			OnSuccess(this->BuildFriendListFromJson(Content));
		}, OnFailure);
}

TArray<FItemPrice_BE> USequenceWallet::BuildItemUpdateListFromJson(FString JSON)
{
	UE_LOG(LogTemp, Error, TEXT("Received json: %s"), *JSON);
	TSharedPtr<FJsonObject> jsonObj;
	FUpdatedPriceReturn updatedPrices;

	if (FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(JSON), jsonObj))
	{
		if (FJsonObjectConverter::JsonObjectToUStruct<FUpdatedPriceReturn>(jsonObj.ToSharedRef(), &updatedPrices))
		{
			return updatedPrices.tokenPrices;
		}
	}
	else
	{//failure
		UE_LOG(LogTemp, Error, TEXT("Failed to convert String: %s to Json object"), *JSON);
	}
	TArray<FItemPrice_BE> updatedItems;
	return updatedItems;
}

void USequenceWallet::GetUpdatedCoinPrice(FID_BE ItemToUpdate, TSuccessCallback<TArray<FItemPrice_BE>> OnSuccess, FFailureCallback OnFailure)
{
	TArray<FID_BE> items;
	items.Add(ItemToUpdate);
	GetUpdatedCoinPrices(items, OnSuccess, OnFailure);
}

void USequenceWallet::GetUpdatedCoinPrices(TArray<FID_BE> ItemsToUpdate, TSuccessCallback<TArray<FItemPrice_BE>> OnSuccess, FFailureCallback OnFailure)
{
	FString args = "{\"tokens\":";
	FString jsonObjString = "";
	TArray<FString> parsedItems;
	for (FID_BE item : ItemsToUpdate)
	{
		if (FJsonObjectConverter::UStructToJsonObjectString<FID_BE>(item, jsonObjString))
			parsedItems.Add(jsonObjString);
	}
	args += UIndexerSupport::StringListToSimpleString(parsedItems);
	args += "}";

	SendRPC(getSequenceURL("getCoinPrices"), args, [this,OnSuccess](FString Content)
		{
			OnSuccess(this->BuildItemUpdateListFromJson(Content));
		}, OnFailure);
}

void USequenceWallet::GetUpdatedCollectiblePrice(FID_BE ItemToUpdate, TSuccessCallback<TArray<FItemPrice_BE>> OnSuccess, FFailureCallback OnFailure)
{
	TArray<FID_BE> items;
	items.Add(ItemToUpdate);
	GetUpdatedCollectiblePrices(items, OnSuccess, OnFailure);
}

void USequenceWallet::GetUpdatedCollectiblePrices(TArray<FID_BE> ItemsToUpdate, TSuccessCallback<TArray<FItemPrice_BE>> OnSuccess, FFailureCallback OnFailure)
{
	FString args = "{\"tokens\":";
	FString jsonObjString = "";
	TArray<FString> parsedItems;
	for (FID_BE item : ItemsToUpdate)
	{
		if (FJsonObjectConverter::UStructToJsonObjectString<FID_BE>(item, jsonObjString))
			parsedItems.Add(jsonObjString);
	}
	args += UIndexerSupport::StringListToSimpleString(parsedItems);
	args += "}";

	SendRPC(getSequenceURL("getCollectiblePrices"), args, [this,OnSuccess](FString Content)
		{
			OnSuccess(this->BuildItemUpdateListFromJson(Content));
		}, OnFailure);
}

FString USequenceWallet::BuildQr_Request_URL(FString walletAddress,int32 Size)
{
	FString urlSize = "/";
	urlSize.AppendInt(Size);

	return SequenceURL_Qr + encodeB64_URL(walletAddress) + urlSize;
}

//we only need to encode base64URL we don't decode them as we receive the QR code
FString USequenceWallet::encodeB64_URL(FString data)
{
	FString ret;
	UE_LOG(LogTemp, Display, TEXT("Pre encoded addr: [%s]"), *data);
	ret = FBase64::Encode(data);
	UE_LOG(LogTemp, Display, TEXT("Post encoded addr: [%s]"), *ret);
	//now we just gotta do some swaps to make it base64 URL compliant
	// + -> -
	// / -> _ 

	FString srch_plus = TEXT("+");
	FString rep_plus = TEXT("-");
	FString srch_slash = TEXT("/");
	FString rep_slash = TEXT("_");

	const TCHAR* srch_ptr_plus = *srch_plus;
	const TCHAR* rep_ptr_plus = *rep_plus;
	const TCHAR* srch_ptr_slash = *srch_slash;
	const TCHAR* rep_ptr_slash = *rep_slash;

	ret.ReplaceInline(srch_ptr_plus, rep_ptr_plus, ESearchCase::IgnoreCase);//remove + and replace with -
	ret.ReplaceInline(srch_ptr_slash, rep_ptr_slash, ESearchCase::IgnoreCase);//remove / and replace with _

	UE_LOG(LogTemp, Display, TEXT("B64-URL encoded addr: [%s]"), *ret);

	return ret;
}

//Indexer Calls

void USequenceWallet::Ping(TSuccessCallback<bool> OnSuccess, FFailureCallback OnFailure)
{
	if (this->Indexer)
		this->Indexer->Ping(this->Credentials.GetNetwork(), OnSuccess, OnFailure);
}

void USequenceWallet::Version(TSuccessCallback<FVersion> OnSuccess, FFailureCallback OnFailure)
{
	if (this->Indexer)
		this->Indexer->Version(this->Credentials.GetNetwork(),OnSuccess,OnFailure);
}

void USequenceWallet::RunTimeStatus(TSuccessCallback<FRuntimeStatus> OnSuccess, FFailureCallback OnFailure)
{
	if (this->Indexer)
		this->Indexer->RunTimeStatus(this->Credentials.GetNetwork(), OnSuccess, OnFailure);
}

void USequenceWallet::GetChainID(TSuccessCallback<int64> OnSuccess, FFailureCallback OnFailure)
{
	if (this->Indexer)
		this->Indexer->GetChainID(this->Credentials.GetNetwork(), OnSuccess, OnFailure);
}

void USequenceWallet::GetEtherBalance(FString AccountAddr, TSuccessCallback<FEtherBalance> OnSuccess, FFailureCallback OnFailure)
{
	if (this->Indexer)
		this->Indexer->GetEtherBalance(this->Credentials.GetNetwork(), AccountAddr, OnSuccess, OnFailure);
}

void USequenceWallet::GetTokenBalances(FGetTokenBalancesArgs Args, TSuccessCallback<FGetTokenBalancesReturn> OnSuccess, FFailureCallback OnFailure)
{
	if (this->Indexer)
		this->Indexer->GetTokenBalances(this->Credentials.GetNetwork(), Args, OnSuccess, OnFailure);
}

void USequenceWallet::GetTokenSupplies(FGetTokenSuppliesArgs Args, TSuccessCallback<FGetTokenSuppliesReturn> OnSuccess, FFailureCallback OnFailure)
{
	if (this->Indexer)
		this->Indexer->GetTokenSupplies(this->Credentials.GetNetwork(), Args, OnSuccess, OnFailure);
}

void USequenceWallet::GetTokenSuppliesMap(FGetTokenSuppliesMapArgs Args, TSuccessCallback<FGetTokenSuppliesMapReturn> OnSuccess, FFailureCallback OnFailure)
{
	if (this->Indexer)
		this->Indexer->GetTokenSuppliesMap(this->Credentials.GetNetwork(), Args, OnSuccess, OnFailure);
}

void USequenceWallet::GetBalanceUpdates(FGetBalanceUpdatesArgs Args, TSuccessCallback<FGetBalanceUpdatesReturn> OnSuccess, FFailureCallback OnFailure)
{
	if (this->Indexer)
		this->Indexer->GetBalanceUpdates(this->Credentials.GetNetwork(), Args, OnSuccess, OnFailure);
}

void USequenceWallet::GetTransactionHistory(FGetTransactionHistoryArgs Args, TSuccessCallback<FGetTransactionHistoryReturn> OnSuccess, FFailureCallback OnFailure)
{
	if (this->Indexer)
		this->Indexer->GetTransactionHistory(this->Credentials.GetNetwork(), Args, OnSuccess, OnFailure);
}

void USequenceWallet::BlockByNumber(uint64 Number, TSuccessCallback<TSharedPtr<FJsonObject>> OnSuccess, FFailureCallback OnFailure)
{
	Provider(this->ProviderUrl).BlockByNumber(Number, OnSuccess, OnFailure);
}

void USequenceWallet::BlockByNumber(EBlockTag Tag, TSuccessCallback<TSharedPtr<FJsonObject>> OnSuccess, FFailureCallback OnFailure)
{
	Provider(this->ProviderUrl).BlockByNumber(Tag,OnSuccess,OnFailure);
}

void USequenceWallet::BlockByHash(FHash256 Hash, TSuccessCallback<TSharedPtr<FJsonObject>> OnSuccess, FFailureCallback OnFailure)
{
	Provider(this->ProviderUrl).BlockByHash(Hash, OnSuccess, OnFailure);
}

void USequenceWallet::BlockNumber(TSuccessCallback<uint64> OnSuccess, FFailureCallback OnFailure)
{
	Provider(this->ProviderUrl).BlockNumber(OnSuccess, OnFailure);
}

void USequenceWallet::HeaderByNumber(uint64 Id, TSuccessCallback<FHeader> OnSuccess, FFailureCallback OnFailure)
{
	Provider(this->ProviderUrl).HeaderByNumber(Id, OnSuccess, OnFailure);
}

void USequenceWallet::HeaderByNumber(EBlockTag Tag, TSuccessCallback<FHeader> OnSuccess, FFailureCallback OnFailure)
{
	Provider(this->ProviderUrl).HeaderByNumber(Tag, OnSuccess, OnFailure);
}

void USequenceWallet::HeaderByHash(FHash256 Hash, TSuccessCallback<FHeader> OnSuccess, FFailureCallback OnFailure)
{
	Provider(this->ProviderUrl).HeaderByHash(Hash,OnSuccess,OnFailure);
}

void USequenceWallet::TransactionByHash(FHash256 Hash, TSuccessCallback<TSharedPtr<FJsonObject>> OnSuccess, FFailureCallback OnFailure)
{
	Provider(this->ProviderUrl).TransactionByHash(Hash,OnSuccess,OnFailure);
}

void USequenceWallet::TransactionCount(FAddress Addr, uint64 Number, TSuccessCallback<uint64> OnSuccess, FFailureCallback OnFailure)
{
	Provider(this->ProviderUrl).TransactionCount(Addr,Number,OnSuccess,OnFailure);
}

void USequenceWallet::TransactionCount(FAddress Addr, EBlockTag Tag, TSuccessCallback<uint64> OnSuccess, FFailureCallback OnFailure)
{
	Provider(this->ProviderUrl).TransactionCount(Addr,Tag,OnSuccess,OnFailure);
}

void USequenceWallet::TransactionReceipt(FHash256 Hash, TSuccessCallback<FTransactionReceipt> OnSuccess, FFailureCallback OnFailure)
{
	Provider(this->ProviderUrl).TransactionReceipt(Hash,OnSuccess,OnFailure);
}

void USequenceWallet::GetGasPrice(TSuccessCallback<FUnsizedData> OnSuccess, FFailureCallback OnFailure)
{
	Provider(this->ProviderUrl).GetGasPrice(OnSuccess, OnFailure);
}

void USequenceWallet::EstimateContractCallGas(FContractCall ContractCall, TSuccessCallback<FUnsizedData> OnSuccess, FFailureCallback OnFailure)
{
	Provider(this->ProviderUrl).EstimateContractCallGas(ContractCall,OnSuccess,OnFailure);
}

void USequenceWallet::EstimateDeploymentGas(FAddress From, FString Bytecode, TSuccessCallback<FUnsizedData> OnSuccess, FFailureCallback OnFailure)
{
	Provider(this->ProviderUrl).EstimateDeploymentGas(From,Bytecode,OnSuccess,OnFailure);
}

void USequenceWallet::DeployContract(FString Bytecode, FPrivateKey PrivKey, int64 ChainId, TSuccessCallback<FAddress> OnSuccess, FFailureCallback OnFailure)
{
	Provider(this->ProviderUrl).DeployContract(Bytecode, PrivKey, ChainId, OnSuccess, OnFailure);
}

void USequenceWallet::DeployContractWithHash(FString Bytecode, FPrivateKey PrivKey, int64 ChainId, TSuccessCallbackTuple<FAddress, FUnsizedData> OnSuccess, FFailureCallback OnFailure)
{
	FUnsizedData test = StringToUTF8("converter");
	Provider(this->ProviderUrl).DeployContractWithHash(Bytecode,PrivKey,ChainId,OnSuccess,OnFailure);
}

void USequenceWallet::NonceAt(uint64 Number, TSuccessCallback<FBlockNonce> OnSuccess, FFailureCallback OnFailure)
{
	Provider(this->ProviderUrl).NonceAt(Number,OnSuccess,OnFailure);
}

void USequenceWallet::NonceAt(EBlockTag Tag, TSuccessCallback<FBlockNonce> OnSuccess, FFailureCallback OnFailure)
{
	Provider(this->ProviderUrl).NonceAt(Tag,OnSuccess,OnFailure);
}

void USequenceWallet::SendRawTransaction(FString Data, TSuccessCallback<FUnsizedData> OnSuccess, FFailureCallback OnFailure)
{
	Provider(this->ProviderUrl).SendRawTransaction(Data,OnSuccess,OnFailure);
}

void USequenceWallet::ChainId(TSuccessCallback<uint64> OnSuccess, FFailureCallback OnFailure)
{
	Provider(this->ProviderUrl).ChainId(OnSuccess,OnFailure);
}

void USequenceWallet::Call(FContractCall ContractCall, uint64 Number, TSuccessCallback<FUnsizedData> OnSuccess, FFailureCallback OnFailure)
{
	Provider(this->ProviderUrl).Call(ContractCall,Number,OnSuccess,OnFailure);
}

void USequenceWallet::Call(FContractCall ContractCall, EBlockTag Number, TSuccessCallback<FUnsizedData> OnSuccess, FFailureCallback OnFailure)
{
	Provider(this->ProviderUrl).Call(ContractCall,Number,OnSuccess,OnFailure);
}

void USequenceWallet::NonViewCall(FEthTransaction Transaction, FPrivateKey PrivateKey, int ChainID, TSuccessCallback<FUnsizedData> OnSuccess, FFailureCallback OnFailure)
{
	Provider(this->ProviderUrl).NonViewCall(Transaction, PrivateKey, ChainID, OnSuccess, OnFailure);
}