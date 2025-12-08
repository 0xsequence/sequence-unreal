#include "ChainCollection.h"

TArray<FChainConfig> FChainCollection::ChainConfigs = {
    FChainConfig(ENetwork::Ethereum, TEXT("1"), TEXT("Ethereum"), TEXT("mainnet")),
    FChainConfig(ENetwork::Immutable, TEXT("13371"), TEXT("Immutable"), TEXT("immutable-zkevm")),
    FChainConfig(ENetwork::SkaleNebula, TEXT("1482601649"), TEXT("Skale Nebula"), TEXT("skale-nebula")),
    FChainConfig(ENetwork::Optimism, TEXT("10"), TEXT("Optimism"), TEXT("optimism")),
    FChainConfig(ENetwork::BNBSmartChain, TEXT("56"), TEXT("BNB Smart Chain"), TEXT("bsc")),
    FChainConfig(ENetwork::GnosisChain, TEXT("100"), TEXT("Gnosis Chain"), TEXT("gnosis")),
    FChainConfig(ENetwork::PolygonChain, TEXT("137"), TEXT("Polygon"), TEXT("polygon")),
    FChainConfig(ENetwork::PolygonZkEVM, TEXT("1101"), TEXT("Polygon zkEVM"), TEXT("polygon-zkevm")),
    FChainConfig(ENetwork::B3, TEXT("8333"), TEXT("B3"), TEXT("b3")),
    FChainConfig(ENetwork::Base, TEXT("8453"), TEXT("Base"), TEXT("base")),
    FChainConfig(ENetwork::OasysHomeverse, TEXT("19011"), TEXT("Oasys Homeverse"), TEXT("homeverse")),
    FChainConfig(ENetwork::APEChain, TEXT("33139"), TEXT("APE Chain"), TEXT("apechain")),
    FChainConfig(ENetwork::ArbitrumOne, TEXT("42161"), TEXT("Arbitrum One"), TEXT("arbitrum")),
    FChainConfig(ENetwork::ArbitrumNova, TEXT("42170"), TEXT("Arbitrum Nova"), TEXT("arbitrum-nova")),
    FChainConfig(ENetwork::Avalanche, TEXT("43114"), TEXT("Avalanche"), TEXT("avalanche")),
    FChainConfig(ENetwork::Blast, TEXT("81457"), TEXT("Blast"), TEXT("blast")),
    FChainConfig(ENetwork::Xai, TEXT("660279"), TEXT("Xai"), TEXT("xai")),
    FChainConfig(ENetwork::LAOS, TEXT("6283"), TEXT("LAOS"), TEXT("laos")),
    FChainConfig(ENetwork::Root, TEXT("7668"), TEXT("Root Network"), TEXT("rootnet")),
    FChainConfig(ENetwork::Soneium, TEXT("1868"), TEXT("Soneium"), TEXT("soneium")),
    FChainConfig(ENetwork::Telos, TEXT("40"), TEXT("Telos"), TEXT("telos")),
    FChainConfig(ENetwork::Moonbeam, TEXT("1284"), TEXT("Moonbeam"), TEXT("moonbeam")),
    FChainConfig(ENetwork::Etherlink, TEXT("42793"), TEXT("Etherlink"), TEXT("etherlink")),
    FChainConfig(ENetwork::XR1, TEXT("273"), TEXT("XR1"), TEXT("xr1")),
    FChainConfig(ENetwork::Somnia, TEXT("5031"), TEXT("Somnia"), TEXT("somnia")),

    // --- TESTNETS ---
    FChainConfig(ENetwork::Sepolia, TEXT("11155111"), TEXT("Sepolia"), TEXT("sepolia")),
    FChainConfig(ENetwork::ArbitrumSepolia, TEXT("421614"), TEXT("Arbitrum Sepolia"), TEXT("arbitrum-sepolia")),
    FChainConfig(ENetwork::BNBSmartChainTestnet, TEXT("97"), TEXT("BNB Testnet"), TEXT("bsc-testnet")),
    FChainConfig(ENetwork::BaseSepolia, TEXT("84532"), TEXT("Base Sepolia"), TEXT("base-sepolia")),
    FChainConfig(ENetwork::OasysHomeverseTestnet, TEXT("40875"), TEXT("Homeverse Testnet"), TEXT("homeverse-testnet")),
    FChainConfig(ENetwork::AvalancheTestnet, TEXT("43113"), TEXT("Avalanche Testnet"), TEXT("avalanche-testnet")),
    FChainConfig(ENetwork::OptimismSepolia, TEXT("11155420"), TEXT("Optimism Sepolia"), TEXT("optimism-sepolia")),
    FChainConfig(ENetwork::PolygonAmoy, TEXT("80002"), TEXT("Polygon Amoy"), TEXT("amoy")),
    FChainConfig(ENetwork::XRSepolia, TEXT("2730"), TEXT("XR Sepolia"), TEXT("xr-sepolia")),
    FChainConfig(ENetwork::XaiSepolia, TEXT("37714555429"), TEXT("Xai Sepolia"), TEXT("xai-sepolia")),
    FChainConfig(ENetwork::B3Sepolia, TEXT("1993"), TEXT("B3 Sepolia"), TEXT("b3-sepolia")),
    FChainConfig(ENetwork::APEChainTestnet, TEXT("33111"), TEXT("APE Chain Testnet"), TEXT("apechain-testnet")),
    FChainConfig(ENetwork::BlastSepolia, TEXT("168587773"), TEXT("Blast Sepolia"), TEXT("blast-sepolia")),
    FChainConfig(ENetwork::SKALENebulaGamingHubTestnet, TEXT("37084624"), TEXT("Skale Nebula Testnet"), TEXT("skale-nebula-testnet")),
    FChainConfig(ENetwork::SoneiumMinatoTestnet, TEXT("1946"), TEXT("Soneium Minato"), TEXT("soneium-minato")),
    FChainConfig(ENetwork::TOYTestnet, TEXT("21000000"), TEXT("TOY Testnet"), TEXT("toy-testnet")),
    FChainConfig(ENetwork::ImmutableTestnet, TEXT("13473"), TEXT("Immutable Testnet"), TEXT("immutable-zkevm-testnet")),
    FChainConfig(ENetwork::TelosTestnet, TEXT("41"), TEXT("Telos Testnet"), TEXT("telos-testnet")),
    FChainConfig(ENetwork::MoonbaseAlpha, TEXT("1287"), TEXT("Moonbase Alpha"), TEXT("moonbase-alpha")),
    FChainConfig(ENetwork::EtherlinkTestnet, TEXT("128123"), TEXT("Etherlink Testnet"), TEXT("etherlink-testnet")),
    FChainConfig(ENetwork::MonadTestnet, TEXT("10143"), TEXT("Monad Testnet"), TEXT("monad-testnet")),
    FChainConfig(ENetwork::SomniaTestnet, TEXT("50312"), TEXT("Somnia Testnet"), TEXT("somnia-testnet")),
    FChainConfig(ENetwork::FrequencyTestnet, TEXT("53716"), TEXT("Frequency Testnet"), TEXT("frequency-testnet")),
    FChainConfig(ENetwork::IncentivTestnet, TEXT("11690"), TEXT("Incentiv Testnet"), TEXT("incentiv-testnet")),
    FChainConfig(ENetwork::IncentivTestnetV2, TEXT("28802"), TEXT("Incentiv v2 Testnet"), TEXT("incentiv-testnet-v2")),
    FChainConfig(ENetwork::SandboxTestnet, TEXT("6252"), TEXT("Sandbox Testnet"), TEXT("sandbox-testnet"))
};

