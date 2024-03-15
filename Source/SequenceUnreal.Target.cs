// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SequenceUnrealTarget : TargetRules
{
	public SequenceUnrealTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_2;
		ExtraModuleNames.Add("SequenceUnreal");
	}
}
