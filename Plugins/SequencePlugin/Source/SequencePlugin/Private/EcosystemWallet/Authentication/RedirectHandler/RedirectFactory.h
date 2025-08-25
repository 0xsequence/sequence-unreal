#pragma once

#include "CoreMinimal.h"
#include "LocalhostRedirectHandler.h"

class IRedirectHandler;

class FRedirectFactory
{
public:
	static TSharedPtr<IRedirectHandler> CreateHandler()
	{
#if PLATFORM_IOS || PLATFORM_ANDROID || PLATFORM_MAC
		return MakeShared<FLocalhostRedirectHandler>();
#elif PLATFORM_HTML5 || PLATFORM_BROWSER
		return MakeShared<FLocalhostRedirectHandler>();
#else
		return MakeShared<FLocalhostRedirectHandler>();
#endif
	}
};