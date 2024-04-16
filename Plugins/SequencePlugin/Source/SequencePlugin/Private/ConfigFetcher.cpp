// Fill out your copyright notice in the Description page of Project Settings.


#include "ConfigFetcher.h"

bool UConfigFetcher::CriticalConfigVarsValidated()
{
	
	return false;
}
	
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
	if (GConfig->GetString(GetData(Section), GetData(ConfigKey),value,GetData(Filename)))
	{
		UE_LOG(LogTemp, Log, TEXT("%s"),*value);
		return value;
	}
	
	UE_LOG(LogTemp, Error, TEXT("[Error reading config file, Please ensure SequenceConfig.ini is setup correctly]"));
	return "";
}

