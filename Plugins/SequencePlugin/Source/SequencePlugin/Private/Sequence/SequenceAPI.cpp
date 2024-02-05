#include "Sequence/SequenceAPI.h"
#include "Eth/Crypto.h"
#include "RequestHandler.h"
#include "Indexer/IndexerSupport.h"
#include "Util/JsonBuilder.h"
#include "Dom/JsonObject.h"
#include "JsonObjectConverter.h"
#include "AES/aes.h"
#include "Bitcoin-Cryptography-Library/cpp/Keccak256.hpp"
#include "JsonUtils/JsonPointer.h"
#include "Types/ContractCall.h"
#include "Misc/Base64.h"
#include "Util/HexUtility.h"

FString SortOrderToString(ESortOrder SortOrder)
{
	return UEnum::GetValueAsString(SortOrder);
}

ESortOrder StringToSortOrder(FString String)
{
	return String == "ASC" ? ESortOrder::ASC : ESortOrder::DESC;
}

FString FPage_Sequence::ToJson()
{
	FJsonBuilder Json = FJsonBuilder();

	if(PageSize.IsSet())
	{
		Json.AddInt("pageSize", PageSize.GetValue());
	}
	if(PageNum.IsSet())
	{
		Json.AddInt("page", PageNum.GetValue());
	}
	if(TotalRecords.IsSet())
	{
		Json.AddInt("totalRecords", TotalRecords.GetValue());
	}
	if(Column.IsSet())
	{
		Json.AddString("column", Column.GetValue());
	}
	if(Sort.IsSet())
	{
		FJsonArray Array = Json.AddArray("sort");

		for(FSortBy SortBy : Sort.GetValue())
		{
			Array.AddValue(SortBy.GetJsonString());
		}
	}
	
	return Json.ToString();
}

FPage_Sequence FPage_Sequence::Convert(FPage Page,int64 TotalRecords)
{
	FPage_Sequence Ret;
	Ret.Column = Page.column;
	Ret.PageNum = Page.page;
	Ret.PageSize = Page.pageSize;
	Ret.Sort = Page.sort;
	Ret.TotalRecords = TotalRecords;
	return Ret;
}

FPage_Sequence FPage_Sequence::From(TSharedPtr<FJsonObject> Json)
{
	FPage_Sequence page = FPage_Sequence{};

	if(Json->HasField("pageSize"))
	{
		page.PageSize = static_cast<uint64>(Json->GetIntegerField("pageSize")); 
	}
	if(Json->HasField("page"))
	{
		page.PageNum =  static_cast<uint64>(Json->GetIntegerField("page")); 
	}
	if(Json->HasField("totalRecords"))
	{
		page.TotalRecords = static_cast<uint64>(Json->GetIntegerField("totalRecords")); 
	}
	if(Json->HasField("Column"))
	{
		page.Column = Json->GetStringField("Column");
	}
	if(Json->HasField("Sort"))
	{
		TArray<TSharedPtr<FJsonValue>> JsonArray = Json->GetArrayField("Sort");
		TArray<FSortBy> Array;

		for(TSharedPtr<FJsonValue> JsonVal : JsonArray)
		{
			FSortBy Result;
			if (!FJsonObjectConverter::JsonObjectToUStruct<FSortBy>(JsonVal->AsObject().ToSharedRef(), &Result))
				Array.Push(Result);
		}
		
		page.Sort = Array;
	}

	return page;
}

//Hide for now
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

FPartnerWallet FPartnerWallet::From(TSharedPtr<FJsonObject> Json)
{
	return FPartnerWallet{
		static_cast<uint64>(Json->GetIntegerField("number")),
		static_cast<uint64>(Json->GetIntegerField("partnerId")),
		static_cast<uint64>(Json->GetIntegerField("walletIndex")),
		Json->GetStringField("walletAddress")
	};
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
			->WithHeader("Authorization", this->AuthToken)
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
	USequenceWallet * Wallet = NewObject<USequenceWallet>();
	Wallet->Init(CredentialsIn);
	return Wallet;
}

