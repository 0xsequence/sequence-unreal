#pragma once

#include "CoreMinimal.h"
#include "WalletInfo.h"
#include "SequenceWallet.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Transactions/Transaction.h"
#include "SequenceWalletBP.generated.h"

UCLASS(Blueprintable)
class SEQUENCEPLUGIN_API USequenceWalletBP : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSession);
	
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnSignature, const FString&, Signature);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnFeeOptions, const FFeeOptionsResponse&, FeeOptions);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnTransaction, const FString&, Hash);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnFailure, const FString&, Error);
	DECLARE_DYNAMIC_DELEGATE(FOnSuccess);
	
	USequenceWalletBP();

	UPROPERTY(BlueprintAssignable, Category = "0xSequence SDK - Events")
	FOnSession OnSessionClosed;

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Ecosystem Wallet")
	void AddSession(const TScriptInterface<IPermissions>& Permissions, FOnSuccess OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Ecosystem Wallet")
	void SignMessage(const FString& Message, FOnSignature OnSuccess, FOnFailure OnFailure);
	
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Ecosystem Wallet")
	void GetFeeOptions(const TScriptInterface<ISeqTransactionBase>& Transaction, FOnFeeOptions OnSuccess, FOnFailure OnFailure);
	
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Ecosystem Wallet")
	void SendTransaction(const TScriptInterface<ISeqTransactionBase>& Transaction, const FFeeOption FeeOption, FOnTransaction OnSuccess, FOnFailure OnFailure);
	
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Ecosystem Wallet")
	void SendTransactionWithoutPermissions(const TScriptInterface<ISeqTransactionBase>& Transaction, FOnTransaction OnSuccess, FOnFailure OnFailure);
	
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Ecosystem Wallet")
	void ClearSessions();

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Ecosystem Wallet")
	FWalletInfo GetWalletInfo();

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Ecosystem Wallet")
	bool CheckIfWalletExists();
	
	USequenceWallet* GetSequenceEcosystemWallet() const;

private:
	UPROPERTY()
	USequenceWallet* Wallet = nullptr;

	void CallOnSessionClosed() const;
};
