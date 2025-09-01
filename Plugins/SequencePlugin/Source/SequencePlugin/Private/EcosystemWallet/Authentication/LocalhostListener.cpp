#include "LocalhostListener.h"
#include "HttpServerModule.h"
#include "IHttpRouter.h"
#include "HttpPath.h"
#include "HttpServerRequest.h"
#include "HttpServerResponse.h"
#include "Util/SequenceSupport.h"

void ULocalhostListener::Start()
{
	FHttpServerModule& HttpModule = FHttpServerModule::Get();

	Router = HttpModule.GetHttpRouter(Port);
	if (!Router.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("HTTP Router could not be created for port %u"), Port);
		return;
	}

	const FHttpPath RootPath(TEXT("/api"));
	RouteHandle = Router->BindRoute(
		RootPath,
		EHttpServerRequestVerbs::VERB_GET,
		FHttpRequestHandler::CreateLambda([this](const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete) -> bool
		{
			return this->HandleAnyRequest(Request, OnComplete);
		})
	);

	HttpModule.StartAllListeners();
}

bool ULocalhostListener::HandleAnyRequest(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
{
	for (const auto& Kvp : Request.QueryParams)
	{
		UE_LOG(LogTemp, Log, TEXT("[HTTP] %s, %s"), *Kvp.Key, *Kvp.Value);
	}

	const FString Id = Request.QueryParams["id"];

	if (Request.QueryParams.Contains("error"))
	{
		return true;
	}
	
	const FString EncodedPayload = Request.QueryParams["payload"];
	const FString PayloadJson = USequenceSupport::DecodeBase64ToString(EncodedPayload);

	UE_LOG(LogTemp, Log, TEXT("Response Payload Json %s"), *PayloadJson);

	const FString Body = TEXT("{}");
	TUniquePtr<FHttpServerResponse> Response = FHttpServerResponse::Create(
		TArray<uint8>((const uint8*)TCHAR_TO_ANSI(*Body), Body.Len()), TEXT("text/plain; charset=utf-8"));

	Response->Code = EHttpServerResponseCodes::Ok;
	OnComplete(MoveTemp(Response));

	return true;
}
