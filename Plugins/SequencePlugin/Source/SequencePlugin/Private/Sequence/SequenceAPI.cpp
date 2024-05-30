// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#include "Sequence/SequenceAPI.h"
#include "RequestHandler.h"
#include "Indexer/IndexerSupport.h"
#include "Dom/JsonObject.h"
#include "JsonObjectConverter.h"
#include "Bitcoin-Cryptography-Library/cpp/Keccak256.hpp"
#include "Eth/Crypto.h"
#include "Kismet/GameplayStatics.h"
#include "Types/ContractCall.h"
#include "Misc/Base64.h"
#include "Native/NativeOAuth.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Sequence/Close.h"
#include "Indexer/Indexer.h"
#include "Util/JsonBuilder.h"
#include "Provider.h"

FTransaction_Sequence FTransaction_Sequence::Convert(const FTransaction_FE& Transaction_Fe)
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

FString FTransaction_Sequence::ToJson()
{
	FJsonBuilder Json = FJsonBuilder();

	Json.AddInt("chainId", ChainId);
	Json.AddString("from", "0x" + From.ToHex());
	Json.AddString("to", "0x" + To.ToHex());

	if(this->Value.IsSet()) Json.AddString("value", this->Value.GetValue());

	return Json.ToString();
}

TransactionID FTransaction_Sequence::ID()
{
	FUnsizedData Data = StringToUTF8(ToJson());
	return GetKeccakHash(Data).ToHex();
}

FString USequenceWallet::Url(const FString& Name) const
{
	return this->Hostname + this->Path + Name;
}

void USequenceWallet::SendRPC(const FString& Url, const FString& Content, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure) const
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
	this->Provider = UProvider::Make("");
}

void USequenceWallet::Initialize(FSubsystemCollectionBase& Collection)
{
	UE_LOG(LogTemp,Display,TEXT("Initilizing wallet subsystem"));
}

void USequenceWallet::Deinitialize()
{
	UE_LOG(LogTemp,Display,TEXT("Deinitializing wallet subsystem"));
}

USequenceWallet * USequenceWallet::GetSubSystem()
{
		if (GEngine)
		{
			const TIndirectArray<FWorldContext> Contexts = GEngine->GetWorldContexts();
			for (FWorldContext Context : Contexts)
			{
				if (const UWorld * World = Context.World())
				{
					if (const UGameInstance * GI = UGameplayStatics::GetGameInstance(World))
					{
						if (USequenceWallet * Wallet = GI->GetSubsystem<USequenceWallet>())
						{
							return Wallet;
						}//GameInstance Subsystem Check
					}//GameInstance Check
				}//World Context Check
			}//Context Search
		}//GEngine Check
		else
		{
			UE_LOG(LogTemp,Error,TEXT("Error Accessing GEngine"));
		}
	UE_LOG(LogTemp,Error,TEXT("Error Accessing USequenceWallet GameInstanceSubSystem"));
	return nullptr;
}

TOptional<USequenceWallet*> USequenceWallet::Get()
{
	if (USequenceWallet * Wallet = GetSubSystem())
	{
		if (Wallet->Credentials.RegisteredValid())
		{
			TOptional OptionalWallet(Wallet);
			return OptionalWallet;
		}
		else
		{
			UE_LOG(LogTemp,Warning,TEXT("Wallet is NOT registered and valid checking on disk credentials"));
			const UAuthenticator * Auth = NewObject<UAuthenticator>();
			FStoredCredentials_BE StoredCredentials = Auth->GetStoredCredentials();

			if (StoredCredentials.GetValid())
			{
				UE_LOG(LogTemp,Display,TEXT("Successfully loaded on disk credentials"));
				Wallet->Init(StoredCredentials.GetCredentials());
				TOptional OptionalWallet(Wallet);
				return OptionalWallet;
			}
			else
			{
				UE_LOG(LogTemp,Warning,TEXT("The Credentials on disk were invalid please login"));
			}//Stored Valid Check
		}//Registered Valid Check
	}//SubSystem Check
	return nullptr;
}