TMap<ENetwork, FString> FChainCollection::NameOf;
TMap<ENetwork, FString> FChainCollection::PathOf;
TMap<ENetwork, FString> FChainCollection::ChainIdOf;
TMap<FString, ENetwork> FChainCollection::ChainById;

void FChainCollection::Initialize()
{
    for (const FChainConfig& Config : ChainConfigs)
    {
        NameOf.Add(Config.Network, Config.Name);
        PathOf.Add(Config.Network, Config.Path);
        ChainIdOf.Add(Config.Network, Config.ChainId);
        ChainById.Add(Config.ChainId, Config.Network);
    }
}

FString FChainCollection::GetNetworkName(const FString NetworkIdIn)
{
    if (const ENetwork* Found = ChainById.Find(NetworkIdIn))
    {
        if (const FString* Name = NameOf.Find(*Found))
        {
            return *Name;
        }
    }
    return TEXT("");
}

FString FChainCollection::GetNetworkName(const ENetwork NetworkIn)
{
    if (const FString* Name = NameOf.Find(NetworkIn))
    {
        return *Name;
    }
    return TEXT("");
}

FString FChainCollection::GetNetworkNameForUrl(const FString NetworkIdIn)
{
    if (const ENetwork* Found = ChainById.Find(NetworkIdIn))
    {
        if (const FString* Path = PathOf.Find(*Found))
        {
            return *Path; // URL-friendly path
        }
    }
    return TEXT("");
}

bool FChainCollection::IsNetworkIdSupported(const FString NetworkIdIn)
{
    return ChainById.Contains(NetworkIdIn);
}

int64 FChainCollection::GetNetworkId(const FString& NetworkNameIn)
{
    for (const FChainConfig& Config : ChainConfigs)
    {
        if (Config.Name.Equals(NetworkNameIn, ESearchCase::IgnoreCase))
        {
            return FCString::Atoi64(*Config.ChainId);
        }
    }
    return -1;
}

int64 FChainCollection::GetNetworkId(const ENetwork& Network)
{
    if (const FString* Id = ChainIdOf.Find(Network))
    {
        return FCString::Atoi64(**Id);
    }
    return -1;
}

TArray<FString> FChainCollection::GetAllNetworks()
{
    TArray<FString> Out;
    for (const FChainConfig& Config : ChainConfigs)
    {
        Out.Add(Config.ChainId);
    }
    return Out;
}

TArray<FString> FChainCollection::GetAllNetworkNames()
{
    TArray<FString> Out;
    for (const FChainConfig& Config : ChainConfigs)
    {
        Out.Add(Config.Name);
    }
    return Out;
}

TArray<FString> FChainCollection::GetAllNetworkIds()
{
    TArray<FString> Out;
    for (const FChainConfig& Config : ChainConfigs)
    {
        Out.Add(Config.ChainId);
    }
    return Out;
}
