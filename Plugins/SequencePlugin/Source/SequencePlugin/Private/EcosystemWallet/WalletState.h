#pragma once

#include "CoreMinimal.h"
#include "Provider.h"
#include "KeyMachine/KeyMachine.h"
#include "KeyMachine/Models/ConfigUpdatesResponse.h"
#include "KeyMachine/Models/DeployHashResponse.h"
#include "EcosystemWallet/Primitives/Config/SeqConfig.h"
#include "EcosystemWallet/Primitives/Sessions/SessionsTopology.h"
#include "UObject/Object.h"

class SEQUENCEPLUGIN_API FWalletState
{
public:
	FString Address = "";
	FString ImageHash = "";
	FString SessionsImageHash = "";
	FString DeployHash = "";
	bool IsDeployed = false;
	FString Nonce = "0";
	FDeployHashContext DeployContext = FDeployHashContext();
	TArray<FConfigUpdate> ConfigUpdates;
	TSharedPtr<FSeqConfig> Config;

	FWalletState();
	
	void UpdateState(const FString& Address, const TFunction<void()>& OnSuccess, const TFunction<void()>& OnFailure);

private:
	UKeyMachine* KeyMachine;

	UProvider* Provider;

	TSharedPtr<FSessionsTopology> SessionsTopology;

	int32 UpdateProgress = 0;

	void UpdateDeployContext(const TFunction<void()>& Callback);
	void UpdateDeployedState(const TFunction<void()>& Callback);
	void UpdateNonce(const TFunction<void()>& Callback);
	void UpdateConfig(const FString& ImageHash, const TFunction<void()>& Callback);
	void UpdateSessionsTopology(const FString& ImageHash, const TFunction<void()>& Callback);
	void GetImplementation(const TFunction<void(FString)>& Callback);
	void GetOnchainImageHash(const TFunction<void(FString)>& Callback);
};
