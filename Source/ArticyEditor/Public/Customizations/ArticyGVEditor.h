
#pragma once

#include "CoreMinimal.h"
#include "UObject/GCObject.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "Misc/NotifyHook.h"
#include "EditorUndoClient.h"
#include "ArticyGlobalVariables.h"
#include "Slate/GV/SArticyGlobalVariables.h"

class FArticyGVEditor : public FAssetEditorToolkit, FEditorUndoClient, FNotifyHook, FGCObject
{
public:
	virtual ~FArticyGVEditor();
	void InitArticyGVEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UArticyGlobalVariables* ObjectToEdit);

	/** FGCObject Interface */
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	/** IToolkit Interface */
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	/** FAssetEditorToolkit Interface */
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& TabManager) override;
	virtual FString GetReferencerName() const override;

private:
	TSharedRef<SDockTab> SpawnTab_ArticyGV(const FSpawnTabArgs& Args) const;
private:
	TWeakObjectPtr<UArticyGlobalVariables> GlobalVariables;
	TSharedPtr<SArticyGlobalVariables> GlobalVariablesWidget;
	static const FName ArticyGVTab;
};
