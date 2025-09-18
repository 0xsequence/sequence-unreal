#include "WalletState.h"
#include "Util/Log.h"

UWalletState::UWalletState()
{
	this->KeyMachine = NewObject<UKeyMachine>();
}

void UWalletState::UpdateState(const FString& Address, const TFunction<void()>& Callback)
{
	this->Address = Address;
	this->UpdateDeployContext(Callback);
}

void UWalletState::UpdateDeployContext(const TFunction<void()>& Callback)
{
	const TSuccessCallback<FDeployHashResponse> OnSuccess = [this, Callback](const FDeployHashResponse& Response)
	{
		this->DeployHash = Response.DeployHash;
		this->DeployContext = Response.Context;
		Callback();
	};

	const TFunction<void(FString)> OnFailure = [this, Callback](const FString& Error)
	{
		SEQ_LOG(Error, TEXT("%s"), *Error);
		Callback();
	};
	
	this->KeyMachine->GetDeployHash(this->Address, OnSuccess, OnFailure);
}
