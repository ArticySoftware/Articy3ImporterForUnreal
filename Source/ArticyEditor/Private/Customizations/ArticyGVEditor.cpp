
#include "Customizations/ArticyGVEditor.h"
#include "ArticyEditorStyle.h"
#include "Editor.h"
#include "Widgets/Docking/SDockTab.h"

#define LOCTEXT_NAMESPACE "ArticyGVEditor"

const FName FArticyGVEditor::ArticyGVTab(TEXT("ArticyGVTab"));

FArticyGVEditor::~FArticyGVEditor()
{
	GEditor->UnregisterForUndo(this);
}

void FArticyGVEditor::InitArticyGVEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UArticyGlobalVariables* ObjectToEdit)
{
	check(ObjectToEdit)

	GlobalVariables = ObjectToEdit;
	
	GlobalVariables->SetFlags(RF_Transactional);
	for(UArticyBaseVariableSet* VarSet : GlobalVariables->GetVariableSets())
	{
		VarSet->SetFlags(RF_Transactional);
		for(UArticyVariable* Var : VarSet->GetVariables())
		{
			Var->SetFlags(RF_Transactional);
		}
	}
	
	GEditor->RegisterForUndo(this);

	const bool bCreateDefaultStandaloneMenu = true;
	const bool bCreateDefaultToolbar = true;

	const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("Standalone_ArticyGV_Layout")
		->AddArea
		(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
			/* Is this really needed?
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.1f)
				->AddTab(GetToolbarTabId(), ETabState::OpenedTab)->SetHideTabWell(true)
			)*/
			->Split
			(
				FTabManager::NewStack()
				//->SetSizeCoefficient(0.225f)
				->AddTab(ArticyGVTab, ETabState::OpenedTab)->SetHideTabWell(false)
			)
		);

	GlobalVariablesWidget = SNew(SArticyGlobalVariables, GlobalVariables).bInitiallyCollapsed(false);
	
	// creates tabmanager and initializes many things
	FAssetEditorToolkit::InitAssetEditor(Mode, InitToolkitHost, TEXT("ArticyExtensionEditor"), StandaloneDefaultLayout, bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, GlobalVariables.Get(), false);
}

FLinearColor FArticyGVEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0.3f, 0.2f, 0.5f, 0.5f);
}

FName FArticyGVEditor::GetToolkitFName() const
{
	return FName("ArticyGVEditor");
}

FText FArticyGVEditor::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "ArticyGVEditor");
}

FString FArticyGVEditor::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricTabPrefix", "ArticyGV ").ToString();
}

void FArticyGVEditor::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_ArticyGVEditor", "Articy Global Variables Editor"));
	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	// Registers toolbar
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner(ArticyGVTab, FOnSpawnTab::CreateSP(this, &FArticyGVEditor::SpawnTab_ArticyGV))
		.SetDisplayName(LOCTEXT("ArticyGVTab", "Details"))
		.SetIcon(FSlateIcon(FArticyEditorStyle::GetStyleSetName(), "ArticyImporter.ArticyImporter.16", "ArticyImporter.ArticyImporter.8"))
		.SetGroup(WorkspaceMenuCategoryRef);
}

TSharedRef<SDockTab> FArticyGVEditor::SpawnTab_ArticyGV(const FSpawnTabArgs& Args) const
{
	check(Args.GetTabId() == ArticyGVTab);
	
	TSharedRef<SDockTab> DockTab = SNew(SDockTab).Label(LOCTEXT("ArticyGVEditorTitle", "Articy Global Variables"));
	if(GlobalVariablesWidget.IsValid())
	{
		DockTab->SetContent(GlobalVariablesWidget.ToSharedRef());
	}

	return DockTab;
}

#undef LOCTEXT_NAMESPACE
