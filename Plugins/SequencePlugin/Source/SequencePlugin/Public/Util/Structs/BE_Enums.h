#pragma once

#include "CoreMinimal.h"
#include "BE_Enums.generated.h"

UENUM(BlueprintType)
enum ESocialSigninType
{
	Google UMETA(DisplayName = "Google"),
	FaceBook UMETA(DisplayName = "FaceBook"),
	Discord UMETA(DisplayName = "Discord"),
	Apple UMETA(DisplayName = "Apple"),
};

UENUM(BlueprintType)
enum ENetwork
{
    Ethereum           UMETA(DisplayName = "Ethereum"),
    Immutable          UMETA(DisplayName = "Immutable"),
    SkaleNebula        UMETA(DisplayName = "SKALE Nebula"),
    Optimism           UMETA(DisplayName = "Optimism"),
    BNBSmartChain      UMETA(DisplayName = "BNB Smart Chain"),
    GnosisChain        UMETA(DisplayName = "Gnosis Chain"),
    PolygonChain	   UMETA(DisplayName = "Polygon"),
    PolygonZkEVM       UMETA(DisplayName = "Polygon zkEVM"),
	B3				   UMETA(DisplayName = "B3"),
    Base               UMETA(DisplayName = "Base"),
    OasysHomeverse     UMETA(DisplayName = "Oasys Homeverse"),
	APEChain		   UMETA(DisplayName = "APE Chain"),
    ArbitrumOne        UMETA(DisplayName = "Arbitrum One"),
    ArbitrumNova       UMETA(DisplayName = "Arbitrum Nova"),
    Avalanche          UMETA(DisplayName = "Avalanche"),
    Blast              UMETA(DisplayName = "Blast"),
    Xai                UMETA(DisplayName = "Xai"),
    LAOS               UMETA(DisplayName = "LAOS"),
    Root        UMETA(DisplayName = "Root Network"),
	Soneium			   UMETA(DisplayName = "Soneium"),
	Telos              UMETA(DisplayName = "Telos"),
	Moonbeam           UMETA(DisplayName = "Moonbeam"),
	Etherlink          UMETA(DisplayName = "Etherlink"),
	XR1				   UMETA(DisplayName = "XR1"),
	Somnia				UMETA(DisplayName = "Somnia"),

	ImmutableTestnet  UMETA(DisplayName = "Immutable Testnet"),
    BNBSmartChainTestnet  UMETA(DisplayName = "BNB Smart Chain Testnet"),
	SoneiumMinatoTestnet  UMETA(DisplayName = "Soneium Minato Testnet"),
    B3Sepolia          UMETA(DisplayName = "B3 Sepolia"),
    XRSepolia          UMETA(DisplayName = "XR Sepolia"),
    APEChainTestnet    UMETA(DisplayName = "APE Chain Testnet"),
    OasysHomeverseTestnet UMETA(DisplayName = "Oasys Homeverse Testnet"),
    AvalancheTestnet    UMETA(DisplayName = "Avalanche Testnet"),
    PolygonAmoy        UMETA(DisplayName = "Polygon Amoy"),
    BaseSepolia        UMETA(DisplayName = "Base Sepolia"),
    ArbitrumSepolia    UMETA(DisplayName = "Arbitrum Sepolia"),
    Sepolia            UMETA(DisplayName = "Sepolia"),
    OptimismSepolia    UMETA(DisplayName = "Optimism Sepolia"),
	TOYTestnet 		   UMETA(DisplayName = "TOY Testnet"),
	SKALENebulaGamingHubTestnet UMETA(DisplayName = "SKALE Nebula Gaming Hub Testnet"),
    BlastSepolia       UMETA(DisplayName = "Blast Sepolia"),
    XaiSepolia         UMETA(DisplayName = "XAI Sepolia"),
    LAOSSigma          UMETA(DisplayName = "LAOS Sigma Testnet"),
    RootPorcini        UMETA(DisplayName = "Root Network Porcini Testnet"),
    TelosTestnet        UMETA(DisplayName = "Telos Testnet"),
	MoonbaseAlpha      UMETA(DisplayName = "Moonbase Alpha"),
	EtherlinkTestnet   UMETA(DisplayName = "Etherlink Testnet"),
	MonadTestnet		UMETA(DisplayName = "Monad Testnet"),
	SomniaTestnet 		UMETA(DisplayName = "Somnia Testnet"),
	FrequencyTestnet    UMETA(DisplayName = "Frequency Testnet"),
	IncentivTestnet     UMETA(DisplayName = "Incentiv Testnet"),
	IncentivTestnetV2     UMETA(DisplayName = "Incentiv Testnet v2"),
};