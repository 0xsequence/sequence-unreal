#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Util/Async.h"
#include "EcosystemWallet/Primitives/Permission/SessionPermissions.h"
#include "Storage/SessionStorage.h"
#include "EcosystemClient.generated.h"

enum class SEQUENCEPLUGIN_API ESessionCreationType : uint8
{
	CreateNewSession UMETA(DisplayName = "Create New Session"),
	IncludeImplicit  UMETA(DisplayName = "Include Implicit"),
	AddExplicit      UMETA(DisplayName = "Add Explicit")
};

UCLASS()
class SEQUENCEPLUGIN_API UEcosystemClient : public UObject
{
	GENERATED_BODY()
	
public:
	UEcosystemClient();
	
	void CreateNewSession(ESessionCreationType Type,
		const FString& PreferredLoginMethod,
		const FString& Email,
		FSessionPermissions Permissions,
		const TSuccessCallback<bool>& OnSuccess,
		const FFailureCallback& OnFailure);

private:
	UPROPERTY()
	USessionStorage* Storage = nullptr;
};
