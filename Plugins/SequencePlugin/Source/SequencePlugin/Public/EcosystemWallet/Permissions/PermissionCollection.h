#pragma once

#include "CoreMinimal.h"
#include "Permissions.h"
#include "UObject/Object.h"
#include "PermissionCollection.generated.h"

UCLASS()
class SEQUENCEPLUGIN_API UPermissionCollection : public UObject, public IPermissions
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Permissions")
	static UPermissionCollection* CreatePermissionCollection();

	UFUNCTION(BlueprintCallable, Category = "0xSequence SDK - Permissions")
	void AddPermission(const TScriptInterface<IPermissions>& NewPermission);
	
	virtual FSessionPermissions GetSessionPermissions() override;

private:
	UPROPERTY()
	TArray<TScriptInterface<IPermissions>> Permissions;
};
