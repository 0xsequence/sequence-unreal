#pragma once

#include "CoreMinimal.h"
#include "SequenceEcosystemWallet.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SequenceEcosystemWalletBP.generated.h"

UCLASS(Blueprintable)
class SEQUENCEPLUGIN_API USequenceEcosystemWalletBP : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnFailure, const FString&, Error);
	DECLARE_DYNAMIC_DELEGATE(FOnSuccess);
	
	USequenceEcosystemWalletBP();

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Ecosystem Wallet")
	void SignMessage(const FString& Message, FOnSuccess OnSuccess, FOnFailure OnFailure);
	
	USequenceEcosystemWallet* GetSequenceEcosystemWallet() const;
};