USequenceWallet* USequenceWallet::Make(const FCredentials_BE& CredentialsIn, const FString& ProviderURL)
{
	USequenceWallet * Wallet = NewObject<USequenceWallet>();
	Wallet->Init(CredentialsIn,ProviderURL);
	return Wallet;
}

void USequenceWallet::Init(const FCredentials_BE& CredentialsIn)
{
	this->Credentials = CredentialsIn;
	this->Indexer = NewObject<UIndexer>();
	this->AuthToken = "Bearer " + this->Credentials.GetIDToken();
}

void USequenceWallet::Init(const FCredentials_BE& CredentialsIn,const FString& ProviderURL)
{
	this->Credentials = CredentialsIn;
	this->Indexer = NewObject<UIndexer>();
	this->AuthToken = "Bearer " + this->Credentials.GetIDToken();
	this->ProviderUrl = ProviderURL;
}

FString USequenceWallet::GetWalletAddress()
{
	return this->Credentials.GetWalletAddress();
}

void USequenceWallet::RegisterSession(const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure)
{
	const TSuccessCallback<FString> OnResponse = [this,OnSuccess,OnFailure](const FString& Response)
	{
		const TSharedPtr<FJsonObject> Json = UIndexerSupport::JsonStringToObject(Response);
		const TSharedPtr<FJsonObject> * Data = nullptr;
		if (Json.Get()->TryGetObjectField("data",Data))
		{
			FString RegisteredSessionId, RegisteredWalletAddress;
			if (Data->Get()->TryGetStringField("sessionId",RegisteredSessionId) && Data->Get()->TryGetStringField("wallet",RegisteredWalletAddress))
			{
				this->Credentials.RegisterSessionData(RegisteredSessionId,RegisteredWalletAddress);
				const UAuthenticator * TUAuth = NewObject<UAuthenticator>();
				TUAuth->StoreCredentials(this->Credentials);
				FString Creds = UIndexerSupport::structToString(this->Credentials);
				UE_LOG(LogTemp,Display,TEXT("Creds: %s"),*Creds);
				OnSuccess("Session Registered");
			}
			else
			{
				OnFailure(FSequenceError(RequestFail, "Request failed: " + Response));
			}
		}
		else
		{
			OnFailure(FSequenceError(RequestFail, "Request failed: " + Response));
		}
	};
	if (this->Credentials.Valid())
		this->SequenceRPC("https://dev-waas.sequence.app/rpc/WaasAuthenticator/RegisterSession",this->GenerateSignedEncryptedRegisterSessionPayload(this->BuildRegisterSessionIntent()),OnResponse,OnFailure);
	else
		OnFailure(FSequenceError(RequestFail, "[Invalid Credentials please login first]"));
}

void USequenceWallet::ListSessions(const TSuccessCallback<TArray<FSession>>& OnSuccess, const FFailureCallback& OnFailure)
{
	const TSuccessCallback<FString> OnResponse = [this,OnSuccess,OnFailure](const FString& Response)
	{
		TArray<FSession> Sessions;
		const TSharedPtr<FJsonObject> Json = UIndexerSupport::JsonStringToObject(Response);
		TArray<TSharedPtr<FJsonValue>> Arr;
		const TArray<TSharedPtr<FJsonValue>>* Data = &Arr;
		if (Json.Get()->TryGetArrayField("sessions",Data))
		{
			for (int i = 0; i < Arr.Num(); i++) {
				TSharedPtr<FJsonValue> SessionJson = Arr[i];
				FSession Session = UIndexerSupport::jsonStringToStruct<FSession>(SessionJson.Get()->AsString());
				Sessions.Push(Session);
			}

			OnSuccess(Sessions);
		}
		else
		{
			OnFailure(FSequenceError(RequestFail, "Request failed: " + Response));
		}
	};
	
	if (this->Credentials.IsRegistered() && this->Credentials.Valid())
		this->SequenceRPC("https://dev-waas.sequence.app/rpc/WaasAuthenticator/ListSessions",this->SignAndEncryptPayload(this->BuildListSessionIntent()),OnResponse,OnFailure);
	else
		OnFailure(FSequenceError(RequestFail, "[Session Not Registered Please Register Session First]"));
}