TOptional<USequenceWallet*> USequenceWallet::Get(const FCredentials_BE& Credentials)
{
	if (USequenceWallet * Wallet = GetSubSystem())
	{
		Wallet->Init(Credentials);
		TOptional OptionalWallet(Wallet);
		return OptionalWallet;
	}
	return nullptr;
}

TOptional<USequenceWallet*> USequenceWallet::Get(const FCredentials_BE& Credentials, const FString& ProviderUrl)
{
	if (USequenceWallet * Wallet = GetSubSystem())
	{
		Wallet->Init(Credentials,ProviderUrl);
		TOptional OptionalWallet(Wallet);
		return OptionalWallet;
	}
	return nullptr;
}

void USequenceWallet::Init(const FCredentials_BE& CredentialsIn)
{
	this->Credentials = CredentialsIn;
	this->Indexer = NewObject<UIndexer>();
	if (!this->Provider)
	{
		this->Provider = UProvider::Make("");
	}
}

void USequenceWallet::Init(const FCredentials_BE& CredentialsIn,const FString& ProviderURL)
{
	this->Credentials = CredentialsIn;
	this->Indexer = NewObject<UIndexer>();
	if (!this->Provider)
	{
		this->Provider = UProvider::Make(ProviderURL);
	}
	else
	{
		this->Provider->UpdateUrl(ProviderURL);
	}
}

