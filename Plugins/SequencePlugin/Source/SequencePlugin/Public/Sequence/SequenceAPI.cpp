#include "SequenceAPI.h"

#include "Http.h"
#include "HttpManager.h"
#include "RequestHandler.h"

FString SequenceAPI::SortOrderToString(ESortOrder SortOrder)
{
	switch(SortOrder)
	{
	case SequenceAPI::ASC:
		return "ASC";
	case SequenceAPI::DESC:
		return "DESC";
	default: return "";
	}
}

SequenceAPI::ESortOrder SequenceAPI::StringToSortOrder(FString String)
{
	return String == "ASC" ? ASC : DESC;
}

FString SequenceAPI::FSortBy::ToJson()
{
	return FJsonBuilder().ToPtr()
		->AddString("column", Column)
		->AddString("order", SortOrderToString(Order))
		->ToString();
}

SequenceAPI::FSortBy SequenceAPI::FSortBy::From(TSharedPtr<FJsonObject> Json)
{
	FSortBy Sort = FSortBy{};
	Sort.Column = Json->GetStringField("column");
	Sort.Order = StringToSortOrder(Json->GetStringField("order"));
	return Sort;
}

FString SequenceAPI::FPage::ToJson()
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
			Array.AddValue(SortBy.ToJson());
		}
	}
	
	return Json.ToString();
}

SequenceAPI::FPage SequenceAPI::FPage::From(TSharedPtr<FJsonObject> Json)
{
	FPage page = FPage{};

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
			Array.Push(FSortBy::From(JsonVal->AsObject()));
		}
		
		page.Sort = Array;
	}

	return page;
}

FString SequenceAPI::FTransaction::ToJson()
{
	FJsonBuilder Json = FJsonBuilder();

	Json.AddInt("chainId", ChainId);
	Json.AddString("from", "0x" + From.ToHex());
	Json.AddString("to", "0x" + From.ToHex());

	if(this->Value.IsSet()) Json.AddString("value", this->Value.GetValue());

	return Json.ToString();
}

SequenceAPI::FPartnerWallet SequenceAPI::FPartnerWallet::From(TSharedPtr<FJsonObject> Json)
{
	return FPartnerWallet{
		static_cast<uint64>(Json->GetIntegerField("number")),
		static_cast<uint64>(Json->GetIntegerField("partnerId")),
		static_cast<uint64>(Json->GetIntegerField("walletIndex")),
		Json->GetStringField("walletAddress")
	};
}

FString SequenceAPI::FSequenceWallet::Url(const FString Name) const
{
	return this->Hostname + this->Path + Name;
}

void SequenceAPI::FSequenceWallet::SendRPC(FString Url, FString Content, TSuccessCallback<FString> OnSuccess,
	FFailureCallback OnFailure)
{
	NewObject<URequestHandler>()
			->PrepareRequest()
			->WithUrl(Url)
			->WithHeader("Content-type", "application/json")
			->WithHeader("Authorization", AuthToken)
			->WithVerb("POST")
			->WithContentAsString(Content)
			->ProcessAndThen(OnSuccess, OnFailure);
}

SequenceAPI::FSequenceWallet::FSequenceWallet(FString Hostname) : Hostname(Hostname)
{
	
}

SequenceAPI::FSequenceWallet::FSequenceWallet()
{
}

void SequenceAPI::FSequenceWallet::CreateWallet(uint64 AccountIndex, TSuccessCallback<FAddress> OnSuccess,
                                                FFailureCallback OnFailure)
{
	TFunction<TResult<FAddress> (FString)> ExtractAddress = [=](FString Content)
	{
		
		TSharedPtr<FJsonObject> Json = Parse(Content);
		TResult<FAddress> Retval = MakeValue(FAddress{});

		if(!Json)
		{
			Retval = MakeError(SequenceError{RequestFail, "Json did not parse"});
		}
		else
		{
			const FString AddressString = Json->GetStringField("address");
			Retval = MakeValue(FAddress::From(AddressString));
		}
		
		
		return Retval;
	};

	FString Content = FJsonBuilder().ToPtr()
		->AddInt("accountIndex", AccountIndex)
		->ToString();
	
	this->SendRPCAndExtract(Url("CreateWallet"), Content, OnSuccess, ExtractAddress,
	OnFailure);
}