void USequenceWallet::CloseSession(const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure)
{
	const TSuccessCallback<FString> OnResponse = [this,OnSuccess,OnFailure](const FString& Response)
	{
		const TSharedPtr<FJsonObject> Json = UIndexerSupport::JsonStringToObject(Response);
		if(Json.IsValid() && Json->GetBoolField("ok"))
		{
			this->Credentials.UnRegisterCredentials();
			UAuthenticator *Auth = NewObject<UAuthenticator>();
			Auth->StoreCredentials(this->Credentials);//update credentials to ensure we remember that this session is now closed
			OnSuccess("[SessionClosed]");
		}
		else
		{
			OnFailure(FSequenceError(RequestFail, "Malformed response: " + Response));
		}
	};
	if (this->Credentials.IsRegistered() && this->Credentials.Valid())
		this->SequenceRPC("https://dev-waas.sequence.app/rpc/WaasAuthenticator/DropSession",this->SignAndEncryptPayload(this->BuildCloseSessionIntent()),OnResponse,OnFailure);
	else
		OnFailure(FSequenceError(RequestFail, "[Session Not Registered Please Register Session First]"));
}

void USequenceWallet::SessionValidation(const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure)
{
	OnSuccess("[RPC_NotActive]");
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

FString USequenceWallet::GenerateSignedEncryptedRegisterSessionPayload(const FString& Intent) const
{
	const FString SessionAddr = "0x" + this->Credentials.GetSessionWalletAddress();
	const FString PreEncryptedPayload = "{\"projectId\": "+this->Credentials.GetProjectId()+",\"idToken\": \""+this->Credentials.GetIDToken()+"\",\"sessionAddress\": \""+SessionAddr+"\",\"friendlyName\": \"FRIENDLY SESSION WALLET\",\"intentJSON\": \""+Intent+"\"}";
	UE_LOG(LogTemp,Display,TEXT("PreEncryptedPayload:\n%s"),*PreEncryptedPayload);
	return SignAndEncryptPayload(PreEncryptedPayload);
}

FString USequenceWallet::GenerateSignedEncryptedPayload(const FString& Intent) const
{
	const FString PreEncryptedPayload = "{\"sessionId\":\""+this->Credentials.GetSessionId()+"\",\"intentJson\":\""+Intent+"\"}";
	UE_LOG(LogTemp,Display,TEXT("PreEncryptedPayload:\n%s"),*PreEncryptedPayload);
	UE_LOG(LogTemp,Display,TEXT("IntentJson: %s"),*Intent);
	return SignAndEncryptPayload(PreEncryptedPayload);
}

FString USequenceWallet::SignAndEncryptPayload(const FString& Intent) const
{
	if (!this->Credentials.Valid())
	{
		UE_LOG(LogTemp,Error,TEXT("[Credentials are not valid! They are either expired or not built correctly, Please ReAuthenticate]"));
		return "{Credentials Invalid!}";
	}
	
	TArray<uint8_t> TPayload = PKCS7(Intent);
	AES_ctx ctx;
	struct AES_ctx* PtrCtx = &ctx;
	constexpr int32 IVSize = 16;
	TArray<uint8_t> iv;

	for (int i = 0; i < IVSize; i++)
		iv.Add(RandomByte());

	AES_init_ctx_iv(PtrCtx,this->Credentials.GetTransportKey().GetData(), iv.GetData());
	AES_CBC_encrypt_buffer(PtrCtx, TPayload.GetData(), TPayload.Num());
	const FString PayloadCipherText = "0x" + BytesToHex(iv.GetData(), iv.Num()).ToLower() + BytesToHex(TPayload.GetData(), TPayload.Num()).ToLower();
	const FString PayloadSig = "0x" + this->Credentials.SignMessageWithSessionWallet(Intent);
	const FString EncryptedPayloadKey = "0x" + this->Credentials.GetEncryptedPayloadKey();
	FString FinalPayload = "{\"encryptedPayloadKey\":\""+ EncryptedPayloadKey +"\",\"payloadCiphertext\":\""+ PayloadCipherText +"\",\"payloadSig\":\""+PayloadSig+"\"}";
	UE_LOG(LogTemp,Display,TEXT("FinalPayload: %s"),*FinalPayload);
	return FinalPayload;
}

void USequenceWallet::SignMessage(const FString& Message, const TSuccessCallback<FSignedMessage>& OnSuccess, const FFailureCallback& OnFailure)
{
	const TSuccessCallback<FString> OnResponse = [this,OnSuccess,OnFailure](const FString& Response)
	{
		const TSharedPtr<FJsonObject> Json = UIndexerSupport::JsonStringToObject(Response);
		const TSharedPtr<FJsonObject> * Data = nullptr;
		FSignedMessage Msg;
		if (Json.Get()->TryGetObjectField("data",Data))
		{
			if (FJsonObjectConverter::JsonObjectToUStruct<FSignedMessage>(Data->ToSharedRef(), &Msg))
			{
				OnSuccess(Msg);
			}
			else
			{
				OnFailure(FSequenceError(RequestFail, "Malformed Response: " + Response));
			}
		}
		else
		{
			OnFailure(FSequenceError(RequestFail, "Request failed: " + Response));
		}
	};
	if (this->Credentials.IsRegistered() && this->Credentials.Valid())
		this->SequenceRPC("https://dev-waas.sequence.app/rpc/WaasAuthenticator/SendIntent",this->GenerateSignedEncryptedPayload(this->BuildSignMessageIntent(Message)),OnResponse,OnFailure);
	else
		OnFailure(FSequenceError(RequestFail, "[Session Not Registered Please Register Session First]"));
}

void USequenceWallet::SendTransaction(TArray<TUnion<FRawTransaction, FERC20Transaction, FERC721Transaction, FERC1155Transaction>> Transactions, TSuccessCallback<TSharedPtr<FJsonObject>> OnSuccess, FFailureCallback OnFailure)
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
	//{"sessionId":"0x2Ccf0230f4f04CCA0f89E785fA82D4E6a8bb4aBe","intentJson":"{\"version\":\"1.0.0\",\"packet\":{\"code\":\"sendTransaction\",\"expires\":1706708327,\"identifier\":\"75c636f2-3084-4d94-bcc1-668dbb53d6f7\",\"issued\":1706708297,\"network\":\"137\",\"transactions\":[{\"to\":\"0x9766bf76b2E3e7BCB8c61410A3fC873f1e89b43f\",\"token\":\"0x0d500B1d8E8eF31E21C99d1Db9A6444d3ADf1270\",\"type\":\"erc20send\",\"value\":\"1\"}],\"wallet\":\"0x2D566542570771c264b98959B037f4eb7534caaA\"},\"signatures\":[{\"session\":\"0x2Ccf0230f4f04CCA0f89E785fA82D4E6a8bb4aBe\",\"signature\":\"0xb988f79a670ec047e1705e18c848deecea1aa2f3e3f5e5137387c6237b0ee5510fe02abb770a42353a7801876077353ea6b88897141d74c741dccabdf17429101b\"}]}"}
	//{"sessionId":"0x116dca29ce380C38fddBA2D0716753f314725207","intentJson":"{\"version\":\"1.0.0\",\"packet\":{\"code\":\"sendTransaction\",\"expires\":1706798484,\"identifier\":\"unreal-sdk-2024.01.31-14.41.54-0xDdF\",\"issued\":1706712084,\"network\":\"137\",\"transactions\":[{\"to\":\"0x0E0f9d1c4BeF9f0B8a2D9D4c09529F260C7758A2\",\"token\":\"0x2791Bca1f2de4661ED88A30C99A7a9449Aa84174\",\"type\":\"erc20send\",\"value\":\"1\"}],\"wallet\":\"0xDdF51fe5a7D618144E117fD23Ac21A487e397C91\"},\"signatures\":[{\"session\":\"0x116dca29ce380C38fddBA2D0716753f314725207\",\"signature\":\"0x3e48897217bf82dec4d61b57173cf891e4668f03b909a709274b14f885ae1a1153e92ac15baba468fd558098e145beb030ecd8f0b324ae56c9751ae934dcddbc00\"}]}"}

	TSuccessCallback<FString> OnResponse = [=](FString Response)
	{
		TSharedPtr<FJsonObject> jsonObj;
		if(FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Response), jsonObj))
		{
			OnSuccess(jsonObj);
		}
		else
		{
			OnFailure(FSequenceError(RequestFail, "Request failed: " + Response));
		}
	};
	
	if (this->Credentials.IsRegistered() && this->Credentials.Valid())
		this->SequenceRPC("https://dev-waas.sequence.app/rpc/WaasAuthenticator/SendIntent", this->GenerateSignedEncryptedPayload(BuildSendTransactionIntent(TransactionsPayload)), OnResponse, OnFailure);
	else
		OnFailure(FSequenceError(RequestFail, "[Session Not Registered Please Register Session First]"));
}

