// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class SequencePlugin : ModuleRules
{
	public SequencePlugin(ReadOnlyTargetRules Target) : base(Target)
	{
		bUseUnity = false;
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		AddEthAbiLibraries();

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
				"Core", "HTTP", "Json", "ApplicationCore", "Engine", "OpenSSL"
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
				"ApplicationCore", "WebBrowser", "EngineSettings", "HTTPServer", "WebBrowserWidget", "UMG"
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
	
		//Add Mac specific Dependencies
		if (Target.Platform == UnrealTargetPlatform.Mac)
		{
			PublicFrameworks.AddRange(new string[]
			{
				"Foundation",
				"Security",
				"CoreFoundation",
				"SystemConfiguration",
				"AVFoundation",
				"LocalAuthentication",
				"AuthenticationServices",
			});
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

	public void AddEthAbiLibraries()
	{
		string EthAbiDirectory = Path.Combine(PluginDirectory, "Source/SequencePlugin/Public/EthAbi/");
		
		PublicSystemLibraryPaths.Add(EthAbiDirectory);
		PublicAdditionalLibraries.Add(Path.Combine(EthAbiDirectory, "libethabi_bridge.a"));

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            string libDir = Path.Combine(EthAbiDirectory, "windows");
            PublicAdditionalLibraries.Add(Path.Combine(libDir, "libethabi_bridge.lib"));

            // Add extra MSVC libs here
            PublicSystemLibraries.AddRange(new string[]
            {
	            "advapi32.lib",
	            "bcrypt.lib",
	            "kernel32.lib",
	            "userenv.lib",
	            "ws2_32.lib",
	            "ntdll.lib"
            });
        }
        else if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            string libDir = Path.Combine(EthAbiDirectory, "macos");
            PublicAdditionalLibraries.Add(Path.Combine(libDir, "libethabi_bridge.a"));
        }
        else if (Target.Platform == UnrealTargetPlatform.IOS)
        {
            PublicAdditionalLibraries.Add(Path.Combine(EthAbiDirectory, "ios", "libethabi_bridge.a"));
        }
        else if (Target.Platform == UnrealTargetPlatform.Android)
        {
            string libDir = Path.Combine(EthAbiDirectory, "android");
            PublicAdditionalLibraries.Add(Path.Combine(libDir, "arm64", "libethabi_bridge.a"));
            PublicAdditionalLibraries.Add(Path.Combine(libDir, "x86", "libethabi_bridge.a"));
        }
	}
}//namespace