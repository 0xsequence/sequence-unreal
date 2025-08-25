// Fill out your copyright notice in the Description page of Project Settings.

#include "EcosystemWallet/SequenceEcosystemWalletBP.h"
#include "Util/Async.h"

USequenceEcosystemWalletBP::USequenceEcosystemWalletBP() { }

void USequenceEcosystemWalletBP::SignMessage(const FString& Message, FOnSuccess OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<bool> SuccessCallback = [OnSuccess](bool Result)
	{
		OnSuccess.ExecuteIfBound();
	};
	
	const FFailureCallback FailureCallback = [OnFailure](const FSequenceError& Error)
	{
		OnFailure.ExecuteIfBound(Error.Message);	
	};
	
	this->GetSequenceEcosystemWallet()->SignMessage(Message, SuccessCallback, FailureCallback);	
}

USequenceEcosystemWallet* USequenceEcosystemWalletBP::GetSequenceEcosystemWallet() const
{
	return NewObject<USequenceEcosystemWallet>();
}
