// Copyright Epic Games, Inc. All Rights Reserved.
#pragma warning(disable: 4018)
#pragma warning(disable: 4146)
#pragma warning(disable: 4104)
#pragma warning(disable: 4101)
#include "SequencePlugin.h"
#include "Core.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "secp256k1Library/secp256k1//secp256k1.h"

#define LOCTEXT_NAMESPACE "FSequencePluginModule"

void FSequencePluginModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
		// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Get the base directory of this plugin
	FString BaseDir = IPluginManager::Get().FindPlugin("SequencePlugin")->GetBaseDir();

	// Add on the relative location of the third party dll and load it
	FString LibraryPath;
#if PLATFORM_WINDOWS
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/secp256k1Library/Win64/libsecp256k1.2.0.1.dll"));
#elif PLATFORM_MAC
    LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/secp256k1Library/Mac/Release/libsecp256k1.2.0.2.dylib"));
#elif PLATFORM_LINUX
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/secp256k1Library/Linux/x86_64-unknown-linux-gnu/libsecp256k1.2.0.1.so"));
#endif // PLATFORM_WINDOWS

	ExampleLibraryHandle = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath) : nullptr;

	if (ExampleLibraryHandle)
	{
		unsigned char msg_hash[32] = {
			0x31, 0x5F, 0x5B, 0xDB, 0x76, 0xD0, 0x78, 0xC4,
			0x3B, 0x8A, 0xC0, 0x06, 0x4E, 0x4A, 0x01, 0x64,
			0x61, 0x2B, 0x1F, 0xCE, 0x77, 0xC8, 0x69, 0x34,
			0x5B, 0xFC, 0x94, 0xC7, 0x58, 0x94, 0xED, 0xD3,
		};
		unsigned char seckey[32];
		unsigned char randomize[32];
		unsigned char compressed_pubkey[33];
		unsigned char serialized_signature[64];
		size_t len;
		int is_signature_valid, is_signature_valid2;
		int return_val;
		secp256k1_pubkey pubkey;
		secp256k1_ecdsa_signature sig;
	}
	else
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ThirdPartyLibraryError", "Failed to load example third party library"));
	}
}

void FSequencePluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	// Free the dll handle
	FPlatformProcess::FreeDllHandle(ExampleLibraryHandle);
	ExampleLibraryHandle = nullptr;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSequencePluginModule, SequencePlugin)