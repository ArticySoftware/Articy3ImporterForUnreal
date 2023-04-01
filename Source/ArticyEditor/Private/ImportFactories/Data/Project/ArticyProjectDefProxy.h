#pragma once
#include "ImportFactories/Data/Project/ArticyProjectDef.h"

/**
 * @brief Provide methods for FArticyProjectDef manipulations
 */
class FArticyProjectDefProxy
{
	FArticyProjectDef _project;
	
public:
	FArticyProjectDef fromJson(const TSharedPtr<FJsonObject> JsonProjectDef);
};
