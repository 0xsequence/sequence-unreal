// Copyright Epic Games, Inc. All Rights Reserved.

#include "SequenceUnreal.h"
#include "Modules/ModuleManager.h"
#include "ABI/ABI.h"
#include "Sequence/SequenceAPI.h"

IMPLEMENT_PRIMARY_GAME_MODULE(FDefaultGameModuleImpl, SequenceUnreal, "SequenceUnreal");

void Test()
{
	TArray<FIdNamePair> Networks = USequenceWallet::GetAllNetworks();
	TArray<FString> NetworkNames = USequenceWallet::GetAllNetworkNames();
	TArray<int64> NetworkIds = USequenceWallet::GetAllNetworkIds();
	FString NetworkName = USequenceWallet::GetNetworkName(137);
	int64 NetworkId = USequenceWallet::GetNetworkId(TEXT("polygon"));
	int64 SystemReadableAmount = USequenceWallet::GetSystemReadableAmount(0.01, 6);//0.01 USDC
	float UserReadableAmount = USequenceWallet::GetUserReadableAmount(1000000, 6);//1 USDC
}