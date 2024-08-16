// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#include "Sequence/SequenceAPI.h"
#include "RequestHandler.h"
#include "Util/SequenceSupport.h"
#include "Dom/JsonObject.h"
#include "JsonObjectConverter.h"
#include "Eth/Crypto.h"
#include "Kismet/GameplayStatics.h"
#include "Types/ContractCall.h"
#include "Misc/Base64.h"
#include "Native/NativeOAuth.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Indexer/Indexer.h"
#include "Util/JsonBuilder.h"
#include "Provider.h"
#include "Transak.h"
#include "SequenceRPCManager.h"

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
	this->SequenceRPCManager = nullptr;
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

TArray<FIdNamePair> USequenceWallet::GetAllNetworks()
{
	return USequenceSupport::GetAllNetworks();
}

TArray<FString> USequenceWallet::GetAllNetworkNames()
{
	return USequenceSupport::GetAllNetworkNames();
}

TArray<int64> USequenceWallet::GetAllNetworkIds()
{
	return USequenceSupport::GetAllNetworkIds();
}

FString USequenceWallet::GetNetworkName(const int64 NetworkIdIn)
{
	return USequenceSupport::GetNetworkName(NetworkIdIn);
}

int64 USequenceWallet::GetNetworkId(const FString& NetworkNameIn)
{
	return USequenceSupport::GetNetworkId(NetworkNameIn);
}

int64 USequenceWallet::GetSystemReadableAmount(const float AmountIn, const int64 DecimalsIn)
{
	return USequenceSupport::GetSystemReadableAmount(AmountIn, DecimalsIn);
}

float USequenceWallet::GetUserReadableAmount(const int64 AmountIn, const int64 DecimalsIn)
{
	return USequenceSupport::GetUserReadableAmount(AmountIn, DecimalsIn);
}

void USequenceWallet::Init(const FCredentials_BE& CredentialsIn)
{
	this->Credentials = CredentialsIn;
	this->Indexer = NewObject<UIndexer>();
	this->SequenceRPCManager = USequenceRPCManager::Make(this->Credentials.GetSessionWallet());
	if (!this->Provider)
	{
		this->Provider = UProvider::Make("");
	}
}

void USequenceWallet::Init(const FCredentials_BE& CredentialsIn,const FString& ProviderURL)
{
	this->Credentials = CredentialsIn;
	this->Indexer = NewObject<UIndexer>();
	this->SequenceRPCManager = USequenceRPCManager::Make(this->Credentials.GetSessionWallet());
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
	/*const TSuccessCallback<FString> OnResponse = [this,OnSuccess,OnFailure](const FString& Response)
	{
		const TSharedPtr<FJsonObject> Json = UIndexerSupport::JsonStringToObject(Response);
		const TSharedPtr<FJsonObject> * SessionObj = nullptr;
		const TSharedPtr<FJsonObject> * ResponseObj = nullptr;
		
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
	}*/
}

void USequenceWallet::ListSessions(const TSuccessCallback<TArray<FSeqListSessions_Session>>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->SequenceRPCManager)
	{
		this->SequenceRPCManager->ListSessions(this->Credentials,OnSuccess,OnFailure);
	}
}

void USequenceWallet::SignOut() const
{
	const UAuthenticator * Auth = NewObject<UAuthenticator>();
	if (this->Credentials.IsRegistered())
	{
		const TFunction<void()> OnSuccess = [Auth]
		{
			Auth->ClearStoredCredentials();
		};

		const TFunction<void (FSequenceError)> OnFailure = [Auth](FSequenceError Err)
		{
			Auth->ClearStoredCredentials();
		};

		if (this->SequenceRPCManager)
		{
			this->SequenceRPCManager->CloseSession(this->Credentials, OnSuccess, OnFailure);
		}
		else
		{
			Auth->ClearStoredCredentials();
		}
	}
	else
	{
		Auth->ClearStoredCredentials();
	}
}

void USequenceWallet::GetSupportedTransakCountries(const TSuccessCallback<TArray<FSupportedCountry>>& OnSuccess, const FFailureCallback& OnFailure)
{
	const UTransakOnRamp * Transak = NewObject<UTransakOnRamp>();
	Transak->GetSupportedCountries(OnSuccess,OnFailure);
}

void USequenceWallet::OpenTransakLink(const FString& FiatCurrency, const FString& DefaultAmount, const FString& DefaultCryptoCurrency, const FString& Networks, bool DisableWalletAddressForm)
{
	if (Credentials.RegisteredValid())
	{
		UTransakOnRamp * Transak = UTransakOnRamp::Init(this->GetWalletAddress());
		Transak->OpenTransakLink(FiatCurrency, DefaultAmount, DefaultCryptoCurrency, Networks, DisableWalletAddressForm);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Please login first."));
	}
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

void USequenceWallet::SignMessage(const FString& Message, const TSuccessCallback<FSeqSignMessageResponse_Response>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->SequenceRPCManager)
	{
		this->SequenceRPCManager->SignMessage(this->Credentials,Message,OnSuccess,OnFailure);
	}
}

