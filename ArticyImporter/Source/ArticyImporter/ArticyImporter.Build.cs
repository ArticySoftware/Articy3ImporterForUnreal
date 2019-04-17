//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
using UnrealBuildTool;
using System.IO;

public class ArticyImporter : ModuleRules
{
	public ArticyImporter(ReadOnlyTargetRules Target) : base(Target)
	{
		//OptimizeCode = CodeOptimization.Never;
		
#if UE_4_21_OR_LATER
		PrivatePCHHeaderFile = "Private/ArticyImporterPrivatePCH.h";
#endif

		PublicIncludePaths.AddRange(
			new string[] {
				Path.Combine(ModuleDirectory, "Public"),
				Path.Combine(ModuleDirectory, "../ArticyImporter/Public"),
				// ... add public include paths required here ...
#if UE_4_20_OR_LATER
				Path.Combine(EngineDirectory, "Source/Editor/GameProjectGeneration"),
#else
				"GameProjectGeneration",
#endif
			}
			);


		PrivateIncludePaths.AddRange(
			new string[] {
				Path.Combine(ModuleDirectory, "Private"),
				// ... add other private include paths required here ...
			}
			);

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Projects",
				"InputCore",
				"UnrealEd",
				"LevelEditor",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				// ... add private dependencies that you statically link with here ...	
                "ArticyRuntime",
				"Json",
                "GameProjectGeneration",
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
