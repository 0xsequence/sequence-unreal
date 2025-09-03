#pragma once

#include "CoreMinimal.h"
#include "Permissions.h"
#include "UObject/Object.h"
#include "ContractPermissions.generated.h"

UCLASS()
class SEQUENCEPLUGIN_API UContractPermissions : public UObject, public IPermissions
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Permissions")
	static UContractPermissions* CreateContractPermissions(const int64 ChainId, const FString Target,
		const int64 Deadline, const int64 ValueLimit);
	
	virtual FSessionPermissions GetSessionPermissions() override;

	int64 ChainId;
	FString Target;
	int64 Deadline;
	int64 ValueLimit;
};
