#pragma once
#include "EcosystemWallet/Primitives/Permission/SessionPermissions.h"
#include "EcosystemWallet/Primitives/Sessions/SessionsLeaf.h"

class SEQUENCEPLUGIN_API FSessionsPermissionsLeaf final : public FSessionsLeaf
{
public:
	explicit FSessionsPermissionsLeaf(const FSessionPermissions& InPermissions)
	{
		this->Type = ESessionsLeafType::Permissions;
		this->Permissions = InPermissions;
	}
	
	virtual TArray<uint8> Encode() override
	{
		return FSessionsLeaf::Encode();
	}
	
	virtual TArray<uint8> EncodeForHash() override
	{
		return FSessionsLeaf::EncodeForHash();
	}

	FSessionPermissions Permissions;
};
