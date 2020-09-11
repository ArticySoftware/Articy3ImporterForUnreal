//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
 
//
using UnrealBuildTool;

public class ArticyEditor : ModuleRules
{
	public ArticyEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		OptimizeCode = CodeOptimization.Never;

		PublicIncludePaths.AddRange(
			new string[] 
			{
				// ... add public include paths required here ...
			}
			);


		PrivateIncludePaths.AddRange(
			new string[] 
			{
				// ... add other private include paths required here ...
			}
			);

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				//"ClassViewer"
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
                "ContentBrowser",
				"PropertyEditor",
				"EditorStyle",
				"EditorWidgets",
				"SourceControl",
				"GraphEditor"
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
