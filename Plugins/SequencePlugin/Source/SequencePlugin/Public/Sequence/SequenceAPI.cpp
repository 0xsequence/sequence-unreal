#include "SequenceAPI.h"

#include "Http.h"
#include "HttpManager.h"

FString SortOrderToString(const SequenceAPI::ESortOrder SortOrder)
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

FString SequenceAPI::FSequenceWallet::Url(const FString Name) const
{
	return this->Hostname + this->Path + Name;
}

SequenceAPI::FSequenceWallet::FSequenceWallet(FString Hostname) : Hostname(Hostname)
{
	
}

void SequenceAPI::FSequenceWallet::CreateWallet(uint64 AccountIndex, TSuccessCallback<FAddress> OnSuccess,
	FFailureCallback OnFailure)
{
	TFunction<TResult<FAddress> (FString)> ExtractAddress = [=](FString Content)
	{
		TResult<TSharedPtr<FJsonObject>> Json = ExtractJsonObjectResult(Content);
		TResult<FAddress> Retval = MakeValue(FAddress{});

		if(Json.HasError())
		{
			Retval = MakeError(Json.GetError());
		}
		else
		{
			const FString AddressString = Json.GetValue()->GetStringField("address");
			Retval = MakeValue(FAddress::From(AddressString));
		}
		
		
		return Retval;
	};
	
	this->SendRPCAndExtract(Url("CreateWallet"), "", OnSuccess, ExtractAddress,
	OnFailure);
}