FString USequenceWallet::GetWalletAddress() const
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
		
		//UE_LOG(LogTemp,Display,TEXT("Pre Processing, Response: %s"),*Response);
		if (Json.Get()->TryGetObjectField(TEXT("session"),SessionObj) && Json.Get()->TryGetObjectField(TEXT("response"),ResponseObj))
		{
			const TSharedPtr<FJsonObject> * DataObj = nullptr;
			const TSharedPtr<FJsonObject> * IdentityObj = nullptr;

			FString CreatedAt = "", RefreshedAt = "", ExpiresAt = "", UserId = "";
			if (ResponseObj->Get()->TryGetObjectField(TEXT("data"), DataObj) && SessionObj->Get()->TryGetObjectField(TEXT("identity"), IdentityObj) && SessionObj->Get()->TryGetStringField(TEXT("userId"),UserId) && SessionObj->Get()->TryGetStringField(TEXT("createdAt"),CreatedAt) && SessionObj->Get()->TryGetStringField(TEXT("refreshedAt"), RefreshedAt) && SessionObj->Get()->TryGetStringField(TEXT("expiresAt"), ExpiresAt))
			{
				FString Wallet = "", Type = "", ISS = "", Sub = "", Email = "";
				if (IdentityObj->Get()->TryGetStringField(TEXT("email"), Email) && IdentityObj->Get()->TryGetStringField(TEXT("sub"), Sub) && IdentityObj->Get()->TryGetStringField(TEXT("iss"),ISS) && IdentityObj->Get()->TryGetStringField(TEXT("type"),Type) && DataObj->Get()->TryGetStringField(TEXT("wallet"),Wallet))
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
			//UE_LOG(LogTemp,Display,TEXT("Response: %s"), *ParsedResponse);
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
		//UE_LOG(LogTemp,Display,TEXT("Response: %s"), *Response);
		const TSharedPtr<FJsonObject> Json = UIndexerSupport::JsonStringToObject(Response);
		FCloseResponseObj ResponseStruct;
		if (FJsonObjectConverter::JsonObjectToUStruct<FCloseResponseObj>(Json.ToSharedRef(), &ResponseStruct))
		{
			const FString ParsedResponse = UIndexerSupport::StructToString(ResponseStruct);
			//UE_LOG(LogTemp,Display,TEXT("Response: %s"), *ParsedResponse);
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

int64 USequenceWallet::GetNetworkId() const
{
	return this->Credentials.GetNetwork();
}

void USequenceWallet::UpdateNetworkId(int64 NewNetwork)
{
	this->Credentials.UpdateNetwork(NewNetwork);
	const UAuthenticator * auth = NewObject<UAuthenticator>();
	auth->StoreCredentials(this->Credentials);
}

void USequenceWallet::UpdateProviderURL(const FString& Url) const
{
	if (this->Provider)
	{
		this->Provider->UpdateUrl(Url);
	}
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
			//UE_LOG(LogTemp,Display,TEXT("Response: %s"), *ParsedResponse);
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

void USequenceWallet::SendTransaction(TArray<TUnion<FRawTransaction, FERC20Transaction, FERC721Transaction, FERC1155Transaction>> Transactions, const TSuccessCallback<FTransactionResponse>& OnSuccess, const FFailureCallback& OnFailure)
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
	const TSuccessCallback<FString> OnResponse = [=](FString Response)
	{
#if PLATFORM_ANDROID
		NativeOAuth::AndroidLog(Response);
#endif
		TSharedPtr<FJsonObject> jsonObj;
		if(FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Response), jsonObj))
		{
			//parse out what we want and wrap it nicely for ease of use
			//UE_LOG(LogTemp,Display,TEXT("Response: %s"), *Response);
			const TSharedPtr<FJsonObject> * ResponseObj = nullptr;
			if (jsonObj->TryGetObjectField(TEXT("response"),ResponseObj))
			{
				const TSharedPtr<FJsonObject> * DataObj = nullptr;
				FString Code = "";
				if (ResponseObj->Get()->TryGetObjectField(TEXT("data"),DataObj) && ResponseObj->Get()->TryGetStringField(TEXT("code"),Code))
				{
					FString TxHash = "";
					FString MetaTxHash = "";

					const TSharedPtr<FJsonObject> * NativeReceiptObj = nullptr;
					const TSharedPtr<FJsonObject> * ReceiptObj = nullptr;
					const TSharedPtr<FJsonObject> * RequestObj = nullptr;
					const TArray<TSharedPtr<FJsonValue>> * SimulationsObj = nullptr;
					
					if (DataObj->Get()->TryGetStringField(TEXT("txHash"),TxHash) &&
						DataObj->Get()->TryGetStringField(TEXT("metaTxHash"),MetaTxHash) &&
						DataObj->Get()->TryGetObjectField(TEXT("nativeReceipt"),NativeReceiptObj) &&
						DataObj->Get()->TryGetObjectField(TEXT("receipt"),ReceiptObj) &&
						DataObj->Get()->TryGetObjectField(TEXT("request"),RequestObj) &&
						DataObj->Get()->TryGetArrayField(TEXT("simulations"), SimulationsObj))
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

FString USequenceWallet::BuildSignMessageIntent(const FString& Message) const
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
	//UE_LOG(LogTemp,Display,TEXT("EIP_191: %s"),*EIP_Message);
	
	//EIP-191
	const FString Data = "{\"message\":\""+EIP_Message+"\",\"network\":\""+this->Credentials.GetNetworkString()+"\",\"wallet\":\""+this->Credentials.GetWalletAddress()+"\"}";
	const FString SigIntent = "{\"data\":"+Data+",\"expiresAt\":"+expiresString+",\"issuedAt\":"+issuedString+",\"name\":\"signMessage\",\"version\":\""+this->Credentials.GetWaasVersion()+"\"}";
	const FString Signature = this->GeneratePacketSignature(SigIntent);
	const FString Intent = "{\"intent\":{\"data\":"+Data+",\"expiresAt\":"+expiresString+",\"issuedAt\":"+issuedString+",\"name\":\"signMessage\",\"signatures\":[{\"sessionId\":\""+this->Credentials.GetSessionId()+"\",\"signature\":\""+Signature+"\"}],\"version\":\""+this->Credentials.GetWaasVersion()+"\"}}";
	//UE_LOG(LogTemp,Display,TEXT("SignMessage Intent: %s"),*Intent);
	return Intent;
}

FString USequenceWallet::BuildSendTransactionIntent(const FString& Txns) const
{
	const int64 issued = FDateTime::UtcNow().ToUnixTimestamp() - 30;
	const int64 expires = issued + 86400;
	const FString issuedString = FString::Printf(TEXT("%lld"),issued);
	const FString expiresString = FString::Printf(TEXT("%lld"),expires);
	const FString identifier = "unreal-sdk-" + FDateTime::UtcNow().ToString() + "-" + this->Credentials.GetWalletAddress();
	const FString SigIntent = "{\"data\":{\"identifier\":\""+identifier+"\",\"network\":\""+this->Credentials.GetNetworkString()+"\",\"transactions\":"+Txns+",\"wallet\":\""+this->Credentials.GetWalletAddress()+"\"},\"expiresAt\":"+expiresString+",\"issuedAt\":"+issuedString+",\"name\":\"sendTransaction\",\"version\":\""+this->Credentials.GetWaasVersion()+"\"}";
	const FString Signature = this->GeneratePacketSignature(SigIntent);
	FString Intent = "{\"intent\":{\"data\":{\"identifier\":\""+identifier+"\",\"network\":\""+this->Credentials.GetNetworkString()+"\",\"transactions\":"+Txns+",\"wallet\":\""+this->Credentials.GetWalletAddress()+"\"},\"expiresAt\":"+expiresString+",\"issuedAt\":"+issuedString+",\"name\":\"sendTransaction\",\"signatures\":[{\"sessionId\":\""+this->Credentials.GetSessionId()+"\",\"signature\":\""+Signature+"\"}],\"version\":\""+this->Credentials.GetWaasVersion()+"\"}}";
	//UE_LOG(LogTemp,Display,TEXT("SendTransactionIntent: %s"),*Intent);
	return Intent;
}

FString USequenceWallet::BuildRegisterSessionIntent() const
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
	//UE_LOG(LogTemp,Display,TEXT("RegisterSession Intent: %s"),*Intent);
	return Intent;
}

FString USequenceWallet::BuildListSessionIntent() const
{
	const int64 issued = FDateTime::UtcNow().ToUnixTimestamp() - 30;
	const int64 expires = issued + 86400;
	const FString issuedString = FString::Printf(TEXT("%lld"),issued);
	const FString expiresString = FString::Printf(TEXT("%lld"),expires);
	const FString SigIntent = "{\"data\":{\"wallet\":\""+this->Credentials.GetWalletAddress()+"\"},\"expiresAt\":"+expiresString+",\"issuedAt\":"+issuedString+",\"name\":\"listSessions\",\"version\":\""+this->Credentials.GetWaasVersion()+"\"}";
	const FString Signature = this->GeneratePacketSignature(SigIntent);
	const FString Intent = "{\"intent\":{\"data\":{\"wallet\":\""+this->Credentials.GetWalletAddress()+"\"},\"expiresAt\":"+expiresString+",\"issuedAt\":"+issuedString+",\"name\":\"listSessions\",\"signatures\":[{\"sessionId\":\""+this->Credentials.GetSessionId()+"\",\"signature\":\""+Signature+"\"}],\"version\":\""+this->Credentials.GetWaasVersion()+"\"}}";
	//UE_LOG(LogTemp,Display,TEXT("ListSessionIntent: %s"),*Intent);
	return Intent;
}

FString USequenceWallet::BuildCloseSessionIntent() const
{
	const int64 issued = FDateTime::UtcNow().ToUnixTimestamp() - 30;
	const int64 expires = issued + 86400;
	const FString issuedString = FString::Printf(TEXT("%lld"),issued);
	const FString expiresString = FString::Printf(TEXT("%lld"),expires);
	const FString SigIntent = "{\"data\":{\"sessionId\":\""+this->Credentials.GetSessionId()+"\"},\"expiresAt\":"+expiresString+",\"issuedAt\":"+issuedString+",\"name\":\"closeSession\",\"version\":\""+this->Credentials.GetWaasVersion()+"\"}";
	const FString Signature = this->GeneratePacketSignature(SigIntent);
	const FString Intent = "{\"intent\":{\"data\":{\"sessionId\":\""+this->Credentials.GetSessionId()+"\"},\"expiresAt\":"+expiresString+",\"issuedAt\":"+issuedString+",\"name\":\"closeSession\",\"signatures\":[{\"sessionId\":\""+this->Credentials.GetSessionId()+"\",\"signature\":\""+Signature+"\"}],\"version\":\""+this->Credentials.GetWaasVersion()+"\"}}";
	//UE_LOG(LogTemp,Display,TEXT("CloseSessionIntent: %s"),*Intent);
	return Intent;
}

FString USequenceWallet::BuildSessionValidationIntent() const
{
	const FString Intent = "{\\\"sessionId\\\":\\\""+this->Credentials.GetSessionId()+"\\\"}";
	//UE_LOG(LogTemp,Display,TEXT("SessionValidationIntent: %s"),*Intent);
	return Intent;
}

template <typename T> void USequenceWallet::SequenceRPC(FString Url, FString Content, TSuccessCallback<T> OnSuccess, FFailureCallback OnFailure)
{
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

FString USequenceWallet::getSequenceURL(const FString& endpoint) const
{
	return this->SequenceURL + endpoint;
}

TArray<FContact_BE> USequenceWallet::BuildFriendListFromJson(const FString& JSON)
{
	TArray<FContact_BE> friendList;
	TSharedPtr<FJsonObject> jsonObj;

	if (FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(JSON), jsonObj))
	{
		const TArray<TSharedPtr<FJsonValue>>* storedFriends;
		if (jsonObj.Get()->TryGetArrayField(TEXT("friends"), storedFriends))
		{
			for (TSharedPtr<FJsonValue> friendData : *storedFriends)
			{
				const TSharedPtr<FJsonObject>* fJsonObj;
				if (friendData.Get()->TryGetObject(fJsonObj))//need it as an object
				{
					FContact_BE newFriend;
					newFriend.Public_Address = fJsonObj->Get()->GetStringField(TEXT("userAddress"));
					newFriend.Nickname = fJsonObj->Get()->GetStringField(TEXT("nickname"));
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
void USequenceWallet::GetFriends(FString username, TSuccessCallback<TArray<FContact_BE>> OnSuccess, const FFailureCallback& OnFailure) const
{
	const FString json_arg = "{}";
	
	SendRPC(getSequenceURL("friendList"), json_arg, [this,OnSuccess](const FString& Content)
		{
			OnSuccess(this->BuildFriendListFromJson(Content));
		}, OnFailure);
}

TArray<FItemPrice_BE> USequenceWallet::BuildItemUpdateListFromJson(const FString& JSON)
{
	//UE_LOG(LogTemp, Error, TEXT("Received json: %s"), *JSON);
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

void USequenceWallet::GetUpdatedCoinPrice(const FID_BE& ItemToUpdate, const TSuccessCallback<TArray<FItemPrice_BE>>& OnSuccess, const FFailureCallback& OnFailure) const
{
	TArray<FID_BE> items;
	items.Add(ItemToUpdate);
	GetUpdatedCoinPrices(items, OnSuccess, OnFailure);
}

void USequenceWallet::GetUpdatedCoinPrices(TArray<FID_BE> ItemsToUpdate, TSuccessCallback<TArray<FItemPrice_BE>> OnSuccess, const FFailureCallback& OnFailure) const
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

	SendRPC(getSequenceURL("getCoinPrices"), args, [this,OnSuccess](const FString& Content)
		{
			OnSuccess(this->BuildItemUpdateListFromJson(Content));
		}, OnFailure);
}

void USequenceWallet::GetUpdatedCollectiblePrice(const FID_BE& ItemToUpdate, const TSuccessCallback<TArray<FItemPrice_BE>>& OnSuccess, const FFailureCallback& OnFailure) const
{
	TArray<FID_BE> items;
	items.Add(ItemToUpdate);
	GetUpdatedCollectiblePrices(items, OnSuccess, OnFailure);
}

void USequenceWallet::GetUpdatedCollectiblePrices(TArray<FID_BE> ItemsToUpdate, TSuccessCallback<TArray<FItemPrice_BE>> OnSuccess, const FFailureCallback& OnFailure) const
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

	SendRPC(getSequenceURL("getCollectiblePrices"), args, [this,OnSuccess](const FString& Content)
		{
			OnSuccess(this->BuildItemUpdateListFromJson(Content));
		}, OnFailure);
}

FString USequenceWallet::BuildQr_Request_URL(const FString& walletAddress,int32 Size) const
{
	FString urlSize = "/";
	urlSize.AppendInt(Size);
	return SequenceURL_Qr + encodeB64_URL(walletAddress) + urlSize;
}

//we only need to encode base64URL we don't decode them as we receive the QR code
FString USequenceWallet::encodeB64_URL(const FString& data)
{
	FString ret = FBase64::Encode(data);
	//now we just gotta do some swaps to make it base64 URL compliant
	// + -> -
	// / -> _ 

	const FString srch_plus = TEXT("+");
	const FString rep_plus = TEXT("-");
	const FString srch_slash = TEXT("/");
	const FString rep_slash = TEXT("_");

	const TCHAR* srch_ptr_plus = *srch_plus;
	const TCHAR* rep_ptr_plus = *rep_plus;
	const TCHAR* srch_ptr_slash = *srch_slash;
	const TCHAR* rep_ptr_slash = *rep_slash;

	ret.ReplaceInline(srch_ptr_plus, rep_ptr_plus, ESearchCase::IgnoreCase);//remove + and replace with -
	ret.ReplaceInline(srch_ptr_slash, rep_ptr_slash, ESearchCase::IgnoreCase);//remove / and replace with _

	return ret;
}

//Indexer Calls

void USequenceWallet::Ping(const TSuccessCallback<bool>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Indexer)
		this->Indexer->Ping(this->Credentials.GetNetwork(), OnSuccess, OnFailure);
}

void USequenceWallet::Version(const TSuccessCallback<FVersion>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Indexer)
		this->Indexer->Version(this->Credentials.GetNetwork(),OnSuccess,OnFailure);
}

void USequenceWallet::RunTimeStatus(const TSuccessCallback<FRuntimeStatus>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Indexer)
		this->Indexer->RunTimeStatus(this->Credentials.GetNetwork(), OnSuccess, OnFailure);
}

void USequenceWallet::GetChainID(const TSuccessCallback<int64>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Indexer)
		this->Indexer->GetChainID(this->Credentials.GetNetwork(), OnSuccess, OnFailure);
}

void USequenceWallet::GetEtherBalance(const FString& AccountAddr, const TSuccessCallback<FEtherBalance>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Indexer)
		this->Indexer->GetEtherBalance(this->Credentials.GetNetwork(), AccountAddr, OnSuccess, OnFailure);
}

void USequenceWallet::GetTokenBalances(const FGetTokenBalancesArgs& Args, const TSuccessCallback<FGetTokenBalancesReturn>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Indexer)
		this->Indexer->GetTokenBalances(this->Credentials.GetNetwork(), Args, OnSuccess, OnFailure);
}

void USequenceWallet::GetTokenSupplies(const FGetTokenSuppliesArgs& Args, const TSuccessCallback<FGetTokenSuppliesReturn>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Indexer)
		this->Indexer->GetTokenSupplies(this->Credentials.GetNetwork(), Args, OnSuccess, OnFailure);
}

void USequenceWallet::GetTokenSuppliesMap(const FGetTokenSuppliesMapArgs& Args, const TSuccessCallback<FGetTokenSuppliesMapReturn>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Indexer)
		this->Indexer->GetTokenSuppliesMap(this->Credentials.GetNetwork(), Args, OnSuccess, OnFailure);
}

