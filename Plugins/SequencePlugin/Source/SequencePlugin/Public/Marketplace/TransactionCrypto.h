// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "TransactionCrypto.generated.h"

ENUM_RANGE_BY_COUNT(ETransactionCrypto, 4)
UENUM(Blueprintable)
enum class ETransactionCrypto : uint8
{
	Unknown			UMETA(DisplayName = "unknown"),
	None			UMETA(DisplayName = "none"),
	Partially		UMETA(DisplayName = "partially"),
	All				UMETA(DisplayName = "all")
};

UCLASS()
class SEQUENCEPLUGIN_API UTransactionCrypto : public UObject
{
	GENERATED_BODY()
	
public:
	static ETransactionCrypto GetTransactionCryptoFromString(const FString& TransactionCryptoString)
	{
		FText EnumDisplayText = FText::FromString("");
		for (const ETransactionCrypto Val : TEnumRange<ETransactionCrypto>())
		{
			UEnum::GetDisplayValueAsText(Val, EnumDisplayText);
			
			if (UEnum::GetValueAsString(Val).Equals(TransactionCryptoString) || EnumDisplayText.ToString().Equals(TransactionCryptoString))
			{
				return Val;
			}
		}

		// Fallback if nothing is found
		constexpr ETransactionCrypto TransactionCryptoDefault{ ETransactionCrypto::Unknown };
		return TransactionCryptoDefault;
	}
	
	static FString AsString(const ETransactionCrypto TransactionCrypto)
	{
		switch (TransactionCrypto)
		{
			case ETransactionCrypto::None: return "none";
			case ETransactionCrypto::Partially: return "partially";
			case ETransactionCrypto::All: return "all";
		default: return "unknown";
		}
	}
};
