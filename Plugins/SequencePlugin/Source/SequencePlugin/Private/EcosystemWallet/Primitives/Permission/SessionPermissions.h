#pragma once
#include "Permission.h"

class SEQUENCEPLUGIN_API FSessionPermissions
{
public:
	int64 ChainId;
	FString Signer;
	int64 ValueLimit;
	int64 Deadline;
	TArray<Permission*> Permissions;
};
