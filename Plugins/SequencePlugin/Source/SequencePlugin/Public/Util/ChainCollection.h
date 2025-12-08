#pragma once
#include "Structs/BE_Enums.h"

struct SEQUENCEPLUGIN_API FChainConfig
{
	ENetwork Network;
	FString ChainId;
	FString Name;
	FString Path;

	FChainConfig() {}

	FChainConfig(ENetwork InNetwork, FString InChainId, FString InName, FString InPath)
		: Network(InNetwork)
		, ChainId(InChainId)
		, Name(InName)
		, Path(InPath)
	{}
};

class SEQUENCEPLUGIN_API FChainCollection
{
public:
	static TArray<FChainConfig> ChainConfigs;

	static TMap<ENetwork, FString> NameOf;
	static TMap<ENetwork, FString> PathOf;
	static TMap<ENetwork, FString> ChainIdOf;
	static TMap<FString, ENetwork> ChainById;

	static void Initialize();

	static FString GetNetworkName(const FString NetworkIdIn);
	static FString GetNetworkName(const ENetwork NetworkIn);

	static FString GetNetworkNameForUrl(const FString NetworkIdIn);

	static bool IsNetworkIdSupported(const FString NetworkIdIn);

	static int64 GetNetworkId(const FString& NetworkNameIn);
	static int64 GetNetworkId(const ENetwork& Network);

	static TArray<FString> GetAllNetworks();
	static TArray<FString> GetAllNetworkNames();
	static TArray<FString> GetAllNetworkIds();
};
