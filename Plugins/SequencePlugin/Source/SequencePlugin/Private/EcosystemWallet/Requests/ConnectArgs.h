#pragma once

#include "Types/BinaryData.h"
#include "ConnectArgs.generated.h"

USTRUCT()
struct FConnectArgs
{
	GENERATED_USTRUCT_BODY()

	FAddress SessionAddress;
	FString PreferredLoginMethod;
	TOptional<FString> Email;
	FString Origin;
	bool bIncludeImplicitSession = false;
	TOptional<FSessionPermissions> Permissions;
};