void SequenceAPI::FSequenceWallet::GetWalletAddress(uint64 AccountIndex, TSuccessCallback<FAddress> OnSuccess,
	FFailureCallback OnFailure)
{
	TFunction<TResult<FAddress> (FString)> ExtractAddress = [=](FString Content)
	{
		
		TSharedPtr<FJsonObject> Json = Parse(Content);
		TResult<FAddress> Retval = MakeValue(FAddress{});

		if(!Json)
		{
			Retval = MakeError(SequenceError{RequestFail, "Json did not parse"});
		}
		else
		{
			const FString AddressString = Json->GetStringField("address");
			Retval = MakeValue(FAddress::From(AddressString));
		}
		
		
		return Retval;
	};

	FString Content = FJsonBuilder().ToPtr()
		->AddInt("accountIndex", AccountIndex)
		->ToString();
	
	this->SendRPCAndExtract(Url("GetWalletAddress"), Content, OnSuccess, ExtractAddress,
	OnFailure);
}

void SequenceAPI::FSequenceWallet::DeployWallet(uint64 ChainId, uint64 AccountIndex,
	TSuccessCallback<FDeployWalletReturn> OnSuccess, FFailureCallback OnFailure)
{
	TFunction<TResult<FDeployWalletReturn> (FString)> ExtractDeployWalletReturn = [=](FString Content)
	{
		
		TSharedPtr<FJsonObject> Json = Parse(Content);
		TResult<FDeployWalletReturn> Retval = MakeValue(FDeployWalletReturn{"", ""});

		if(!Json)
		{
			Retval = MakeError(SequenceError{RequestFail, "Json did not parse"});
		}
		else
		{
			const FString AddressString = Json->GetStringField("address");
			const FString TransactionHashString = Json->GetStringField("txnHash");
			Retval = MakeValue(FDeployWalletReturn{AddressString, TransactionHashString});
		}
		
		return Retval;
	};

	FString Content = FJsonBuilder().ToPtr()
		->AddInt("chainId", ChainId)
		->AddInt("accountIndex", AccountIndex)
		->ToString();
	
	this->SendRPCAndExtract(Url("DeployWallet"), Content, OnSuccess, ExtractDeployWalletReturn,
	OnFailure);
}

void SequenceAPI::FSequenceWallet::Wallets(FPage Page, TSuccessCallback<FWalletsReturn> OnSuccess,
	FFailureCallback OnFailure)
{
	const TFunction<TResult<FWalletsReturn> (FString)> ExtractWallets = [=](FString Content)
	{
		
		TSharedPtr<FJsonObject> Json = Parse(Content);
		TResult<FWalletsReturn> ReturnVal = MakeValue(FWalletsReturn{});

		if(!Json)
		{
			ReturnVal = MakeError(SequenceError{RequestFail, "Json did not parse"});
		}
		else
		{
			TArray<TSharedPtr<FJsonValue>> WalletsJsonArray = Json->GetArrayField("wallets");
			const TSharedPtr<FJsonObject> PageJson = Json->GetObjectField("page");
			TArray<FPartnerWallet> Wallets;

			for(const TSharedPtr<FJsonValue> WalletJson : WalletsJsonArray)
			{
				Wallets.Push(FPartnerWallet::From(WalletJson->AsObject()));
			}
			
			ReturnVal = MakeValue(FWalletsReturn{Wallets, FPage::From(PageJson)});
		}
		
		return ReturnVal;
	};

	const FString Content = FJsonBuilder().ToPtr()
          ->AddField("page", Page.ToJson())
          ->ToString();
	
	this->SendRPCAndExtract(Url("Wallets"), Content, OnSuccess, ExtractWallets,
	OnFailure); 
}

void SequenceAPI::FSequenceWallet::Wallets(TSuccessCallback<FWalletsReturn> OnSuccess, FFailureCallback OnFailure)
{
	this->Wallets(FPage{}, OnSuccess, OnFailure);
}

