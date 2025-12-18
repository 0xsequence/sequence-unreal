#include "HttpHandler.h"

#include "ConfigFetcher.h"
#include "EcosystemWallet/Authentication/RedirectHandler/MobileRedirectHandler.h"
#include "Util/Log.h"

void UHttpHandler::SetRequestUrl(const FString& Url)
{
	this->RequestUrl = Url;
}

void UHttpHandler::SendPostRequest(const FString& Endpoint, const FString& Payload, TSuccessCallback<FString> OnSuccess, TFunction<void(FString)> OnFailure) const
{
	FString Url = this->RequestUrl;
	if (!this->RequestUrl.EndsWith(TEXT("/")))
	{
		Url += TEXT("/");
	}
	
	const FString FinalUrl = FString::Printf(TEXT("%s%s"), *Url, *Endpoint);
	const TSharedRef<IHttpRequest> HTTP_Post_Req = FHttpModule::Get().CreateRequest();

	HTTP_Post_Req->SetVerb("POST");
	HTTP_Post_Req->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HTTP_Post_Req->SetHeader(TEXT("Accept"), TEXT("application/json"));
	HTTP_Post_Req->SetHeader(TEXT("Accept-Signature"), TEXT("sig=()"));

	const FString AccessKey = UConfigFetcher::GetConfigVar("ProjectAccessKey");
	HTTP_Post_Req->SetHeader(TEXT("X-Access-Key"), *AccessKey);
	
	HTTP_Post_Req->SetTimeout(30);
	HTTP_Post_Req->SetURL(FinalUrl);
	HTTP_Post_Req->SetContentAsString(Payload);

	const FString CurlCommand = FString::Printf(
		TEXT("curl -X %s \"%s\" -H \"Content-Type: application/json\" -H \"Accept: application/json\" --data \"%s\""),
		*HTTP_Post_Req->GetVerb(),
		*HTTP_Post_Req->GetURL(),
		*FString(UTF8_TO_TCHAR(HTTP_Post_Req->GetContent().GetData())).Replace(TEXT("\""), TEXT("\\\""))
	);

	SEQ_LOG_EDITOR(Log, TEXT("%s"), *CurlCommand);
	
	HTTP_Post_Req->OnProcessRequestComplete().BindLambda([OnSuccess, OnFailure](const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, const bool bWasSuccessful)
	{
		if (bWasSuccessful)
		{
			const FString Content = Response->GetContentAsString();
			SEQ_LOG(Display, TEXT("Response: %s"), *Content);
			OnSuccess(Content);
		}
		else
		{
			if (Request.IsValid() && Response.IsValid())
			{
				const FString ErrorMessage = Response->GetContentAsString();
				SEQ_LOG(Error, TEXT("Request failed: %s"), *ErrorMessage);  
				OnFailure("Request failed: " + ErrorMessage);
			}
			else
			{
				SEQ_LOG(Error, TEXT("Request failed: Invalid Request Pointer")); 
				OnFailure("Request failed: Invalid Request Pointer");
			}
		}
	});

	HTTP_Post_Req->ProcessRequest();
}

void UHttpHandler::SendGetRequest(const FString& Endpoint, TSuccessCallback<FString> OnSuccess, TFunction<void(FString)> OnFailure) const
{
	FString Url = this->RequestUrl;
	if (!this->RequestUrl.EndsWith(TEXT("/")))
	{
		Url += TEXT("/");
	}
	
	const FString FinalUrl = FString::Printf(TEXT("%s%s"), *Url, *Endpoint);
	const TSharedRef<IHttpRequest> HTTP_Post_Req = FHttpModule::Get().CreateRequest();

	HTTP_Post_Req->SetVerb("GET");
	HTTP_Post_Req->SetHeader(TEXT("Accept"), TEXT("application/json"));

	const FString AccessKey = UConfigFetcher::GetConfigVar("ProjectAccessKey");
	HTTP_Post_Req->SetHeader(TEXT("X-Access-Key"), *AccessKey);
	
	HTTP_Post_Req->SetTimeout(30);
	HTTP_Post_Req->SetURL(FinalUrl);

	const FString CurlCommand = FString::Printf(
		TEXT("curl -X %s \"%s\" -H \"Accept: application/json\""),
		*HTTP_Post_Req->GetVerb(),
		*HTTP_Post_Req->GetURL()
	);

	SEQ_LOG_EDITOR(Log, TEXT("%s"), *CurlCommand);
	
	HTTP_Post_Req->OnProcessRequestComplete().BindLambda([OnSuccess, OnFailure](const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, const bool bWasSuccessful)
	{
		if (bWasSuccessful)
		{
			const FString Content = Response->GetContentAsString();
			SEQ_LOG(Display, TEXT("Response: %s"), *Content);
			OnSuccess(Content);
		}
		else
		{
			if (Request.IsValid() && Response.IsValid())
			{
				const FString ErrorMessage = Response->GetContentAsString();
				SEQ_LOG(Error, TEXT("Request failed: %s"), *ErrorMessage);  
				OnFailure("Request failed: " + ErrorMessage);
			}
			else
			{
				SEQ_LOG(Error, TEXT("Request failed: Invalid Request Pointer")); 
				OnFailure("Request failed: Invalid Request Pointer");
			}
		}
	});

	HTTP_Post_Req->ProcessRequest();
}