FString USequenceWallet::BuildSignMessageIntent(const FString& message)
{
	const int64 issued = FDateTime::UtcNow().ToUnixTimestamp() - 30;
	const int64 expires = issued + 86400;
	const FString issuedString = FString::Printf(TEXT("%lld"),issued);
	const FString expiresString = FString::Printf(TEXT("%lld"),expires);
	
	const FString Wallet = this->Credentials.GetWalletAddress();
	//eip-191 and keccak hashing the message
	const FString LeadingByte = "\x19";//leading byte
	FString Payload = LeadingByte + "Ethereum Signed Message:\n";
	Payload.AppendInt(message.Len());
	Payload += message;
	const FUnsizedData PayloadBytes = StringToUTF8(Payload);
	const FString EIP_Message = "0x" + BytesToHex(PayloadBytes.Ptr(),PayloadBytes.GetLength());
	UE_LOG(LogTemp,Display,TEXT("EIP_191: %s"),*EIP_Message);
	//EIP-191
	
	const FString Packet = "{\\\"code\\\":\\\"signMessage\\\",\\\"expires\\\":"+expiresString+",\\\"issued\\\":"+issuedString+",\\\"message\\\":\\\""+EIP_Message+"\\\",\\\"network\\\":\\\""+this->Credentials.GetNetworkString()+"\\\",\\\"wallet\\\":\\\""+Wallet+"\\\"}";
	const FString PacketRaw = "{\"code\":\"signMessage\",\"expires\":"+expiresString+",\"issued\":"+issuedString+",\"message\":\""+EIP_Message+"\",\"network\":\""+this->Credentials.GetNetworkString()+"\",\"wallet\":\""+Wallet+"\"}";
	const FString Signature = this->GeneratePacketSignature(PacketRaw);
	FString Intent = "{\\\"version\\\":\\\""+this->Credentials.GetWaasVersin()+"\\\",\\\"packet\\\":"+Packet+",\\\"signatures\\\":[{\\\"session\\\":\\\""+this->Credentials.GetSessionId()+"\\\",\\\"signature\\\":\\\""+Signature+"\\\"}]}";
	UE_LOG(LogTemp,Display,TEXT("SignMessageIntent: %s"),*Intent);
	return Intent;
}

