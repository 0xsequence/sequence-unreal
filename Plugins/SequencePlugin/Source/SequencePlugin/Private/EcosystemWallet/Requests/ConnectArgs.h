#pragma once

struct FSessionPermissions
{
	
};

struct FConnectArgs
{
	FString SessionAddress;
	FString PreferredLoginMethod;
	TOptional<FString> Email;
	FString Origin;
	bool bIncludeImplicitSession = false;
	TOptional<FSessionPermissions> Permissions;
};
