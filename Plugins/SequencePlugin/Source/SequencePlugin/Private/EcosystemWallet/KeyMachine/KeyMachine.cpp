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
	SendPostRequest("DeployHash", Payload, OnRequestSuccess, OnFailure);
}

void UKeyMachine::GetTree(const FString& ImageHash, TSuccessCallback<TSharedPtr<FJsonValue>> OnSuccess, const TFunction<void(FString)>& OnFailure) const
{
	const TSuccessCallback<FString> OnRequestSuccess = [this, OnSuccess](const FString& Response)
	{
		const TSharedPtr<FJsonValue> TreeData = ParseSessionsTreeFromJson(Response);
		OnSuccess(TreeData);
	};
	
	const FTreeArgs Args = {
		ImageHash,
	};

	const FString Payload = USequenceSupport::StructToString(Args);
	SendPostRequest("Tree", Payload, OnRequestSuccess, OnFailure);
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
	SendPostRequest("ConfigUpdates", Payload, OnRequestSuccess, OnFailure);
}

void UKeyMachine::GetConfiguration(const FString& ImageHash, TSuccessCallback<FSeqConfigContext> OnSuccess, const TFunction<void(FString)>& OnFailure) const
{
	const TSuccessCallback<FString> OnRequestSuccess = [this, OnSuccess](const FString& Response)
	{
		const FSeqConfigContext Data = ParseConfigFromJson(Response);
		OnSuccess(Data);
	};
	
	const FConfigArgs Args = {
		ImageHash
	};

	const FString Payload = USequenceSupport::StructToString(Args);
	SendPostRequest("Config", Payload, OnRequestSuccess, OnFailure);
}

void UKeyMachine::SendPostRequest(const FString& Endpoint, const FString& Payload, const TSuccessCallback<FString>& OnSuccess, const TFunction<void(FString)>& OnFailure)
{
	UHttpHandler* Handler = NewObject<UHttpHandler>();
	Handler->SetRequestUrl(TEXT("https://v3-keymachine.sequence-dev.app/rpc/Sessions"));
	Handler->SendPostRequest(Endpoint, Payload, OnSuccess, OnFailure);
}

FSeqConfigContext UKeyMachine::ParseConfigFromJson(const FString& Json)
{
	TSharedPtr<FJsonObject> Root;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Json);

	if (!FJsonSerializer::Deserialize(Reader, Root) || !Root.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid JSON for FSeqConfigContext"));
		return FSeqConfigContext();
	}

	TSharedPtr<FJsonObject> ConfigObject = Root->GetObjectField(TEXT("config"));

	FSeqConfigContext Context;
	Context.Checkpoint = ConfigObject->GetStringField(TEXT("checkpoint"));
	Context.Threshold = ConfigObject->GetIntegerField(TEXT("threshold"));

	TSharedPtr<FJsonValue> TreeValue = ConfigObject->TryGetField(TEXT("tree"));
	if (TreeValue.IsValid())
	{
		Context.Tree = TreeValue;
	}

	return Context;
}

TSharedPtr<FJsonValue> UKeyMachine::ParseSessionsTreeFromJson(const FString& Json)
{
	TSharedPtr<FJsonObject> Root;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Json);

	if (!FJsonSerializer::Deserialize(Reader, Root) || !Root.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid JSON for ParseSessionsTreeFromJson"));
		return TSharedPtr<FJsonValue>();
	}

	TSharedPtr<FJsonValue> TreeValue = Root->TryGetField(TEXT("tree"));
	if (TreeValue.IsValid())
	{
		return TreeValue;
	}

	return TSharedPtr<FJsonValue>();
}
