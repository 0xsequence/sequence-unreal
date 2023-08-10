#include "SequenceAPI.h"

#include "Http.h"
#include "HttpManager.h"

FString SortOrderToString(const ESortOrder SortOrder)
{
	switch(SortOrder)
	{
	case ASC:
		return "ASC";
	case DESC:
		return "DESC";
	default: return "";
	}
}

FString FSequenceWallet::Url(const FString Name) const
{
	return this->Hostname + this->Path + Name;
}

void FSequenceWallet::HTTPPost(const FString Endpoint, const FString Args, TSuccessCallback<FString> OnSuccess,
                               FFailureCallback OnFailure)
{
	//Now we create the post request
	const TSharedRef<IHttpRequest> HttpPostReq = FHttpModule::Get().CreateRequest();

	HttpPostReq->SetVerb("POST");
	HttpPostReq->SetHeader("Content-Type", "application/json");
	HttpPostReq->SetHeader("Accept", "application/json");
	HttpPostReq->SetURL(this->Url(Endpoint));
	HttpPostReq->SetContentAsString(Args);//args will need to be a json object converted to a string

	HttpPostReq->OnProcessRequestComplete().BindLambda([=](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
	{
		if(bWasSuccessful)
		{
			const FString Content = Request->GetResponse()->GetContentAsString();
			OnSuccess(Content);
		}
		else
		{
			OnFailure(SequenceError(RequestFail, "Request failed: " + Request->GetResponse()->GetContentAsString()));
		}
	});
	
	HttpPostReq->ProcessRequest();
}

FSequenceWallet::FSequenceWallet(FString Hostname) : Hostname(Hostname)
{
	
}

void FSequenceWallet::CreateWallet(uint64 AccountIndex, TSuccessCallback<FAddress> OnSuccess,
	FFailureCallback OnFailure)
{
	
	
	this->HTTPPost("CreateWallet", "", [=](FString Content)
	{
		
	}, OnFailure);
}