//{"sessionId":"0xA4057588629C81Dac0e67870b2Aa6359f981C6Bc","intentJson":"{\"version\":\"1.0.0\",\"packet\":{\"code\":\"sendTransaction\",\"expires\":1706804837,\"identifier\":\"unreal-sdk-2024.01.31-16.27.47-0xDdF51fe5a7D618144E117fD23Ac21A487e397C91\",\"issued\":1706718437,\"network\":\"137\",\"transactions\":[{\"data\":\"null\",\"true\",\"to\":\"0x0E0f9d1c4BeF9f0B8a2D9D4c09529F260C7758A2\",\"token\":\"0xa9a6A3626993D487d2Dbda3173cf58cA1a9D9e9f\",\"type\":\"erc721send\"}],\"wallet\":\"0xDdF51fe5a7D618144E117fD23Ac21A487e397C91\"},\"signatures\":[{\"session\":\"0xA4057588629C81Dac0e67870b2Aa6359f981C6Bc\",\"signature\":\"0xb3261d36798830bcc26ab784151d1c5db8332bc154846eafe6b54992e09dcdb41b00c804b721c6bc784e6e3fa1f8dbfb423088d8ff2190a84cd9ba3635bed4b800\"}]}"}

FString USequenceWallet::BuildSendTransactionIntent(const FString& Txns)
{
	const int64 issued = FDateTime::UtcNow().ToUnixTimestamp() - 30;
	const int64 expires = issued + 86400;
	const FString issuedString = FString::Printf(TEXT("%lld"),issued);
	const FString expiresString = FString::Printf(TEXT("%lld"),expires);
	const FString identifier = "unreal-sdk-" + FDateTime::UtcNow().ToString() + "-" + this->Credentials.GetWalletAddress();
	FString PaddedTxns = Txns;
	const FString srch_slash = TEXT("\"");
	const FString replace = TEXT("\\\"");
	const TCHAR* srch_ptr_slash = *srch_slash;
	const TCHAR* rep_ptr = *replace;
	(PaddedTxns).ReplaceInline(srch_ptr_slash, rep_ptr, ESearchCase::IgnoreCase);
	
	const FString Packet = "{\\\"code\\\":\\\"sendTransaction\\\",\\\"expires\\\":"+expiresString+",\\\"identifier\\\":\\\""+identifier+"\\\",\\\"issued\\\":"+issuedString+",\\\"network\\\":\\\""+this->Credentials.GetNetworkString()+"\\\",\\\"transactions\\\":"+PaddedTxns+",\\\"wallet\\\":\\\""+this->Credentials.GetWalletAddress()+"\\\"}";
	const FString RawPacket = "{\"code\":\"sendTransaction\",\"expires\":"+expiresString+",\"identifier\":\""+identifier+"\",\"issued\":"+issuedString+",\"network\":\""+this->Credentials.GetNetworkString()+"\",\"transactions\":"+Txns+",\"wallet\":\""+this->Credentials.GetWalletAddress()+"\"}";
	const FString Signature = this->GeneratePacketSignature(RawPacket);
	FString Intent = "{\\\"version\\\":\\\""+this->Credentials.GetWaasVersin()+"\\\",\\\"packet\\\":"+Packet+",\\\"signatures\\\":[{\\\"session\\\":\\\""+this->Credentials.GetSessionId()+"\\\",\\\"signature\\\":\\\""+Signature+"\\\"}]}";
	UE_LOG(LogTemp,Display,TEXT("SendTransactionIntent: %s"),*Intent);
	return Intent;
}

