#pragma once

#include "CoreMinimal.h"
#include "SequenceConnect.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SequenceConnectBP.generated.h"

UCLASS(Blueprintable)
class SEQUENCEPLUGIN_API USequenceConnectBP : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnFailure, const FString&, Error);
	DECLARE_DYNAMIC_DELEGATE(FOnSuccess);
	
	USequenceConnectBP();

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Ecosystem Wallet")
	void SignInWithGoogle();

	USequenceConnect* GetSequenceConnect() const;
	
private:
	UPROPERTY()
	USequenceConnect* SequenceConnect;
};
