#pragma once

#include "CoreMinimal.h"
#include "ConfigFetcher.h"
#include "Authentication/RedirectHandler/IRedirectHandler.h"
#include "EcosystemWallet/Permissions/Permissions.h"
#include "UObject/Object.h"
#include "Util/Async.h"
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
	
	void CreateNewSession(ESessionCreationType Type, const FString& PreferredLoginMethod, const FString& Email,
		const TScriptInterface<IPermissions>& Permissions, const TSuccessCallback<bool>& OnSuccess, const FFailureCallback& OnFailure);

	template<typename TPayload, typename TResponse>
	void SendRequest(const FString& Path, const FString& Action, const TPayload& Payload,
		TFunction<void(TResponse)> OnSuccess, FFailureCallback OnFailure)
	{
		const FString EcosystemUrl = UConfigFetcher::GetConfigVar(UConfigFetcher::WalletAppUrl);
		const FString Url = FString::Printf(TEXT("%s/request/%s"), *EcosystemUrl, *Path);

		this->RedirectHandler->WaitForResponse(Url, Action, Payload, OnSuccess, OnFailure);
	}

private:
	UPROPERTY()
	USessionStorage* Storage = nullptr;

	TSharedPtr<IRedirectHandler> RedirectHandler = nullptr;

	FString Origin;
};