void SequenceAPI::FSequenceWallet::SignMessage(uint64 ChainId, FAddress AccountAddress, FUnsizedData Message,
                                               const TSuccessCallback<FSignature> OnSuccess, const FFailureCallback OnFailure)
{
	const TFunction<TResult<FSignature> (FString)> ExtractSignature = [=](FString Content)
	{
		
		TSharedPtr<FJsonObject> Json = Parse(Content);
		TResult<FSignature> ReturnVal = MakeValue(FUnsizedData::Empty());

		if(!Json)
		{
			ReturnVal = MakeError(SequenceError{RequestFail, "Json did not parse"});
		}
		else
		{
			const FString SignatureString = Json->GetStringField("signature");
			ReturnVal = MakeValue(HexStringToBinary(SignatureString));
		}
		
		return ReturnVal;
	};

	const FString Content = FJsonBuilder().ToPtr()
          ->AddInt("chainId", ChainId)
          ->AddString("accountAddress", "0x" + AccountAddress.ToHex())
          ->AddString("message", "0x" +Message.ToHex())
          ->ToString();
	
	this->SendRPCAndExtract(Url("SignMessage"), Content, OnSuccess, ExtractSignature,
	OnFailure);
}

void SequenceAPI::FSequenceWallet::IsValidMessageSignature(uint64 ChainId, FAddress WalletAddress, FUnsizedData Message,
	FSignature Signature, TSuccessCallback<bool> OnSuccess, FFailureCallback OnFailure)
{
	const TFunction<TResult<bool> (FString)> ExtractSignature = [=](FString Content)
	{
		
		TSharedPtr<FJsonObject> Json = Parse(Content);
		TResult<bool> ReturnVal = MakeValue(false);

		if(!Json)
		{
			ReturnVal = MakeError(SequenceError{RequestFail, "Json did not parse"});
		}
		else
		{
			const bool bIsValid = Json->GetBoolField("isValid");
			ReturnVal = MakeValue(bIsValid);
		}
		
		return ReturnVal;
	};

	const FString Content = FJsonBuilder().ToPtr()
		  ->AddInt("chainId", ChainId)
		  ->AddString("walletAddress", "0x" + WalletAddress.ToHex())
		  ->AddString("message", "0x" + Message.ToHex())
		  ->AddString("signature", "0x" + Signature.ToHex())
		  ->ToString();
	
	this->SendRPCAndExtract(Url("isValidMessageSignature"), Content, OnSuccess, ExtractSignature,
	OnFailure);
}

void SequenceAPI::FSequenceWallet::SendTransaction(FTransaction Transaction, TSuccessCallback<FHash256> OnSuccess,
	FFailureCallback OnFailure)
{
	const TFunction<TResult<FHash256> (FString)> ExtractSignature = [=](FString Content)
	{
		
		TSharedPtr<FJsonObject> Json = Parse(Content);
		TResult<FHash256> ReturnVal = MakeValue(FHash256{});

		if(!Json)
		{
			ReturnVal = MakeError(SequenceError{RequestFail, "Json did not parse"});
		}
		else
		{
			const FString HashString = Json->GetStringField("txHash");
			ReturnVal = MakeValue(FHash256::From(HashString));
		}
		
		return ReturnVal;
	};

	const FString Content = FJsonBuilder().ToPtr()
		  ->AddField("tx", Transaction.ToJson())
		  ->ToString();
	
	this->SendRPCAndExtract(Url("SendTransaction"), Content, OnSuccess, ExtractSignature,
	OnFailure);
}

void SequenceAPI::FSequenceWallet::SendTransactionBatch(TArray<FTransaction> Transactions,
	TSuccessCallback<FHash256> OnSuccess, FFailureCallback OnFailure)
{
	const TFunction<TResult<FHash256> (FString)> ExtractSignature = [=](FString Content)
	{
		
		TResult<TSharedPtr<FJsonObject>> Json = ExtractJsonObjectResult(Content);
		TResult<FHash256> ReturnVal = MakeValue(FHash256{});

		if(Json.HasError())
		{
			ReturnVal = MakeError(Json.GetError());
		}
		else
		{
			const FString HashString = Json.GetValue()->GetStringField("txHash");
			ReturnVal = MakeValue(FHash256::From(HashString));
		}
		
		return ReturnVal;
	};

	FJsonArray JsonArray = FJsonBuilder().AddArray("txs");
	for(FTransaction Transaction : Transactions)
	{
		JsonArray.AddValue(Transaction.ToJson());
	}
	const FString Content = JsonArray.EndArray()->ToString();
	
	this->SendRPCAndExtract(Url("sendTransactionBatch"), Content, OnSuccess, ExtractSignature,
	OnFailure);
}
