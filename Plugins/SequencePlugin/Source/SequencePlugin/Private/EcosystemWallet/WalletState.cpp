#include "WalletState.h"
#include "EcosystemWallet/Primitives/Config/Leafs/ConfigSapientSignerLeaf.h"
#include "EcosystemWallet/Primitives/Sessions/SessionsTopology.h"
#include "EthAbi/EthAbiBridge.h"
#include "Extensions/ExtensionsFactory.h"
#include "Util/Log.h"
#include "Util/SequenceSupport.h"

FWalletState::FWalletState()
{
	this->KeyMachine = NewObject<UKeyMachine>();
}

void FWalletState::UpdateState(const FString& InAddress, const TFunction<void()>& OnSuccess, const TFunction<void(FString)>& OnFailure)
{
	this->Address = InAddress;
	this->Provider = NewObject<UProvider>();
	
	UpdateDeployedState([this, OnSuccess, OnFailure]() {
		this->GetImplementation([this, OnSuccess, OnFailure](const FString& Implementation) {
			const TFunction<void(FString)> GetConfig = [this, OnSuccess, OnFailure](const FString& WalletImageHash)
			{
				this->KeyMachine->GetConfigUpdates(this->Address, WalletImageHash, [this, OnSuccess, OnFailure, WalletImageHash](const FConfigUpdatesResponse& Updates)
				{
					this->ConfigUpdates = Updates.Updates;
					
					FString ConfigImageHash = WalletImageHash;
					if (Updates.Updates.Num() > 0)
					{
						ConfigImageHash = Updates.Updates[Updates.Updates.Num() - 1].ToImageHash;
					}
					
					this->UpdateConfig(ConfigImageHash, [this, OnSuccess, OnFailure]()
					{
						TSharedPtr<FConfigLeaf> Leaf = this->Config->Topology.Get()->FindSignerLeaf(FExtensionsFactory::GetCurrent().Sessions);
						
						if (!Leaf.IsValid())
						{
							OnFailure("");
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
					OnFailure("");
				});
			};
			
			if (this->IsDeployed && Implementation.Equals("0x7438718F9E4b9B834e305A620EEeCf2B9E6eBE79", ESearchCase::IgnoreCase))
			{
				this->GetOnchainImageHash([this, GetConfig](const FString& OnchainImageHash)
				{
					GetConfig(OnchainImageHash);
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

void FWalletState::UpdateDeployContext(const TFunction<void()>& Callback)
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

void FWalletState::UpdateDeployedState(const TFunction<void()>& Callback)
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

void FWalletState::UpdateNonce(const TFunction<void()>& Callback)
{
	const TSuccessCallback<FString> OnSuccess = [this, Callback](const FString& Response)
	{
		this->Nonce = FEthAbiBridge::HexToBigIntString(Response);
		UE_LOG(LogTemp, Display, TEXT("Nonce %s"), *Nonce)
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

void FWalletState::UpdateConfig(const FString& InImageHash, const TFunction<void()>& Callback)
{
	const TSuccessCallback<FSeqConfigContext> OnSuccess = [this, Callback](const FSeqConfigContext& Response)
	{
		this->Config = MakeShared<FSeqConfig>(FSeqConfig(
		FBigInt(FString::Printf(TEXT("%d"), Response.Threshold)),
			FBigInt(Response.Checkpoint),
			FConfigTopology::FromServiceConfigTree(Response.Tree)));
		
		Callback();
	};

	const TFunction<void(FString)> OnFailure = [this, Callback](const FString& Error)
	{
		SEQ_LOG(Error, TEXT("Error while updating wallet config: %s"), *Error);
		Callback();
	};
	
	KeyMachine->GetConfiguration(InImageHash, OnSuccess, OnFailure);
}

void FWalletState::UpdateSessionsTopology(const FString& InImageHash, const TFunction<void()>& Callback)
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
	
	KeyMachine->GetTree(InImageHash, OnSuccess, OnFailure);
}

void FWalletState::GetImplementation(const TFunction<void(FString)>& Callback)
{
	const TSuccessCallback<FString> OnSuccess = [this, Callback](const FString& Response)
	{
		const FString Code = FByteArrayUtils::BytesToHexString(FByteArrayUtils::SliceBytesFrom(FByteArrayUtils::HexStringToBytes(Response), 12));
		UE_LOG(LogTemp, Display, TEXT("Implementation %s"), *Code);
		Callback(Code);
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

void FWalletState::GetOnchainImageHash(const TFunction<void(FString)>& Callback)
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

