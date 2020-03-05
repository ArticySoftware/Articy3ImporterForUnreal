//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
using UnrealBuildTool;
using System.IO;

public class ArticyGenerated : ModuleRules
{
	public ArticyGenerated(ReadOnlyTargetRules Target) : base(Target)
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
				"CoreUObject", 
				"Engine"
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"ArticyRuntime"
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
