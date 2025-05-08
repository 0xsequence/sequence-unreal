#include "ConfigFetcher.h"
#include "Misc/ConfigCacheIni.h"
#include "CoreGlobals.h"
#include "Util/Log.h"

FString UConfigFetcher::GetConfigVar(const FString& ConfigKey)
{
	if(!GConfig)
	{
		SEQ_LOG(Error, TEXT("[GConfig Error]"));
		return "";
	}
 
	FString value;
	const FString Section = "/Script/Sequence.Config";
	const FString Filename = FConfigCacheIni::NormalizeConfigIniPath(FPaths::ProjectConfigDir() + TEXT("/SequenceConfig.ini"));
	GConfig->Flush(true,Filename);
	if (GConfig->GetString(GetData(Section), GetData(ConfigKey),value,GetData(Filename)))
	{
		return value;
	}
	
	SEQ_LOG(Error, TEXT("[Error reading config file, Please ensure SequenceConfig.ini is setup correctly]"));
	return "";
}

