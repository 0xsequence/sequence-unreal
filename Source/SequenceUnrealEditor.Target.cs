// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SequenceUnrealEditorTarget : TargetRules
{
	public SequenceUnrealEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("SequenceUnreal");
	}
}
