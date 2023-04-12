// Fill out your copyright notice in the Description page of Project Settings.

using System.IO;
using UnrealBuildTool;

public class secp256k1Library : ModuleRules
{
	public secp256k1Library(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			// Add the import library
			PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "x64", "Release", "ExampleLibrary.lib"));

			// Delay-load the DLL, so we can load it from the right place first
			PublicDelayLoadDLLs.Add("ExampleLibrary.dll");

			// Ensure that the DLL is staged along with the executable
			RuntimeDependencies.Add("$(PluginDir)/Binaries/ThirdParty/secp256k1Library/Win64/ExampleLibrary.dll");
        }
        else if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            PublicDelayLoadDLLs.Add(Path.Combine(ModuleDirectory, "Mac", "Release", "libsecp256k1.2.0.2.dylib"));
            RuntimeDependencies.Add("$(PluginDir)/Source/ThirdParty/secp256k1Library/Mac/Release/libsecp256k1.2.0.2.dylib");
        }
        else if (Target.Platform == UnrealTargetPlatform.Linux)
		{
			string ExampleSoPath = Path.Combine("$(PluginDir)", "Binaries", "ThirdParty", "secp256k1Library", "Linux", "x86_64-unknown-linux-gnu", "libExampleLibrary.so");
			PublicAdditionalLibraries.Add(ExampleSoPath);
			PublicDelayLoadDLLs.Add(ExampleSoPath);
			RuntimeDependencies.Add(ExampleSoPath);
		}
	}
}
