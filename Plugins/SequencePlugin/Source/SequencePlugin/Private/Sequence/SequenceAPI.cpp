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
					const FString Creds = UIndexerSupport::structToString(this->Credentials);
					UE_LOG(LogTemp,Display,TEXT("Creds: %s"),*Creds);
					OnSuccess("Session Registered");
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

	if (this->Credentials.IsRegistered())
	{
		if (this->Credentials.Valid())
		{
			const FString URL = this->Credentials.GetRPCServer() + "/rpc/WaasAuthenticator/RegisterSession";
			this->SequenceRPC(URL,this->BuildRegisterSessionIntent(),OnResponse,OnFailure);
		}
		else
		{
			OnFailure(FSequenceError(RequestFail, "[Invalid Credentials please login first]"));
		}
	}
	else
	{
		const FString URL = this->Credentials.GetRPCServer() + "/rpc/WaasAuthenticator/RegisterSession";
		this->SequenceRPC(URL,this->BuildRegisterSessionIntent(),OnResponse,OnFailure);
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
			const FString ParsedResponse = UIndexerSupport::structToString(ResponseStruct);
			UE_LOG(LogTemp,Display,TEXT("Response: %s"), *ParsedResponse);
			OnSuccess(ResponseStruct.response.data);
		}
		else
		{
			OnFailure(FSequenceError(RequestFail, "2nd Level Parsing: Request failed: " + Response));
		}
	};
	
	if (this->Credentials.IsRegistered() && this->Credentials.Valid())
	{
		const FString URL = this->Credentials.GetRPCServer() + "/rpc/WaasAuthenticator/SendIntent";
		this->SequenceRPC(URL,this->BuildListSessionIntent(),OnResponse,OnFailure);
	}
	else
		OnFailure(FSequenceError(RequestFail, "[Session Not Registered Please Register Session First]"));
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
			const FString ParsedResponse = UIndexerSupport::structToString(ResponseStruct);
			UE_LOG(LogTemp,Display,TEXT("Response: %s"), *ParsedResponse);
			if (ResponseStruct.response.code.Compare("sessionClosed")==0)
			{
				this->Credentials.UnRegisterCredentials();
				const UAuthenticator * TUAuth = NewObject<UAuthenticator>();
				TUAuth->StoreCredentials(this->Credentials);
				OnSuccess(ResponseStruct.response.code);
			}
			else
				OnFailure(FSequenceError(RequestFail, "2nd Level Parsing: Request failed: " + Response));
		}
		else
		{
			OnFailure(FSequenceError(RequestFail, "2nd Level Parsing: Request failed: " + Response));
		}
	};
	if (this->Credentials.IsRegistered() && this->Credentials.Valid())
	{
		const FString URL = this->Credentials.GetRPCServer() + "/rpc/WaasAuthenticator/SendIntent";
		this->SequenceRPC(URL,this->BuildCloseSessionIntent(),OnResponse,OnFailure);
	}
	else
		OnFailure(FSequenceError(RequestFail, "[Session Not Registered Please Register Session First]"));
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
			const FString ParsedResponse = UIndexerSupport::structToString(Msg);
			UE_LOG(LogTemp,Display,TEXT("Response: %s"), *ParsedResponse);
			OnSuccess(Msg.response);
		}
		else
		{
			OnFailure(FSequenceError(RequestFail, "Malformed Response: " + Response));
		}
		
	};
	if (this->Credentials.IsRegistered() && this->Credentials.Valid())
	{
		const FString URL = this->Credentials.GetRPCServer() + "/rpc/WaasAuthenticator/SendIntent";
		this->SequenceRPC(URL,this->BuildSignMessageIntent(Message),OnResponse,OnFailure);
	}
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
	TSuccessCallback<FString> OnResponse = [=](FString Response)
	{
#if PLATFORM_ANDROID
		NativeOAuth::AndroidLog(Response);
#endif
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
	{
		const FString URL = this->Credentials.GetRPCServer() + "/rpc/WaasAuthenticator/SendIntent";
		this->SequenceRPC(URL, BuildSendTransactionIntent(TransactionsPayload), OnResponse, OnFailure);
	}
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
	const FString Data = "{\"message\":\""+EIP_Message+"\",\"network\":\""+this->Credentials.GetNetworkString()+"\",\"wallet\":\""+this->Credentials.GetWalletAddress()+"\"}";
	const FString SigIntent = "{\"data\":"+Data+",\"expiresAt\":"+expiresString+",\"issuedAt\":"+issuedString+",\"name\":\"signMessage\",\"version\":\""+this->Credentials.GetWaasVersin()+"\"}";
	const FString Signature = this->GeneratePacketSignature(SigIntent);
	const FString Intent = "{\"intent\":{\"data\":"+Data+",\"expiresAt\":"+expiresString+",\"issuedAt\":"+issuedString+",\"name\":\"signMessage\",\"signatures\":[{\"sessionId\":\""+this->Credentials.GetSessionId()+"\",\"signature\":\""+Signature+"\"}],\"version\":\""+this->Credentials.GetWaasVersin()+"\"}}";
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
	const FString SigIntent = "{\"data\":{\"identifier\":\""+identifier+"\",\"network\":\""+this->Credentials.GetNetworkString()+"\",\"transactions\":"+Txns+",\"wallet\":\""+this->Credentials.GetWalletAddress()+"\"},\"expiresAt\":"+expiresString+",\"issuedAt\":"+issuedString+",\"name\":\"sendTransaction\",\"version\":\""+this->Credentials.GetWaasVersin()+"\"}";
	const FString Signature = this->GeneratePacketSignature(SigIntent);
	FString Intent = "{\"intent\":{\"data\":{\"identifier\":\""+identifier+"\",\"network\":\""+this->Credentials.GetNetworkString()+"\",\"transactions\":"+Txns+",\"wallet\":\""+this->Credentials.GetWalletAddress()+"\"},\"expiresAt\":"+expiresString+",\"issuedAt\":"+issuedString+",\"name\":\"sendTransaction\",\"signatures\":[{\"sessionId\":\""+this->Credentials.GetSessionId()+"\",\"signature\":\""+Signature+"\"}],\"version\":\""+this->Credentials.GetWaasVersin()+"\"}}";
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
	const FString SigIntent = "{\"data\":"+Data+",\"expiresAt\":"+expiresString+",\"issuedAt\":"+issuedString+",\"name\":\"openSession\",\"version\":\""+this->Credentials.GetWaasVersin()+"\"}";
	const FString Signature = this->GeneratePacketSignature(SigIntent);
	const FString Intent = "{\"intent\":{\"data\":"+Data+",\"expiresAt\":"+expiresString+",\"issuedAt\":"+issuedString+",\"name\":\"openSession\",\"signatures\":[{\"sessionId\":\""+this->Credentials.GetSessionId()+"\",\"signature\":\""+Signature+"\"}],\"version\":\""+this->Credentials.GetWaasVersin()+"\"},\"friendlyName\":\""+GUID+"\"}";
	UE_LOG(LogTemp,Display,TEXT("RegisterSession Intent: %s"),*Intent);
	return Intent;
}

