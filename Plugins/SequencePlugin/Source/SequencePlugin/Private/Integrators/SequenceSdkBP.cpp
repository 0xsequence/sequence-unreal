// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "Integrators/SequenceSdkBP.h"

#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Util/Log.h"

USequenceSdkBP::USequenceSdkBP() { }

void USequenceSdkBP::SetupAsync()
{
	// TODO: Integrate async setup process like querying remote chain configs
	this->CallSetupFinished();
}

void USequenceSdkBP::CallSetupFinished() const
{
	if (this->SetupFinished.IsBound())
		this->SetupFinished.Broadcast();
	else
		SEQ_LOG(Error, TEXT("Nothing bound to delegate: SetupFinished"));
}
