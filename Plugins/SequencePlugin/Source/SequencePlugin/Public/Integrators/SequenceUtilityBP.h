// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "Sequence/Transactions.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Util/Structs/BE_Structs.h"
#include "SequenceUtilityBP.generated.h"

UCLASS(Blueprintable)
class SEQUENCEPLUGIN_API USequenceUtilityBP : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	USequenceUtilityBP();

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	UTransactions* ConstructSingleERC20Transaction(const FString& ContractAddress, const FString& RecipientAddress, const FString& Value);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	UTransactions* ConstructSingleERC721Transaction(const FString& ContractAddress, const FString& RecipientAddress, const FString& TokenId);

	UFUNCTION(BlueprintCallable, Category="0xSequence SDK - Functions")
	UTransactions* ConstructSingleERC1155Transaction(const FString& ContractAddress, const FString& RecipientAddress, const FString& TokenId, const FString& Amount);

private:
	static USequenceUtilityBP* GetSubSystem();
};
