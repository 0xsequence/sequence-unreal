#include "LocalhostListener.h"
#include "HttpServerModule.h"
#include "IHttpRouter.h"
#include "HttpPath.h"
#include "HttpServerRequest.h"
#include "HttpServerResponse.h"
#include "Util/SequenceSupport.h"

ULocalhostListener* ULocalhostListener::Instance = nullptr;

void ULocalhostListener::ClearInstance()
{
	if (Instance == nullptr)
		return;
	
	Instance->StopListening();
	Instance = nullptr;
}

ULocalhostListener* ULocalhostListener::GetInstance()
{
	if (Instance == nullptr)
	{
		Instance = NewObject<ULocalhostListener>();
	}

	return Instance;
}

void ULocalhostListener::WaitForResponse(TSuccessCallback<FString> OnSuccess, FFailureCallback OnFailure)
{
	this->CurrentOnSuccess = MakeShared<TSuccessCallback<FString>>(OnSuccess);
	this->CurrentOnFailure = MakeShared<FFailureCallback>(OnFailure);
	
	if (this->bServerStarted)
	{
		return;
	}
	
	FHttpServerModule& HttpModule = FHttpServerModule::Get();

	Router = HttpModule.GetHttpRouter(Port, true);
	if (!Router.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("HTTP Router could not be created for port %u"), Port);
		OnFailure(FSequenceError(EErrorType::InvalidArgument, TEXT("Router could not be created")));
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
	this->bServerStarted = true;
}

void ULocalhostListener::StopListening()
{
	if (Router.IsValid() && RouteHandle.IsSet())
	{
		Router->UnbindRoute(RouteHandle.GetValue());
		RouteHandle.Reset();
	}

	if (bServerStarted)
	{
		FHttpServerModule::Get().StopAllListeners();
		bServerStarted = false;
	}

	Router.Reset();
}

bool ULocalhostListener::HandleAnyRequest(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
{
	const FString Id = Request.QueryParams["id"];

	if (Request.QueryParams.Contains("error"))
	{
		if (CurrentOnFailure.IsValid() && *CurrentOnFailure)
		{
			(*CurrentOnFailure)(FSequenceError(EErrorType::InvalidArgument, Request.QueryParams["error"]));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("CurrentOnFailure is invalid"));
		}
	}
	else
	{
		const FString EncodedPayload = Request.QueryParams["payload"];
		const FString PayloadJson = USequenceSupport::DecodeBase64ToString(EncodedPayload);

		UE_LOG(LogTemp, Log, TEXT("Response Payload Json %s"), *PayloadJson);

		if (CurrentOnSuccess.IsValid() && *CurrentOnSuccess)
		{
			(*CurrentOnSuccess)(PayloadJson);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("CurrentOnSuccess is invalid"));
		}
	}

	const FString Body = TEXT("{}");
	TUniquePtr<FHttpServerResponse> Response = FHttpServerResponse::Create(
		TArray<uint8>((const uint8*)TCHAR_TO_ANSI(*Body), Body.Len()), TEXT("text/plain; charset=utf-8"));

	Response->Code = EHttpServerResponseCodes::Ok;
	OnComplete(MoveTemp(Response));

	return true;
}
