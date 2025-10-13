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
		UE_LOG(LogTemp, Display, TEXT("Encoding permissions leaf %s"), *Permissions.SessionAddress);
		constexpr uint32 Flag = FSessionsTopology::FlagPermissions << 4;
		return FByteArrayUtils::ConcatBytes({ FByteArrayUtils::ByteArrayFromNumber(Flag, 1), Permissions.Encode() });
	}
	
	virtual TArray<uint8> EncodeForHash() override
	{
		return FByteArrayUtils::ConcatBytes({ FByteArrayUtils::ByteArrayFromNumber(FSessionsTopology::FlagPermissions, 1), Permissions.Encode() });
	}

	FSessionPermissions Permissions;
};
