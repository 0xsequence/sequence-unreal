#pragma once

#include "CoreMinimal.h"
#include "WalletInfo.h"
#include "EcosystemWallet/EcosystemClient.h"
#include "Permissions/Permissions.h"
#include "Transactions/Transaction.h"
#include "UObject/Object.h"
#include "Util/Async.h"
#include "SequenceWallet.generated.h"

class USessionStorage;
class FWalletState;

UCLASS()
class SEQUENCEPLUGIN_API USequenceWallet : public UObject
{
	GENERATED_BODY()

public:
	USequenceWallet();

	void AddSession(const TScriptInterface<IPermissions>& Permissions, TSuccessCallback<bool> OnSuccess, const FFailureCallback& OnFailure);
	void SignMessage(const FString& Message, TSuccessCallback<FString> OnSuccess, const FFailureCallback& OnFailure);
	void SendTransaction(const TScriptInterface<ISeqTransactionBase>& Transaction, TSuccessCallback<FString> OnSuccess, const FFailureCallback& OnFailure);
	void SendTransactionWithoutPermissions(const TScriptInterface<ISeqTransactionBase>& Transaction, TSuccessCallback<FString> OnSuccess, const FFailureCallback& OnFailure);
	
	void ClearSessions();
	FWalletInfo GetWalletInfo();
	bool CheckIfWalletExists();

private:
	UPROPERTY()
	UEcosystemClient* Client = nullptr;
	
	UPROPERTY()
	USessionStorage* SessionStorage;

	TSharedPtr<FWalletState> WalletState;
};