void USequenceWallet::SendTransactionWithFeeOption(const TArray<TransactionUnion>& Transactions, const FFeeOption& FeeOption, const TSuccessCallback<FSeqTransactionResponse_Data>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->SequenceRPCManager)
	{
		this->SequenceRPCManager->SendTransactionWithFeeOption(this->Credentials,Transactions,FeeOption,OnSuccess,OnFailure);
	}
}

TArray<FFeeOption> USequenceWallet::BalancesListToFeeOptionList(const TArray<FSeqTokenBalance>& BalanceList)
{
	TArray<FFeeOption> ParsedBalances;

	for (auto Balance : BalanceList)
	{
		ParsedBalances.Add(FFeeOption(Balance));
	}
	return ParsedBalances;
}

TArray<FFeeOption> USequenceWallet::MarkValidFeeOptions(TArray<FFeeOption> FeeOptions, TArray<FFeeOption> BalanceOptions)
{
	for (int i = 0; i < FeeOptions.Num(); i++)
	{
		for (int j = 0; j < BalanceOptions.Num(); j++)
		{
			FeeOptions[i].bCanAfford |= BalanceOptions[j].CanAfford(FeeOptions[i]);
		}
	}
	return FeeOptions;
}

TArray<FFeeOption> USequenceWallet::FindValidFeeOptions(const TArray<FFeeOption>& FeeOptions, const TArray<FFeeOption>& BalanceOptions)
{
	TArray<FFeeOption> ValidFeeOptions;

	for (auto BalanceOption : BalanceOptions)
	{
		for (auto FeeOption : FeeOptions)
		{
			if (BalanceOption.CanAfford(FeeOption))
			{
				FeeOption.bCanAfford = true;
				ValidFeeOptions.Add(FeeOption);
			}
		}
	}
	
	return ValidFeeOptions;
}

void USequenceWallet::GetFeeOptions(const TArray<TransactionUnion>& Transactions, const TSuccessCallback<TArray<FFeeOption>>& OnSuccess, const FFailureCallback& OnFailure) const
{
	const TSuccessCallback<TArray<FFeeOption>> OnResponse = [this, OnSuccess, OnFailure](const TArray<FFeeOption>& Fees)
	{
		FSeqGetTokenBalancesArgs Args;
		Args.accountAddress = this->GetWalletAddress();
		Args.includeMetaData = true;

		const TSuccessCallback<FSeqGetTokenBalancesReturn> BalanceSuccess = [this, Fees, OnSuccess, OnFailure] (const FSeqGetTokenBalancesReturn& BalanceResponse)
		{
			TArray<FSeqTokenBalance> PreProcBalances = BalanceResponse.balances;
			
			const TSuccessCallback<FSeqEtherBalance> EtherSuccess = [this, PreProcBalances, Fees, OnSuccess] (const FSeqEtherBalance& EtherBalance)
			{
				TArray<FFeeOption> Balances = this->BalancesListToFeeOptionList(PreProcBalances);
				Balances.Add(FFeeOption(EtherBalance));
				OnSuccess(this->FindValidFeeOptions(Fees,Balances));
			};

			const FFailureCallback EtherFailure = [this,OnFailure] (const FSequenceError& Err)
			{
				OnFailure(FSequenceError(RequestFail, "Failed to Get EtherBalance from Indexer"));
			};

			this->GetEtherBalance(this->GetWalletAddress(),EtherSuccess,EtherFailure);
		};

		const FFailureCallback BalanceFailure = [this, OnFailure](const FSequenceError& Err)
		{
			OnFailure(FSequenceError(RequestFail, "Failed to Get Balances from Indexer"));
		};
						
		this->GetTokenBalances(Args,BalanceSuccess,BalanceFailure);
	};

	if (this->SequenceRPCManager)
	{
		this->SequenceRPCManager->GetFeeOptions(this->Credentials,Transactions,OnResponse,OnFailure);
	}
}

