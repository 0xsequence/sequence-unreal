#include "GuardService.h"
#include "Util/HttpHandler.h"
#include "Util/SequenceSupport.h"

void UGuardService::WithHost(const FString& Host)
{
	this->HttpHandler = NewObject<UHttpHandler>();
	this->HttpHandler->SetRequestUrl(Host);
}

void UGuardService::SignWith(const FSignWithArgs& Args, TSuccessCallback<FSignWithResponse> OnSuccess, const TFunction<void(FString)>& OnFailure) const
{
	const TSuccessCallback<FString> OnRequestSuccess = [OnSuccess](const FString& Response)
	{
		const FSignWithResponse Data = USequenceSupport::JSONStringToStruct<FSignWithResponse>(Response);
		OnSuccess(Data);
	};
	
	const FString Payload = USequenceSupport::StructToString(Args);
	this->HttpHandler->SendPostRequest("DeployHash", Payload, OnRequestSuccess, OnFailure);
}
