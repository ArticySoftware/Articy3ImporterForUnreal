
#include "Customizations/AssetActions/AssetTypeActions_ArticyGV.h"

#include "Customizations/ArticyGVEditor.h"

void FAssetTypeActions_ArticyGV::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		const auto ArticyGV = Cast<UArticyGlobalVariables>(*ObjIt);
		if (ArticyGV != nullptr)
		{
			TSharedRef<FArticyGVEditor> Editor(new FArticyGVEditor);
			Editor->InitArticyGVEditor(Mode, EditWithinLevelEditor, ArticyGV);
		}
	}
}
