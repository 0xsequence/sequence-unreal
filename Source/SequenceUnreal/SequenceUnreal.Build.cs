// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SequenceUnreal : ModuleRules
{
	public SequenceUnreal(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp20;
		
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "SequencePlugin", "EOSSDK", "HttpServer", "OnlineServicesEOS" });
        PrivateDependencyModuleNames.AddRange(new string[] { "HttpServer" });
    }
}