void USequenceWallet::GetBalanceUpdates(const FGetBalanceUpdatesArgs& Args, const TSuccessCallback<FGetBalanceUpdatesReturn>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Indexer)
		this->Indexer->GetBalanceUpdates(this->Credentials.GetNetwork(), Args, OnSuccess, OnFailure);
}

void USequenceWallet::GetTransactionHistory(const FGetTransactionHistoryArgs& Args, const TSuccessCallback<FGetTransactionHistoryReturn>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Indexer)
		this->Indexer->GetTransactionHistory(this->Credentials.GetNetwork(), Args, OnSuccess, OnFailure);
}

void USequenceWallet::BlockByNumber(uint64 Number, const TSuccessCallback<TSharedPtr<FJsonObject>>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->BlockByNumber(Number, OnSuccess, OnFailure);
	}
}

void USequenceWallet::BlockByNumber(EBlockTag Tag, const TSuccessCallback<TSharedPtr<FJsonObject>>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->BlockByNumber(Tag,OnSuccess,OnFailure);
	}
}

void USequenceWallet::BlockByHash(const FHash256& Hash, const TSuccessCallback<TSharedPtr<FJsonObject>>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->BlockByHash(Hash, OnSuccess, OnFailure);
	}
}

void USequenceWallet::BlockNumber(const TSuccessCallback<uint64>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->BlockNumber(OnSuccess, OnFailure);
	}
}

