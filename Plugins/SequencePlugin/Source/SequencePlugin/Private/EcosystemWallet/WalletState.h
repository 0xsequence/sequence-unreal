#pragma once

#include "CoreMinimal.h"
#include "KeyMachine/KeyMachine.h"
#include "KeyMachine/Models/ConfigUpdatesResponse.h"
#include "KeyMachine/Models/DeployHashResponse.h"
#include "UObject/Object.h"
#include "WalletState.generated.h"

UCLASS()
class SEQUENCEPLUGIN_API UWalletState : public UObject
{
	GENERATED_BODY()

public:
	FString Address = "";
	FString ImageHash = "";
	FString SessionsImageHash = "";
	FString DeployHash = "";
	bool IsDeployed = false;
	int64 Nonce = 0;
	FDeployHashContext DeployContext = FDeployHashContext();
	TArray<FConfigUpdate> ConfigUpdates;

	// Add Config
	// Add Sessions Topology

	UWalletState();
	
	void UpdateState(const FString& Address, const TFunction<void()>& Callback);

private:
	UPROPERTY()
	UKeyMachine* KeyMachine;

	void UpdateDeployContext(const TFunction<void()>& Callback);
};
