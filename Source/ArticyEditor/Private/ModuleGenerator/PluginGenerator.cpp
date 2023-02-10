//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.

#include "PluginGenerator.h"
#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "PluginUtils.h"
#include "PluginDescriptor.h"
#include "BuildToolParser/BuildToolParser.h"
#include "Features/IPluginsEditorFeature.h"
#include "Interfaces/IPluginManager.h"

bool PluginGenerator::IsPluginLoaded()
{
	return FModuleManager::Get().IsModuleLoaded(*PluginName);
}

void PluginGenerator::GeneratePlugin()
{
	// .uplugin file data
	FPluginUtils::FNewPluginParamsWithDescriptor CreationParams;
	CreationParams.Descriptor.bCanContainContent = false;
	CreationParams.Descriptor.Modules.Add(FModuleDescriptor(*PluginName));
	CreationParams.Descriptor.FriendlyName = PluginName;
	CreationParams.Descriptor.Version = 1;
	CreationParams.Descriptor.VersionName = TEXT("1.0");
	CreationParams.Descriptor.Category = TEXT("Articy");
	CreationParams.Descriptor.CreatedBy = TEXT("Articy Software GmbH & Co. KG");
	CreationParams.Descriptor.CreatedByURL = TEXT("https://www.articy.com");
	CreationParams.Descriptor.DocsURL = TEXT("https://github.com/ArticySoftware/ArticyImporterForUnreal/blob/master/README.md");
	CreationParams.Descriptor.MarketplaceURL = TEXT("com.epicgames.launcher://ue/marketplace/product/16208606a3f84c9a880c2edffd533ec8");
	CreationParams.Descriptor.SupportURL = TEXT("https://www.articy.com/en/support/");
	CreationParams.Descriptor.Description = TEXT("Project generated code plugin from Articy Editor plugin module");
	CreationParams.Descriptor.bCanContainContent = false;
	CreationParams.Descriptor.bIsBetaVersion = false;
	CreationParams.Descriptor.bIsExperimentalVersion = false;
	CreationParams.Descriptor.bInstalled = false;
	CreationParams.Descriptor.EnabledByDefault = EPluginEnabledByDefault::Enabled;
	CreationParams.Descriptor.Plugins.Add(FPluginReferenceDescriptor(TEXT("ArticyImporter"), true));
	
	// Plugin template : from an Unreal blank plugin template
	FString PluginBaseDir = IPluginManager::Get().FindPlugin(TEXT("PluginBrowser"))->GetBaseDir();
	CreationParams.TemplateFolders.Add(PluginBaseDir / TEXT("Templates") / TEXT("Blank"));

	// New plugin loading parameters
	FText FailReason;
	FPluginUtils::FLoadPluginParams LoadParams;
	LoadParams.bEnablePluginInProject = true;
	LoadParams.bUpdateProjectPluginSearchPath = true;
	LoadParams.bSelectInContentBrowser = true;
	LoadParams.OutFailReason = &FailReason;
		
	// Trigger UE plugin creation
	TSharedPtr<IPlugin> NewPlugin = FPluginUtils::CreateAndLoadNewPlugin(
		PluginName,
		FPaths::ProjectPluginsDir(),
		CreationParams, LoadParams);

	// User feedback (Unreal log)
	if (NewPlugin.IsValid())
	{
		UE_LOG(LogTemp, Display, TEXT("Articy generated code plugin successfully created"));

		// Add ArticyRuntime reference inside generated plugin
		FString PluginBuildCSPath = FPaths::ProjectPluginsDir() / PluginName / TEXT("Source") / PluginName / TEXT("ArticyGenerated.Build.cs");
		
		if(BuildToolParser::AddArticyRuntimeRef(PluginBuildCSPath))
		{
			UE_LOG(LogTemp, Display, TEXT("Articy Runtime reference added successfully."));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Unable to add Articy runtime reference to generated file"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to generate Articy generated code plugin"));
		UE_LOG(LogTemp, Error, TEXT("Reason : %s"), *FailReason.ToString());
	}
}
