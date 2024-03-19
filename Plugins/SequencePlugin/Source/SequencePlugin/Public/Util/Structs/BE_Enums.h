#pragma once

#include "CoreMinimal.h"
#include "BE_Enums.generated.h"

UENUM(BlueprintType)
enum ETXNType
{
	//Unreal Disallows duplicate enumeration values within separate enums so I use a cmlCase Suffix to differentiate
	TXN_Send UMETA(DisplayName = "Send"),
	TXN_Receive UMETA(DisplayName = "Receive"),
	TXN_Swap UMETA(DisplayName = "Swap")
};

UENUM(BlueprintType)
enum EHistoryItemType
{
	Currency_HIT UMETA(DisplayName = "Currency"),
	Token_HIT UMETA(DisplayName = "Token"),
	Collection_HIT UMETA(DisplayName = "Collection")
};

UENUM(BlueprintType)
enum ESocialSigninType
{
	Google UMETA(DisplayName = "Google"),
	FaceBook UMETA(DisplayName = "FaceBook"),
	Discord UMETA(DisplayName = "Discord"),
	Apple UMETA(DisplayName = "Apple"),
};