void USequenceWallet::GetUnfilteredFeeOptions(const TArray<TransactionUnion>& Transactions, const TSuccessCallback<TArray<FFeeOption>>& OnSuccess, const FFailureCallback& OnFailure) const
{	
	const TSuccessCallback<TArray<FFeeOption>> OnResponse = [this, OnSuccess, OnFailure](const TArray<FFeeOption>& Fees)
	{
		const TSuccessCallback<FSeqGetTokenBalancesReturn> BalanceSuccess = [this, Fees, OnSuccess, OnFailure] (const FSeqGetTokenBalancesReturn& BalanceResponse)
		{
			TArray<FSeqTokenBalance> PreProcBalances = BalanceResponse.balances;
			const TSuccessCallback<FSeqEtherBalance> EtherSuccess = [this, PreProcBalances, Fees, OnSuccess] (const FSeqEtherBalance& EtherBalance)
			{
				TArray<FFeeOption> Balances = this->BalancesListToFeeOptionList(PreProcBalances);
				Balances.Add(FFeeOption(EtherBalance));
				OnSuccess(this->MarkValidFeeOptions(Fees,Balances));
			};

			const FFailureCallback EtherFailure = [this,OnFailure] (const FSequenceError& Err)
			{
				OnFailure(FSequenceError(RequestFail, "Failed to Get EtherBalance from Indexer"));
			};

			this->GetEtherBalance(this->GetWalletAddress(),EtherSuccess,EtherFailure);
		};

		const FFailureCallback BalanceFailure = [this, OnFailure](const FSequenceError& Err)
		{
			OnFailure(FSequenceError(RequestFail, "Failed to Get Balances from Indexer"));
		};

		FSeqGetTokenBalancesArgs Args;
		Args.accountAddress = this->GetWalletAddress();
		Args.includeMetaData = true;
		
		this->GetTokenBalances(Args,BalanceSuccess,BalanceFailure);
	};

	if (this->SequenceRPCManager)
	{
		this->SequenceRPCManager->GetFeeOptions(this->Credentials,Transactions,OnResponse,OnFailure);
	}
}

void USequenceWallet::SendTransaction(const TArray<TransactionUnion>& Transactions, const TSuccessCallback<FSeqTransactionResponse_Data>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->SequenceRPCManager)
	{
		this->SequenceRPCManager->SendTransaction(this->Credentials, Transactions, OnSuccess, OnFailure);
	}
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
	args += USequenceSupport::StringListToSimpleString(parsedItems);
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
	args += USequenceSupport::StringListToSimpleString(parsedItems);
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

void USequenceWallet::Version(const TSuccessCallback<FSeqVersion>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Indexer)
		this->Indexer->Version(this->Credentials.GetNetwork(),OnSuccess,OnFailure);
}

void USequenceWallet::RunTimeStatus(const TSuccessCallback<FSeqRuntimeStatus>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Indexer)
		this->Indexer->RunTimeStatus(this->Credentials.GetNetwork(), OnSuccess, OnFailure);
}

void USequenceWallet::GetChainID(const TSuccessCallback<int64>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Indexer)
		this->Indexer->GetChainID(this->Credentials.GetNetwork(), OnSuccess, OnFailure);
}

void USequenceWallet::GetEtherBalance(const FString& AccountAddr, const TSuccessCallback<FSeqEtherBalance>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Indexer)
		this->Indexer->GetEtherBalance(this->Credentials.GetNetwork(), AccountAddr, OnSuccess, OnFailure);
}

void USequenceWallet::GetTokenBalances(const FSeqGetTokenBalancesArgs& Args, const TSuccessCallback<FSeqGetTokenBalancesReturn>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Indexer)
		this->Indexer->GetTokenBalances(this->Credentials.GetNetwork(), Args, OnSuccess, OnFailure);
}

void USequenceWallet::GetTokenSupplies(const FSeqGetTokenSuppliesArgs& Args, const TSuccessCallback<FSeqGetTokenSuppliesReturn>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Indexer)
		this->Indexer->GetTokenSupplies(this->Credentials.GetNetwork(), Args, OnSuccess, OnFailure);
}

void USequenceWallet::GetTokenSuppliesMap(const FSeqGetTokenSuppliesMapArgs& Args, const TSuccessCallback<FSeqGetTokenSuppliesMapReturn>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Indexer)
		this->Indexer->GetTokenSuppliesMap(this->Credentials.GetNetwork(), Args, OnSuccess, OnFailure);
}

void USequenceWallet::GetBalanceUpdates(const FSeqGetBalanceUpdatesArgs& Args, const TSuccessCallback<FSeqGetBalanceUpdatesReturn>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->Indexer)
		this->Indexer->GetBalanceUpdates(this->Credentials.GetNetwork(), Args, OnSuccess, OnFailure);
}

void USequenceWallet::GetTransactionHistory(const FSeqGetTransactionHistoryArgs& Args, const TSuccessCallback<FSeqGetTransactionHistoryReturn>& OnSuccess, const FFailureCallback& OnFailure) const
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