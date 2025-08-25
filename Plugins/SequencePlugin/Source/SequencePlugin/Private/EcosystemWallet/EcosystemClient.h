#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Util/Async.h"
#include "EcosystemClient.generated.h"

enum class ESessionCreationType : uint8
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
	void CreateNewSession(ESessionCreationType Type,
		const FString& PreferredLoginMethod,
		const TOptional<FString>& Email,
		const TSuccessCallback<bool>& OnSuccess,
		const FFailureCallback& OnFailure);
};
