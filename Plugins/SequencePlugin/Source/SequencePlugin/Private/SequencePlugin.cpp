// Copyright Epic Games, Inc. All Rights Reserved.
#pragma warning(disable: 4018)
#pragma warning(disable: 4146)
#pragma warning(disable: 4104)
#pragma warning(disable: 4101)

#include "SequencePlugin.h"
#include "Modules/ModuleManager.h"
#include "Engine/Engine.h"

#if PLATFORM_IOS
#include "EcosystemWallet/Authentication/RedirectHandler/MobileRedirectHandler.h"
#include "IOS/IOSAppDelegate.h"
#endif

#define LOCTEXT_NAMESPACE "FSequencePluginModule"

void FSequencePluginModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
		// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
#if PLATFORM_IOS
	FIOSCoreDelegates::OnOpenURL.AddLambda([](UIApplication* App, NSURL* Url, NSString* SourceApp, id Annotation)
	{
		UE_LOG(LogTemp, Display, TEXT("Received deep link: %s"), *Url.absoluteString);
		FMobileRedirectHandler::HandleResponse(Url.absoluteString);
	});
#endif
}

void FSequencePluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSequencePluginModule, SequencePlugin)