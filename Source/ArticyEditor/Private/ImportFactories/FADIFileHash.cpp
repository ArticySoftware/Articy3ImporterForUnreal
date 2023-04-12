//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#include "FADIFileHash.h"
#include "ArticyHelpers.h"


void FADIFileHash::ImportFromJson(const TSharedPtr<FJsonObject> Json)
{
	if (!Json.IsValid())
		return;

	JSON_TRY_STRING(Json, FileName);
	JSON_TRY_STRING(Json,Hash);
}
