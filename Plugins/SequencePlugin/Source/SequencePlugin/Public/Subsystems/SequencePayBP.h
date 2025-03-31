// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Sequence/SequencePay.h"
#include "Sequence/SequenceWalletBPTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UObject/Object.h"
#include "SequencePayBP.generated.h"

UCLASS(Blueprintable)
class SEQUENCEPLUGIN_API USequencePayBP : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnSupportedTransakCountries, const TArray<FSupportedCountry>&, SupportedCountries);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnFailure, const FString&, Error);

public:
	USequencePayBP();
	
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	FString GetOnRampLink(const FString& WalletAddress, const FTransakSettings& Settings);
	
	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void OpenOnRampInExternalBrowser(const FString& WalletAddress, const FTransakSettings& Settings);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	void GetSupportedOnRampCountries(FOnSupportedTransakCountries OnSuccess, FOnFailure OnFailure);

private:
	UPROPERTY()
	USequencePay* Pay;
};