void USequenceWallet::HeaderByNumber(uint64 Id, const TSuccessCallback<FHeader>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->HeaderByNumber(Id, OnSuccess, OnFailure);
	}
}

void USequenceWallet::HeaderByNumber(EBlockTag Tag, const TSuccessCallback<FHeader>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->HeaderByNumber(Tag, OnSuccess, OnFailure);
	}
}

void USequenceWallet::HeaderByHash(const FHash256& Hash, const TSuccessCallback<FHeader>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->HeaderByHash(Hash,OnSuccess,OnFailure);
	}
}

void USequenceWallet::TransactionByHash(const FHash256& Hash, const TSuccessCallback<TSharedPtr<FJsonObject>>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->TransactionByHash(Hash,OnSuccess,OnFailure);
	}
}

void USequenceWallet::TransactionCount(const FAddress& Addr, uint64 Number, const TSuccessCallback<uint64>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->TransactionCount(Addr,Number,OnSuccess,OnFailure);	
	}
}

void USequenceWallet::TransactionCount(const FAddress& Addr, EBlockTag Tag, const TSuccessCallback<uint64>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->TransactionCount(Addr,Tag,OnSuccess,OnFailure);
	}
}

void USequenceWallet::TransactionReceipt(const FHash256& Hash, const TSuccessCallback<FTransactionReceipt>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->TransactionReceipt(Hash,OnSuccess,OnFailure);
	}
}