FString USequenceWallet::BuildRegisterSessionIntent()
{
	const int64 issued = FDateTime::UtcNow().ToUnixTimestamp() - 30;
	const int64 expires = issued + 86400;
	const FString issuedString = FString::Printf(TEXT("%lld"),issued);
	const FString expiresString = FString::Printf(TEXT("%lld"),expires);
	const FString Packet = "{\\\"code\\\":\\\"openSession\\\",\\\"expires\\\":"+expiresString+",\\\"issued\\\":"+issuedString+",\\\"session\\\":\\\""+this->Credentials.GetSessionId()+"\\\",\\\"proof\\\":{\\\"idToken\\\":\\\""+this->Credentials.GetIDToken()+"\\\"}}";
	const FString Intent = "{\\\"version\\\":\\\""+this->Credentials.GetWaasVersin()+"\\\",\\\"packet\\\":"+Packet+"}";
	UE_LOG(LogTemp,Display,TEXT("RegisterSessionIntent: %s"),*Intent);
	return Intent;
}

FString USequenceWallet::BuildListSessionIntent()
{
	const FString Intent = "{\"sessionId\":\""+this->Credentials.GetSessionId()+"\"}";
	UE_LOG(LogTemp,Display,TEXT("ListSessionIntent: %s"),*Intent);
	return Intent;
}

