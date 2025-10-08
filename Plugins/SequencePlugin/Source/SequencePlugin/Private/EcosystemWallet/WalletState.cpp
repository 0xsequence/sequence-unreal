#include "WalletState.h"
#include "EcosystemWallet/Primitives/Config/Leafs/ConfigSapientSignerLeaf.h"
#include "EcosystemWallet/Primitives/Sessions/SessionsTopology.h"
#include "EthAbi/EthAbiBridge.h"
#include "Util/Log.h"
#include "Util/SequenceSupport.h"

UWalletState::UWalletState()
{
	this->KeyMachine = NewObject<UKeyMachine>();
	this->Provider = NewObject<UProvider>();
}

void UWalletState::UpdateState(const FString& Address, const TFunction<void()>& OnSuccess, const TFunction<void()>& OnFailure)
{
	this->Address = Address;
	
	UpdateDeployedState([this, OnSuccess, OnFailure]() {
		this->GetImplementation([this, OnSuccess, OnFailure](const FString& Implementation) {
			const TFunction<void(FString)> GetConfig = [this, OnSuccess, OnFailure](const FString& WalletImageHash)
			{
				this->KeyMachine->GetConfigUpdates(this->Address, WalletImageHash, [this, OnSuccess, OnFailure, WalletImageHash](const FConfigUpdatesResponse& Updates)
				{
					FString ConfigImageHash = WalletImageHash;
					if (Updates.Updates.Num() > 0)
					{
						ConfigImageHash = Updates.Updates[Updates.Updates.Num() - 1].ToImageHash;
					}
					
					this->UpdateConfig(ConfigImageHash, [this, OnSuccess, OnFailure]()
					{
						const FString SessionsManagerAddress = TEXT("0x0000000000CC58810c33F3a0D78aA1Ed80FaDcD8");
						TSharedPtr<FConfigLeaf> Leaf = this->Config->Topology.Get()->FindSignerLeaf(SessionsManagerAddress);
						
						if (!Leaf.IsValid())
						{
							OnFailure();
							return;
						}

						const FConfigSapientSignerLeaf* SapientSignerLeaf = static_cast<FConfigSapientSignerLeaf*>(Leaf.Get());
						const FString SessionsLeafImageHash = SapientSignerLeaf->ImageHash;
						
						this->UpdateSessionsTopology(SessionsLeafImageHash, [this, OnSuccess]()
						{
							this->UpdateNonce(OnSuccess);
						});
					});
				},
				[OnFailure](const FString& Error)
				{
					SEQ_LOG(Error, TEXT("Error while getting config updates: %s"), *Error);
					OnFailure();
				});
			};
			
			if (this->IsDeployed && Implementation == "0x7438718F9E4b9B834e305A620EEeCf2B9E6eBE79")
			{
				this->GetOnchainImageHash([this, GetConfig](const FString& ImageHash)
				{
					GetConfig(ImageHash);
				});
			}
			else
			{
				this->UpdateDeployContext([this, GetConfig]()
				{
					GetConfig(this->DeployHash);
				});
			}
		});
	});
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
		this->Nonce = FEthAbiBridge::HexToBigIntString(Response);
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

void UWalletState::UpdateConfig(const FString& ImageHash, const TFunction<void()>& Callback)
{
	const TSuccessCallback<FSeqConfigContext> OnSuccess = [this, Callback](const FSeqConfigContext& Response)
	{
		this->Config = NewObject<USeqConfig>();
		this->Config->Checkpoint = Response.Checkpoint;
		this->Config->Threshold = Response.Threshold;
		this->Config->Topology = FConfigTopology::FromServiceConfigTree(Response.Tree);
		
		Callback();
	};

	const TFunction<void(FString)> OnFailure = [this, Callback](const FString& Error)
	{
		SEQ_LOG(Error, TEXT("Error while updating wallet config: %s"), *Error);
		Callback();
	};
	
	KeyMachine->GetConfiguration(ImageHash, OnSuccess, OnFailure);
}

void UWalletState::UpdateSessionsTopology(const FString& ImageHash, const TFunction<void()>& Callback)
{
	const TSuccessCallback<TSharedPtr<FJsonValue>> OnSuccess = [this, Callback](const TSharedPtr<FJsonValue>& Tree)
	{
		this->SessionsTopology = FSessionsTopology::FromServiceConfigTree(Tree);
		Callback();
	};

	const TFunction<void(FString)> OnFailure = [this, Callback](const FString& Error)
	{
		SEQ_LOG(Error, TEXT("Error while updating sessions topology: %s"), *Error);
		Callback();
	};
	
	KeyMachine->GetTree(ImageHash, OnSuccess, OnFailure);
}

void UWalletState::GetImplementation(const TFunction<void(FString)>& Callback)
{
	const TSuccessCallback<FString> OnSuccess = [this, Callback](const FString& Response)
	{
		Callback(Response);
	};

	const FFailureCallback OnFailure = [this, Callback](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error while getting implementation: %s"), *Error.Message);
		Callback("");
	};

	const FString EncodedFunctionData = USequenceSupport::EncodeFunctionCall("getImplementation()", "[]");

	FContractCall Call;
	Call.To = this->Address;
	Call.Data = EncodedFunctionData;
	
	this->Provider->Call(Call, EBlockTag::EPending, OnSuccess, OnFailure);
}

void UWalletState::GetOnchainImageHash(const TFunction<void(FString)>& Callback)
{
	const TSuccessCallback<FString> OnSuccess = [this, Callback](const FString& Response)
	{
		Callback(Response);
	};

	const FFailureCallback OnFailure = [this, Callback](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error while getting onchain image hash: %s"), *Error.Message);
		Callback(TEXT(""));
	};

	const FString EncodedFunctionData = USequenceSupport::EncodeFunctionCall("imageHash()", "[]");

	FContractCall Call;
	Call.To = this->Address;
	Call.Data = EncodedFunctionData;
	
	this->Provider->Call(Call, EBlockTag::EPending, OnSuccess, OnFailure);
}

