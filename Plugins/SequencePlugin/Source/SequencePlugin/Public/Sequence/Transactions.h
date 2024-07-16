// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "FeeOption.h"
#include "Types/Types.h"
#include "UObject/NoExportTypes.h"
#include "Util/Structs/BE_Structs.h"
#include "Transactions.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class SEQUENCEPLUGIN_API UTransactions : public UObject
{
	GENERATED_BODY()
private:
	TArray<TransactionUnion> Transactions;

	//By default we will assume our txn is gassed up
	bool FeeSet = false;

	//If we are using a fee we store it here
	FFeeOption CachedFee;

public:

	UFUNCTION(BlueprintCallable, Category = "Default")
	void AddERC20(const FERC20Transaction& Item);
	
	UFUNCTION(BlueprintCallable, Category = "Default")
	void AddERC721(const FERC721Transaction& Item);

	UFUNCTION(BlueprintCallable, Category = "Default")
	void AddERC1155(const FERC1155Transaction& Item);

	UFUNCTION(BlueprintCallable, Category = "Default")
	void AddRaw(const FRawTransaction& Item);

	UFUNCTION(BlueprintCallable, Category = "Default")
	void AddDelayed(const FDelayedTransaction& Item);

	UFUNCTION(BlueprintCallable, Category = "Default")
	void SetFee(const FFeeOption& Fee);

	bool IsFeeSet() const;

	FFeeOption GetFee();

	TArray<TransactionUnion> GetTransactions();
};
