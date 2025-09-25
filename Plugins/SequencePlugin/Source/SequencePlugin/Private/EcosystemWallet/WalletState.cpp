#include "WalletState.h"
#include "Util/Log.h"
#include "Util/SequenceSupport.h"

UWalletState::UWalletState()
{
	this->KeyMachine = NewObject<UKeyMachine>();
	this->Provider = NewObject<UProvider>();
}

void UWalletState::UpdateState(const FString& Address, const TFunction<void()>& Callback)
{
	UpdateProgress = 0;
	
	const TFunction<void()> InnerCallback = [this, Callback]() {
		UpdateProgress++;
		if (this->UpdateProgress >= 3)
		{
			Callback();
		}
	};
	
	this->Address = Address;
	this->UpdateDeployContext(InnerCallback);
	this->UpdateDeployedState(InnerCallback);
	this->UpdateNonce(InnerCallback);
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
		SEQ_LOG(Error, TEXT("Error while updating deploy context: %s"), *Error);
		Callback();
	};
	
	this->KeyMachine->GetDeployHash(this->Address, OnSuccess, OnFailure);
}

void UWalletState::UpdateDeployedState(const TFunction<void()>& Callback)
{
	const TSuccessCallback<FString> OnSuccess = [this, Callback](const FString& Response)
	{
		this->IsDeployed = Response != "0x";
		Callback();
	};

	const FFailureCallback OnFailure = [this, Callback](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error while updating deployed state: %s"), *Error.Message);
		Callback();
	};
	
	this->Provider->CodeAt(this->Address, EBlockTag::EPending, OnSuccess, OnFailure);
}

void UWalletState::UpdateNonce(const TFunction<void()>& Callback)
{
	const TSuccessCallback<FString> OnSuccess = [this, Callback](const FString& Response)
	{
		this->IsDeployed = Response != "0x";
		Callback();
	};

	const FFailureCallback OnFailure = [this, Callback](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error while updating nonce: %s"), *Error.Message);
		Callback();
	};

	constexpr int32 Space = 0;
	const FString EncodedFunctionData = USequenceSupport::EncodeFunctionCall(
		"readNonce(uint256)", FString::Printf(TEXT("[%d]"), Space));

	FContractCall Call;
	Call.To = this->Address;
	Call.Data = EncodedFunctionData;
	
	this->Provider->Call(Call, EBlockTag::EPending, OnSuccess, OnFailure);
}

void UWalletState::UpdateConfig()
{
	
}

void UWalletState::UpdateSessionsTopology()
{
	
}
