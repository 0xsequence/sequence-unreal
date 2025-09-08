#pragma once

#include "CoreMinimal.h"
#include "WalletInfo.h"
#include "EcosystemWallet/EcosystemClient.h"
#include "Permissions/Permissions.h"
#include "UObject/Object.h"
#include "Util/Async.h"
#include "SequenceEcosystemWallet.generated.h"

class USessionStorage;

UCLASS()
class SEQUENCEPLUGIN_API USequenceEcosystemWallet : public UObject
{
	GENERATED_BODY()

public:
	USequenceEcosystemWallet();

	void AddSession(const TScriptInterface<IPermissions>& Permissions, TSuccessCallback<bool> OnSuccess, const FFailureCallback& OnFailure);
	void SignMessage(const FString& Message, TSuccessCallback<FString> OnSuccess, const FFailureCallback& OnFailure);
	void SendTransaction(TSuccessCallback<FString> OnSuccess, const FFailureCallback& OnFailure);
	void SendTransactionThroughEcosystem(TSuccessCallback<FString> OnSuccess, const FFailureCallback& OnFailure);
	
	void ClearSessions();
	FWalletInfo GetWalletInfo();
	bool CheckIfWalletExists();

private:
	UPROPERTY()
	UEcosystemClient* Client = nullptr;
	
	UPROPERTY()
	USessionStorage* SessionStorage;
};
