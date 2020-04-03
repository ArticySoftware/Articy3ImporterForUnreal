//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
 
//
using UnrealBuildTool;
using System.IO;

public class ArticyRuntime : ModuleRules
{
	public ArticyRuntime(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        OptimizeCode = CodeOptimization.Never;

		PublicIncludePaths.AddRange(
			new string[] 
			{
				// ... add other public include paths required here ...
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
				// ... add other public dependencies that you statically link with here ...
                "MediaAssets"
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Projects",
				"InputCore",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				// ... add private dependencies that you statically link with here ...	
				"Json"
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