void USequenceWallet::GetGasPrice(const TSuccessCallback<FUnsizedData>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->GetGasPrice(OnSuccess, OnFailure);
	}
}

void USequenceWallet::EstimateContractCallGas(const FContractCall& ContractCall, const TSuccessCallback<FUnsizedData>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->EstimateContractCallGas(ContractCall,OnSuccess,OnFailure);
	}
}

void USequenceWallet::EstimateDeploymentGas(const FAddress& From, const FString& Bytecode, const TSuccessCallback<FUnsizedData>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->EstimateDeploymentGas(From,Bytecode,OnSuccess,OnFailure);
	}
}

void USequenceWallet::DeployContract(const FString& Bytecode, const FPrivateKey& PrivKey, int64 ChainId, const TSuccessCallback<FAddress>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->DeployContract(Bytecode, PrivKey, ChainId, OnSuccess, OnFailure);
	}
}

void USequenceWallet::DeployContractWithHash(const FString& Bytecode, const FPrivateKey& PrivKey, int64 ChainId, const TSuccessCallbackTuple<FAddress, FUnsizedData>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->DeployContractWithHash(Bytecode,PrivKey,ChainId,OnSuccess,OnFailure);
	}
}

void USequenceWallet::NonceAt(uint64 Number, const TSuccessCallback<FBlockNonce>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->NonceAt(Number,OnSuccess,OnFailure);
	}
}

void USequenceWallet::NonceAt(EBlockTag Tag, const TSuccessCallback<FBlockNonce>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->NonceAt(Tag,OnSuccess,OnFailure);
	}
}

void USequenceWallet::SendRawTransaction(const FString& Data, const TSuccessCallback<FUnsizedData>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->SendRawTransaction(Data,OnSuccess,OnFailure);
	}
}

void USequenceWallet::ChainId(const TSuccessCallback<uint64>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->ChainId(OnSuccess,OnFailure);
	}
}

void USequenceWallet::Call(const FContractCall& ContractCall, uint64 Number, const TSuccessCallback<FUnsizedData>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->Call(ContractCall,Number,OnSuccess,OnFailure);
	}
}

void USequenceWallet::Call(const FContractCall& ContractCall, EBlockTag Number, const TSuccessCallback<FUnsizedData>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->Call(ContractCall,Number,OnSuccess,OnFailure);
	}
}

void USequenceWallet::NonViewCall(const FEthTransaction& Transaction, const FPrivateKey& PrivateKey, int ChainID, const TSuccessCallback<FUnsizedData>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Provider)
	{
		this->Provider->NonViewCall(Transaction, PrivateKey, ChainID, OnSuccess, OnFailure);
	}
}