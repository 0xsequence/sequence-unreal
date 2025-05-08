

#pragma once
#include "CoreMinimal.h"
#include "FeeOption.h"
#include "Types/Types.h"
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

	UFUNCTION(BlueprintCallable, Category = "0xSequence")
	void AddERC20(const FERC20Transaction& Item);
	
	UFUNCTION(BlueprintCallable, Category = "0xSequence")
	void AddERC721(const FERC721Transaction& Item);

	UFUNCTION(BlueprintCallable, Category = "0xSequence")
	void AddERC1155(const FERC1155Transaction& Item);

	UFUNCTION(BlueprintCallable, Category = "0xSequence")
	void AddRaw(const FRawTransaction& Item);

	UFUNCTION(BlueprintCallable, Category = "0xSequence")
	void AddDelayed(const FDelayedTransaction& Item);

	UFUNCTION(BlueprintCallable, Category = "0xSequence")
	void SetFee(const FFeeOption& Fee);

	bool IsFeeSet() const;

	FFeeOption GetFee();

	TArray<TransactionUnion> GetTransactions();
};
