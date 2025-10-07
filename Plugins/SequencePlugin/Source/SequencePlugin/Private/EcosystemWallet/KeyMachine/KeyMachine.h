#pragma once

#include "CoreMinimal.h"
#include "Models/ConfigResponse.h"
#include "Models/ConfigUpdatesResponse.h"
#include "Models/DeployHashResponse.h"
#include "Models/TreeResponse.h"
#include "UObject/Object.h"
#include "Util/Async.h"
#include "KeyMachine.generated.h"

class UHttpHandler;

UCLASS()
class SEQUENCEPLUGIN_API UKeyMachine : public UObject
{
	GENERATED_BODY()

public:
	UKeyMachine();
	
	void GetDeployHash(const FString& WalletAddress, TSuccessCallback<FDeployHashResponse> OnSuccess, const TFunction<void(FString)>& OnFailure) const;
	void GetTree(const FString& ImageHash, TSuccessCallback<TSharedPtr<FJsonValue>> OnSuccess, const TFunction<void(FString)>& OnFailure) const;
	void GetConfigUpdates(const FString& WalletAddress, const FString& FromImageHash, TSuccessCallback<FConfigUpdatesResponse> OnSuccess, const TFunction<void(FString)>& OnFailure) const;
	void GetConfiguration(const FString& ImageHash, TSuccessCallback<FSeqConfigContext> OnSuccess, const TFunction<void(FString)>& OnFailure) const;
	
private:
	static FSeqConfigContext ParseConfigFromJson(const FString& Json);
	static TSharedPtr<FJsonValue> ParseSessionsTreeFromJson(const FString& Json);
	
	UPROPERTY()
	UHttpHandler* HttpHandler;
	
	const FString Host = "https://v3-keymachine.sequence-dev.app/rpc/Sessions";
};
