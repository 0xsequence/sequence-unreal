// Fill out your copyright notice in the Description page of Project Settings.


#include "EcosystemWallet/SequenceEcosystemWalletBP.h"

void USequenceEcosystemWalletBP::SignMessage(const FString& Message)
{
	this->GetSequenceEcosystemWallet()->SignMessage(Message);	
}

USequenceEcosystemWallet* USequenceEcosystemWalletBP::GetSequenceEcosystemWallet() const
{
	return NewObject<USequenceEcosystemWallet>();
}
