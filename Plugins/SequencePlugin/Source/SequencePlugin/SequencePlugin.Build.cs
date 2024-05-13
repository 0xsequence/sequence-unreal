// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class SequencePlugin : ModuleRules
{
	public SequencePlugin(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);


        if (Target.Platform == UnrealTargetPlatform.Android)
        {
            PrivateDependencyModuleNames.AddRange(
                new string[] 
                {
					"Engine",
					"Launch"                    
                }
            );
            AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(ModuleDirectory, "SequencePlugin_UPL_Android.xml"));
        }
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core", "HTTP", "Json", "ApplicationCore"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"Projects",
                "Json",
                "JsonUtilities",
				"ApplicationCore", "WebBrowser",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			PublicSystemLibraries.AddRange(
				new string[]
				{
					"Crypt32.lib"
				}
				);
		}
		
		//Add IOS Specific Dependencies
		if (Target.Platform == UnrealTargetPlatform.IOS)
		{
			PrivateDependencyModuleNames.AddRange(new string[] {"Launch"});
			
			/* A list of public frameworks from Apple that you need to use when building.
			* You can view the entire list here: https://developer.apple.com/documentation/technologies
			*/
			PublicFrameworks.AddRange(
				new string[]
				{
					"EventKit",
					"GLKit",
					"CoreTelephony",
					"SystemConfiguration",
					"UIKit",
					"Foundation",
					"CoreGraphics",
					"MobileCoreServices",
					"StoreKit",
					"CFNetwork",
					"CoreData",
					"Security",
					"CoreLocation",
					"WatchConnectivity",
					"MediaPlayer",
					"CoreFoundation",
					"AdSupport",
					"MessageUI",
					"AppTrackingTransparency",
					"WebKit",
					"AVFoundation",
					"Accelerate",
					"LocalAuthentication",
					"SafariServices",
					"AuthenticationServices",
					"UserNotifications",
				}
			);//Public Frameworks
		}//IOS Frameworks
	}//SequencePlugin
}//namespace