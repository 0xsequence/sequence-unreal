#pragma once

#include "CoreMinimal.h"
#include "WalletInfo.h"
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

	void SignMessage(const FString& Message, TSuccessCallback<bool> OnSuccess, const FFailureCallback& OnFailure);
	void ClearSessions();
	
	FWalletInfo GetWalletInfo();
	bool CheckIfWalletExists();

private:
	UPROPERTY()
	USessionStorage* SessionStorage;
};
