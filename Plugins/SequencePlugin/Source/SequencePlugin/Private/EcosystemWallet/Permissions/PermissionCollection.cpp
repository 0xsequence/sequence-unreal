#include "EcosystemWallet/Permissions/PermissionCollection.h"
#include "EcosystemWallet/Primitives/Permission/Permission.h"
#include "EcosystemWallet/Primitives/Permission/SessionPermissions.h"

UPermissionCollection* UPermissionCollection::CreatePermissionCollection()
{
	return NewObject<UPermissionCollection>();
}

void UPermissionCollection::AddPermission(const TScriptInterface<IPermissions>& NewPermission)
{
	Permissions.Add(NewPermission);
}

FSessionPermissions UPermissionCollection::GetSessionPermissions()
{
	if (Permissions.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Permission Collection is empty"));
		return FSessionPermissions();
	}
	
	TArray<FPermission> AllPermissions;
	for (TScriptInterface<IPermissions>& Permission : Permissions)
	{
		if (!Permission || Permission.GetInterface() == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Null or invalid permission interface in collection"));
			continue;
		}
		
		FSessionPermissions CurrentSessionPermissions = Permission->GetSessionPermissions();
		for (FPermission ChildPermission : CurrentSessionPermissions.Permissions)
		{
			AllPermissions.Add(ChildPermission);		
		}
	}

	if (!Permissions[0] || Permissions[0].GetInterface() == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("First permission is invalid"));
		return FSessionPermissions();
	}

	FSessionPermissions RootPermissions = Permissions[0]->GetSessionPermissions();

	FSessionPermissions SessionPermissions;
	SessionPermissions.ChainId = RootPermissions.ChainId;
	SessionPermissions.Deadline = RootPermissions.Deadline;
	SessionPermissions.ValueLimit = RootPermissions.ValueLimit;
	SessionPermissions.Permissions = AllPermissions; 

	return SessionPermissions;
}
