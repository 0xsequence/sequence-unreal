#pragma once

UENUM(Blueprintable)
enum class EStepType : uint8
{
	Unknown				UMETA(DisplayName = "unknown"),
	TokenApproval		UMETA(DisplayName = "tokenApproval"),
	Buy					UMETA(DisplayName = "buy"),
	Sell				UMETA(DisplayName = "sell"),
	CreateListing		UMETA(DisplayName = "createListing"),
	CreateOffer			UMETA(DisplayName = "createOffer"),
	SignEip712			UMETA(DisplayName = "signEIP712"),
	SignEip191			UMETA(DisplayName = "signEIP191"),
};
