#include "EcosystemWallet/Permissions/ContractPermissions.h"
#include "EcosystemWallet/Primitives/Permission/Permission.h"
#include "EcosystemWallet/Primitives/Permission/SessionPermissions.h"

UContractPermissions* UContractPermissions::CreateContractPermissions(const int64 ChainId, const FString Target,
	const int64 Deadline, const int64 ValueLimit)
{
	UContractPermissions* Permissions = NewObject<UContractPermissions>();
	Permissions->ChainId = ChainId;
	Permissions->Target = Target;
	Permissions->Deadline = Deadline;
	Permissions->ValueLimit = ValueLimit;

	return Permissions;
}

FSessionPermissions UContractPermissions::GetSessionPermissions()
{
	FPermission ContractPermission;
	ContractPermission.Target = this->Target;
	
	TArray<FPermission> Things;
	Things.Add(ContractPermission);

	FSessionPermissions Permissions;
	Permissions.ChainId = FString::Printf(TEXT("%lld"), this->ChainId);
	Permissions.Deadline = FString::Printf(TEXT("%lld"), this->Deadline);
	Permissions.ValueLimit = FString::Printf(TEXT("%lld"), this->ValueLimit);
	Permissions.Permissions = Things; 

	return Permissions;
}
