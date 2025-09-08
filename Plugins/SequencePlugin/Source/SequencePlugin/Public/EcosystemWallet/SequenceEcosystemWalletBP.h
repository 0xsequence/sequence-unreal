#pragma once

#include "CoreMinimal.h"
#include "WalletInfo.h"
#include "SequenceEcosystemWallet.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SequenceEcosystemWalletBP.generated.h"

UCLASS(Blueprintable)
class SEQUENCEPLUGIN_API USequenceEcosystemWalletBP : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSession);
	
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnSignature, const FString&, Signature);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnTransaction, const FString&, Signature);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnFailure, const FString&, Error);
	DECLARE_DYNAMIC_DELEGATE(FOnSuccess);
	
	USequenceEcosystemWalletBP();

	UPROPERTY(BlueprintAssignable, Category = "0xSequence SDK - Events")
	FOnSession OnSessionClosed;

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Ecosystem Wallet")
	void AddSession(const TScriptInterface<IPermissions>& Permissions, FOnSuccess OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Ecosystem Wallet")
	void SignMessage(const FString& Message, FOnSignature OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Ecosystem Wallet")
	void SendTransaction(FOnTransaction OnSuccess, FOnFailure OnFailure);
	
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Ecosystem Wallet")
	void SendTransactionThroughEcosystem(FOnTransaction OnSuccess, FOnFailure OnFailure);
	
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Ecosystem Wallet")
	void ClearSessions();

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Ecosystem Wallet")
	FWalletInfo GetWalletInfo();

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Ecosystem Wallet")
	bool CheckIfWalletExists();
	
	USequenceEcosystemWallet* GetSequenceEcosystemWallet() const;

private:
	UPROPERTY()
	USequenceEcosystemWallet* Wallet = nullptr;

	void CallOnSessionClosed() const;
};
