#include "EcosystemWallet/SequenceConnect.h"
#include "EcosystemClient.h"

USequenceConnect::USequenceConnect()
{
	this->Client = NewObject<UEcosystemClient>();
}

void USequenceConnect::SignInWithGoogle(const TSuccessCallback<bool>& OnSuccess, const FFailureCallback& OnFailure)
{
	FPermission ContractPermission;
	ContractPermission.Target = "0x33985d320809E26274a72E03268c8a29927Bc6dA";
	
	TArray<FPermission> Things;
	Things.Add(ContractPermission);

	FSessionPermissions Permissions;
	Permissions.ChainId = 421614;
	Permissions.Deadline = 1856724472000;
	Permissions.ValueLimit = 0;
	Permissions.Permissions = Things; 
	
	this->Client->CreateNewSession(ESessionCreationType::CreateNewSession, "google",
		"", Permissions, OnSuccess, OnFailure);
}