FString USequenceWallet::BuildCloseSessionIntent()
{
	FString Intent = "{\"sessionId\":\""+this->Credentials.GetSessionId()+"\",\"dropSessionId\":\""+this->Credentials.GetSessionId()+"\"}";
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
	return this->sequenceURL + endpoint;
}

TArray<FContact_BE> USequenceWallet::buildFriendListFromJson(FString json)
{
	TArray<FContact_BE> friendList;
	TSharedPtr<FJsonObject> jsonObj;

	if (FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(json), jsonObj))
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
		UE_LOG(LogTemp, Error, TEXT("Failed to convert String: %s to Json object"), *json);
	}
	return friendList;
}

//DEPRECATED
/*
* Gets the friend data from the given username!
* This function appears to require some form of authentication (perhaps all of the sequence api does)
* @Deprecated
*/
void USequenceWallet::getFriends(FString username, TSuccessCallback<TArray<FContact_BE>> OnSuccess, FFailureCallback OnFailure)
{
	FString json_arg = "{}";
	
	SendRPC(getSequenceURL("friendList"), json_arg, [=](FString Content)
		{
			OnSuccess(buildFriendListFromJson(Content));
		}, OnFailure);
}

TArray<FItemPrice_BE> USequenceWallet::buildItemUpdateListFromJson(FString json)
{
	UE_LOG(LogTemp, Error, TEXT("Received json: %s"), *json);
	TSharedPtr<FJsonObject> jsonObj;
	FUpdatedPriceReturn updatedPrices;

	if (FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(json), jsonObj))
	{
		if (FJsonObjectConverter::JsonObjectToUStruct<FUpdatedPriceReturn>(jsonObj.ToSharedRef(), &updatedPrices))
		{
			return updatedPrices.tokenPrices;
		}
	}
	else
	{//failure
		UE_LOG(LogTemp, Error, TEXT("Failed to convert String: %s to Json object"), *json);
	}
	TArray<FItemPrice_BE> updatedItems;
	return updatedItems;
}

void USequenceWallet::getUpdatedCoinPrice(FID_BE itemToUpdate, TSuccessCallback<TArray<FItemPrice_BE>> OnSuccess, FFailureCallback OnFailure)
{
	TArray<FID_BE> items;
	items.Add(itemToUpdate);
	getUpdatedCoinPrices(items, OnSuccess, OnFailure);
}

void USequenceWallet::getUpdatedCoinPrices(TArray<FID_BE> itemsToUpdate, TSuccessCallback<TArray<FItemPrice_BE>> OnSuccess, FFailureCallback OnFailure)
{
	FString args = "{\"tokens\":";
	FString jsonObjString = "";
	TArray<FString> parsedItems;
	for (FID_BE item : itemsToUpdate)
	{
		if (FJsonObjectConverter::UStructToJsonObjectString<FID_BE>(item, jsonObjString))
			parsedItems.Add(jsonObjString);
	}
	args += UIndexerSupport::stringListToSimpleString(parsedItems);
	args += "}";

	SendRPC(getSequenceURL("getCoinPrices"), args, [=](FString Content)
		{
			OnSuccess(buildItemUpdateListFromJson(Content));
		}, OnFailure);
}

void USequenceWallet::getUpdatedCollectiblePrice(FID_BE itemToUpdate, TSuccessCallback<TArray<FItemPrice_BE>> OnSuccess, FFailureCallback OnFailure)
{
	TArray<FID_BE> items;
	items.Add(itemToUpdate);
	getUpdatedCollectiblePrices(items, OnSuccess, OnFailure);
}

void USequenceWallet::getUpdatedCollectiblePrices(TArray<FID_BE> itemsToUpdate, TSuccessCallback<TArray<FItemPrice_BE>> OnSuccess, FFailureCallback OnFailure)
{
	FString args = "{\"tokens\":";
	FString jsonObjString = "";
	TArray<FString> parsedItems;
	for (FID_BE item : itemsToUpdate)
	{
		if (FJsonObjectConverter::UStructToJsonObjectString<FID_BE>(item, jsonObjString))
			parsedItems.Add(jsonObjString);
	}
	args += UIndexerSupport::stringListToSimpleString(parsedItems);
	args += "}";

	SendRPC(getSequenceURL("getCollectiblePrices"), args, [=](FString Content)
		{
			OnSuccess(buildItemUpdateListFromJson(Content));
		}, OnFailure);
}

