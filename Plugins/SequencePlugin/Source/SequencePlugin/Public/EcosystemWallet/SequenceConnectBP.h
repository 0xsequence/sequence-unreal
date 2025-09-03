#pragma once

#include "CoreMinimal.h"
#include "SequenceConnect.h"
#include "Permissions/Permissions.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SequenceConnectBP.generated.h"

UCLASS(Blueprintable)
class SEQUENCEPLUGIN_API USequenceConnectBP : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSession);
	
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnFailure, const FString&, Error);
	DECLARE_DYNAMIC_DELEGATE(FOnSuccess);
	
	USequenceConnectBP();

	UPROPERTY(BlueprintAssignable, Category = "0xSequence SDK - Events")
	FOnSession OnSessionCreated;

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Ecosystem Wallet")
	void SignInWithEmail(const FString& Email, const TScriptInterface<IPermissions>& Permissions, FOnSuccess OnSuccess, FOnFailure OnFailure);
	
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Ecosystem Wallet")
	void SignInWithGoogle(const TScriptInterface<IPermissions>& Permissions, FOnSuccess OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Ecosystem Wallet")
	void SignInWithApple(const TScriptInterface<IPermissions>& Permissions, FOnSuccess OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Ecosystem Wallet")
	void SignInWithPasskey(const TScriptInterface<IPermissions>& Permissions, FOnSuccess OnSuccess, FOnFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Ecosystem Wallet")
	void SignInWithMnemonic(const TScriptInterface<IPermissions>& Permissions, FOnSuccess OnSuccess, FOnFailure OnFailure);

	USequenceConnect* GetSequenceConnect() const;
	
private:
	UPROPERTY()
	USequenceConnect* SequenceConnect;

	void CallOnSessionCreated() const;
};
