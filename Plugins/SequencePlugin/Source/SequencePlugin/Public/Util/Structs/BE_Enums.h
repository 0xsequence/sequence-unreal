// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
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
	BorneTestnet	   UMETA(DisplayName = "Borne Testnet"),
    ArbitrumSepolia    UMETA(DisplayName = "Arbitrum Sepolia"),
    Sepolia            UMETA(DisplayName = "Sepolia"),
    OptimismSepolia    UMETA(DisplayName = "Optimism Sepolia"),
	TOYTestnet 		   UMETA(DisplayName = "TOY Testnet"),
	SKALENebulaGamingHubTestnet UMETA(DisplayName = "SKALE Nebula Gaming Hub Testnet"),
    BlastSepolia       UMETA(DisplayName = "Blast Sepolia"),
    XaiSepolia         UMETA(DisplayName = "XAI Sepolia")
};