FString USequenceWallet::BuildListSessionIntent()
{
	const int64 issued = FDateTime::UtcNow().ToUnixTimestamp() - 30;
	const int64 expires = issued + 86400;
	const FString issuedString = FString::Printf(TEXT("%lld"),issued);
	const FString expiresString = FString::Printf(TEXT("%lld"),expires);
	const FString SigIntent = "{\"data\":{\"wallet\":\""+this->Credentials.GetWalletAddress()+"\"},\"expiresAt\":"+expiresString+",\"issuedAt\":"+issuedString+",\"name\":\"listSessions\",\"version\":\""+this->Credentials.GetWaasVersin()+"\"}";
	const FString Signature = this->GeneratePacketSignature(SigIntent);
	const FString Intent = "{\"intent\":{\"data\":{\"wallet\":\""+this->Credentials.GetWalletAddress()+"\"},\"expiresAt\":"+expiresString+",\"issuedAt\":"+issuedString+",\"name\":\"listSessions\",\"signatures\":[{\"sessionId\":\""+this->Credentials.GetSessionId()+"\",\"signature\":\""+Signature+"\"}],\"version\":\""+this->Credentials.GetWaasVersin()+"\"}}";
	UE_LOG(LogTemp,Display,TEXT("ListSessionIntent: %s"),*Intent);
	return Intent;
}

FString USequenceWallet::BuildCloseSessionIntent()
{
	const int64 issued = FDateTime::UtcNow().ToUnixTimestamp() - 30;
	const int64 expires = issued + 86400;
	const FString issuedString = FString::Printf(TEXT("%lld"),issued);
	const FString expiresString = FString::Printf(TEXT("%lld"),expires);
	const FString SigIntent = "{\"data\":{\"sessionId\":\""+this->Credentials.GetSessionId()+"\"},\"expiresAt\":"+expiresString+",\"issuedAt\":"+issuedString+",\"name\":\"closeSession\",\"version\":\""+this->Credentials.GetWaasVersin()+"\"}";
	const FString Signature = this->GeneratePacketSignature(SigIntent);
	const FString Intent = "{\"intent\":{\"data\":{\"sessionId\":\""+this->Credentials.GetSessionId()+"\"},\"expiresAt\":"+expiresString+",\"issuedAt\":"+issuedString+",\"name\":\"closeSession\",\"signatures\":[{\"sessionId\":\""+this->Credentials.GetSessionId()+"\",\"signature\":\""+Signature+"\"}],\"version\":\""+this->Credentials.GetWaasVersin()+"\"}}";
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

/*
* Gets the friend data from the given username!
* This function appears to require some form of authentication (perhaps all of the sequence api does)
* @Deprecated
*/
void USequenceWallet::getFriends(FString username, TSuccessCallback<TArray<FContact_BE>> OnSuccess, FFailureCallback OnFailure)
{
	FString json_arg = "{}";
	
	SendRPC(getSequenceURL("friendList"), json_arg, [this,OnSuccess](FString Content)
		{
			OnSuccess(this->buildFriendListFromJson(Content));
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

	SendRPC(getSequenceURL("getCoinPrices"), args, [this,OnSuccess](FString Content)
		{
			OnSuccess(this->buildItemUpdateListFromJson(Content));
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

	SendRPC(getSequenceURL("getCollectiblePrices"), args, [this,OnSuccess](FString Content)
		{
			OnSuccess(this->buildItemUpdateListFromJson(Content));
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

void USequenceWallet::NonViewCall(FEthTransaction transaction, FPrivateKey PrivateKey, int ChainID, TSuccessCallback<FUnsizedData> OnSuccess, FFailureCallback OnFailure)
{
	Provider(this->ProviderUrl).NonViewCall(transaction, PrivateKey, ChainID, OnSuccess, OnFailure);
}