// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "ConfigFetcher.h"
#include "Misc/ConfigCacheIni.h"
#include "CoreGlobals.h"
	
FString UConfigFetcher::GetConfigVar(const FString& ConfigKey)
{
	if(!GConfig)
	{
		UE_LOG(LogTemp, Error, TEXT("[GConfig Error]"));
		return "";
	}
 
	FString value;
	const FString Section = "/Script/Sequence.Config";
	const FString Filename = FConfigCacheIni::NormalizeConfigIniPath(FPaths::ProjectConfigDir() + TEXT("/SequenceConfig.ini"));
	GConfig->Flush(true,Filename);
	if (GConfig->GetString(GetData(Section), GetData(ConfigKey),value,GetData(Filename)))
	{
		UE_LOG(LogTemp, Log, TEXT("%s"),*value);
		return value;
	}
	
	UE_LOG(LogTemp, Error, TEXT("[Error reading config file, Please ensure SequenceConfig.ini is setup correctly]"));
	return "";
}

