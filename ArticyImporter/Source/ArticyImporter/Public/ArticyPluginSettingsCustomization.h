// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "IDetailCustomization.h"

class FArticyPluginSettingsCustomization : public IDetailCustomization
{
public:
	FArticyPluginSettingsCustomization();

	static TSharedRef<IDetailCustomization> MakeInstance();

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;
};
