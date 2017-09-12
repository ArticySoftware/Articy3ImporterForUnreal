<p align="center">
  <img height="300" src="https://www.nevigo.com/articy-importer/unity/media/ad_Importer_Large.png">
</p>

# ArticyImporter for Unreal Engine 4

This unreal engine 4 plugin allows you to import your articy:draft content into your unreal project and gives you a simple C++ and blueprint interface to work with your data.

# Goals

We release our current progress for the plugin as a github open source project in the hopes that this will give you a substantial headstart into incorporating your articy:draft data into your unreal project while still give you enough flexibility to adjust the plugin to your needs. 
We also would love to hear about suggestions or even better for you to directly contribute to the development of the plugin.

# Setup

There are a couple of steps need to get the plugin to work with your unreal project.

## Create a new project

You need to create a C++ unreal project or make sure that your existing project is capable of compiling c++ projects.


<p align="center">
  <img height="400" src="https://www.nevigo.com/articy-importer/unreal/create_new_cpp_project.png">
</p>

After you have created a new project, close the unreal editor for now.

## Clone/Copy the plugin into the unreal project

Get a copy of the plugin and copy it into your projects **Plugins** folder. It is possible that you don't have one if it is a new project, so you can just create it.
Copy the plugin folder **ArticyImporter** into this **Plugins** folder. Your project structure should now look something like this.

<p align="center">
  <img src="https://www.nevigo.com/articy-importer/unreal/copy_plugin.png">
</p>

## Adjust build configuration

For unreal to correctly build the plugin we need to add it as an dependency, to do that open the file **Source/\<ProjectName>/\<ProjectName\>Cpp.Build.cs**

<p align="center">
  <img src="https://www.nevigo.com/articy-importer/unreal/buildconfigfile.png">
</p>

And open it in your favorite text or code editor.

Now we need to adjust the existing code and make sure that the Importer is an dependency for the project. 
Locate the `PublicDependencyModuleNames` array and add `"ArticyRuntime"` as an additional dependancy. 
You should also add the plugin header files to the public include path by adding `PublicIncludePaths.AddRange(new string[] {"ArticyRuntime/Public"});` below the dependencies.
Your file should now look something like this:

```
// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MyArticyProject : ModuleRules
{
	public MyArticyProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
							{
								"Core",
								"CoreUObject",
								"Engine",
								"InputCore",
								"HeadMountedDisplay",
								"ArticyRuntime"
							});
		
		PublicIncludePaths.AddRange(new string[] {"ArticyRuntime/Public"});
	}
}
```

Make sure to save the file and close the editor.

## Enable plugin in unreal

Now you can open your unreal project and open the Plugins window by selecting Edit->Plugins in the main window menu bar.
Inside this window scroll in the list of plugins down until you find the Group **Project** and the sub group **Articy**, click on Articy and enable the ArticyImporter plugin on the right.

<p align="center">
  <img src="https://www.nevigo.com/articy-importer/unreal/enable_plugin.png">
</p>

After enabling the plugin the window will prompt you to restart the editor which you should to do activate it for your project. When the editor restarts it will prompt you to rebuild missing dll files for the plugin which you should confirm. This can take a bit depending on the size of the project and the power of your computer but once finished the unreal editor should be back up again.

## Export project from articy:draft

Now that the plugin is running, you are ready to export your data from articy:draft. You first need to enable the experimental unreal export to do that you need to start articy:draft with the `-experimental.unreal` command line switch.

In the explorer you can do that by modifying the shortcut for articy:draft.

<p align="center">
  <img src="https://www.nevigo.com/articy-importer/unreal/enable_explorer_export.png">
</p>

or if you are using steam, you can change add that in the launch options of your articy:draft version.

<p align="center">
  <img src="https://www.nevigo.com/articy-importer/unreal/enable_steam_export.png">
</p>

> Please note that the experimental unreal export needs at least articy:draft version **3.1.6.31945**

Now open your articy:draft project and open the export window. Here you will now find the unreal engine export. Please note that the unreal export uses [Rulesets](https://www.nevigo.com/help/Exports_Rulesets.html) to choose what and how to export.
When exporting, chose your unreal projects **Content** folder as the target for the `.articyue4` export file.

After every export, going back to unreal will trigger the ArticyImporter plugin to automatically parse the new file and import the changes.

Make sure to confirm the prompt after the import to save all assets generated by the plugin.

# Contributing

We are very grateful for any kind of contribution that you bring to the ArticyImporter plugin, no matter if it is reporting any issues or by actively adding new features of fixing existing issues. If you want to know more about how to contribute please check our [Contribution](https://github.com/ArticySoftware/ArticyImporterForUnreal/blob/master/CONTRIBUTING.md) article.