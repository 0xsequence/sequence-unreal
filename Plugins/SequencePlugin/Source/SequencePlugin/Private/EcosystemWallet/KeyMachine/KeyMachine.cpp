#include "KeyMachine.h"
#include "Models/ConfigArgs.h"
#include "Models/ConfigUpdatesArgs.h"
#include "Models/DeployHashArgs.h"
#include "Models/TreeArgs.h"
#include "Util/HttpHandler.h"
#include "Util/SequenceSupport.h"

UKeyMachine::UKeyMachine()
{
	this->HttpHandler = NewObject<UHttpHandler>();
	this->HttpHandler->SetRequestUrl(this->Host);
}

void UKeyMachine::GetDeployHash(const FString& WalletAddress, TSuccessCallback<FDeployHashResponse> OnSuccess, const TFunction<void(FString)>& OnFailure) const
{
	const TSuccessCallback<FString> OnRequestSuccess = [OnSuccess](const FString& Response)
	{
		const FDeployHashResponse Data = USequenceSupport::JSONStringToStruct<FDeployHashResponse>(Response);
		OnSuccess(Data);
	};
	
	const FDeployHashArgs Args = {
		WalletAddress,
	};

	const FString Payload = USequenceSupport::StructToString(Args);
	this->HttpHandler->SendPostRequest("DeployHash", Payload, OnRequestSuccess, OnFailure);
}

void UKeyMachine::GetTree(const FString& ImageHash, TSuccessCallback<FTreeResponse> OnSuccess, const TFunction<void(FString)>& OnFailure) const
{
	const TSuccessCallback<FString> OnRequestSuccess = [OnSuccess](const FString& Response)
	{
		const FTreeResponse Data = USequenceSupport::JSONStringToStruct<FTreeResponse>(Response);
		OnSuccess(Data);
	};
	
	const FTreeArgs Args = {
		ImageHash,
	};

	const FString Payload = USequenceSupport::StructToString(Args);
	this->HttpHandler->SendPostRequest("Tree", Payload, OnRequestSuccess, OnFailure);
}

void UKeyMachine::GetConfigUpdates(const FString& WalletAddress, const FString& FromImageHash, TSuccessCallback<FConfigUpdatesResponse> OnSuccess, const TFunction<void(FString)>& OnFailure) const
{
	const TSuccessCallback<FString> OnRequestSuccess = [OnSuccess](const FString& Response)
	{
		const FConfigUpdatesResponse Data = USequenceSupport::JSONStringToStruct<FConfigUpdatesResponse>(Response);
		OnSuccess(Data);
	};
	
	const FConfigUpdatesArgs Args = {
		WalletAddress,
		FromImageHash,
	};

	const FString Payload = USequenceSupport::StructToString(Args);
	this->HttpHandler->SendPostRequest("ConfigUpdates", Payload, OnRequestSuccess, OnFailure);
}

void UKeyMachine::GetConfiguration(const FString& ImageHash, TSuccessCallback<FConfigResponse> OnSuccess, const TFunction<void(FString)>& OnFailure) const
{
	const TSuccessCallback<FString> OnRequestSuccess = [OnSuccess](const FString& Response)
	{
		const FConfigResponse Data = USequenceSupport::JSONStringToStruct<FConfigResponse>(Response);
		OnSuccess(Data);
	};
	
	const FConfigArgs Args = {
		ImageHash
	};

	const FString Payload = USequenceSupport::StructToString(Args);
	this->HttpHandler->SendPostRequest("Config", Payload, OnRequestSuccess, OnFailure);
}