FString USequenceWallet::buildQR_Request_URL(FString walletAddress,int32 size)
{
	FString urlSize = "/";
	urlSize.AppendInt(size);

	return sequenceURL_QR + encodeB64_URL(walletAddress) + urlSize;
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

void USequenceWallet::Ping(int64 chainID, TSuccessCallback<bool> OnSuccess, FFailureCallback OnFailure)
{
	if (this->Indexer)
		this->Indexer->Ping(chainID, OnSuccess, OnFailure);
}

void USequenceWallet::Version(int64 chainID, TSuccessCallback<FVersion> OnSuccess, FFailureCallback OnFailure)
{
	if (this->Indexer)
		this->Indexer->Version(chainID,OnSuccess,OnFailure);
}

void USequenceWallet::RunTimeStatus(int64 chainID, TSuccessCallback<FRuntimeStatus> OnSuccess, FFailureCallback OnFailure)
{
	if (this->Indexer)
		this->Indexer->RunTimeStatus(chainID, OnSuccess, OnFailure);
}

void USequenceWallet::GetChainID(int64 chainID, TSuccessCallback<int64> OnSuccess, FFailureCallback OnFailure)
{
	if (this->Indexer)
		this->Indexer->GetChainID(chainID, OnSuccess, OnFailure);
}

void USequenceWallet::GetEtherBalance(int64 chainID, FString accountAddr, TSuccessCallback<FEtherBalance> OnSuccess, FFailureCallback OnFailure)
{
	if (this->Indexer)
		this->Indexer->GetEtherBalance(chainID, accountAddr, OnSuccess, OnFailure);
}

void USequenceWallet::GetTokenBalances(int64 chainID, FGetTokenBalancesArgs args, TSuccessCallback<FGetTokenBalancesReturn> OnSuccess, FFailureCallback OnFailure)
{
	if (this->Indexer)
		this->Indexer->GetTokenBalances(chainID, args, OnSuccess, OnFailure);
}

void USequenceWallet::GetTokenSupplies(int64 chainID, FGetTokenSuppliesArgs args, TSuccessCallback<FGetTokenSuppliesReturn> OnSuccess, FFailureCallback OnFailure)
{
	if (this->Indexer)
		this->Indexer->GetTokenSupplies(chainID, args, OnSuccess, OnFailure);
}

void USequenceWallet::GetTokenSuppliesMap(int64 chainID, FGetTokenSuppliesMapArgs args, TSuccessCallback<FGetTokenSuppliesMapReturn> OnSuccess, FFailureCallback OnFailure)
{
	if (this->Indexer)
		this->Indexer->GetTokenSuppliesMap(chainID, args, OnSuccess, OnFailure);
}

void USequenceWallet::GetBalanceUpdates(int64 chainID, FGetBalanceUpdatesArgs args, TSuccessCallback<FGetBalanceUpdatesReturn> OnSuccess, FFailureCallback OnFailure)
{
	if (this->Indexer)
		this->Indexer->GetBalanceUpdates(chainID, args, OnSuccess, OnFailure);
}

void USequenceWallet::GetTransactionHistory(int64 chainID, FGetTransactionHistoryArgs args, TSuccessCallback<FGetTransactionHistoryReturn> OnSuccess, FFailureCallback OnFailure)
{
	if (this->Indexer)
		this->Indexer->GetTransactionHistory(chainID, args, OnSuccess, OnFailure);
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

void USequenceWallet::NonViewCall(FEthTransaction transaction, FPrivateKey PrivateKey, int ChainID, TSuccessCallback<FUnsizedData> OnSuccess, FFailureCallback OnFailure)
{
	Provider(this->ProviderUrl).NonViewCall(transaction, PrivateKey, ChainID, OnSuccess, OnFailure);
}