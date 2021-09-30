//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
 
//


#include "ArticyBuiltinTypes.h"
#include "ArticyAsset.h"
#include "ArticyHelpers.h"
#include "ArticyDatabase.h"

void UArticyPreviewImage::InitFromJson(TSharedPtr<FJsonValue> Json)
{
	Super::InitFromJson(Json);

	if(!Json.IsValid() || !ensure(Json->Type == EJson::Object))
		return;

	auto obj = Json->AsObject();

	ViewBox = FArticyRect{ obj->TryGetField(STRINGIFY(ViewBox)) };
	JSON_TRY_ENUM(obj, Mode);
	JSON_TRY_HEX_ID(obj, Asset);
}

//---------------------------------------------------------------------------//

void UArticyConnection::InitFromJson(TSharedPtr<FJsonValue> Json)
{
	Super::InitFromJson(Json);

	if(!Json.IsValid() || !ensure(Json->Type == EJson::Object))
		return;
	
	auto obj = Json->AsObject();
	JSON_TRY_STRING(obj, Label);
	Color = ArticyHelpers::ParseColorFromJson(obj->TryGetField("Color"));
}

UArticyPrimitive* UArticyOutgoingConnection::GetTarget() const
{
	if(!TargetObj)
	{
		auto db = UArticyDatabase::Get(this);
		TargetObj = db ? db->GetObject(Target) : nullptr;
	}

	return TargetObj;
}

UArticyFlowPin* UArticyOutgoingConnection::GetTargetPin() const
{
	if(!TargetPinObj)
	{
		auto target = GetTarget();
		if(target)
			TargetPinObj = Cast<UArticyFlowPin>(target->GetSubobject(TargetPin));
	}

	return TargetPinObj;
}

void UArticyOutgoingConnection::InitFromJson(TSharedPtr<FJsonValue> Json)
{
	Super::InitFromJson(Json);

	if(!Json.IsValid() || !ensure(Json->Type == EJson::Object))
		return;
	
	auto obj = Json->AsObject();
	JSON_TRY_HEX_ID(obj, Target);
	JSON_TRY_HEX_ID(obj, TargetPin);
}

UArticyPrimitive* UArticyIncomingConnection::GetSource() const
{
	if(!SourceObj)
	{
		auto db = UArticyDatabase::Get(this);
		SourceObj = db ? db->GetObject(Source) : nullptr;
	}

	return SourceObj;
}

UArticyOutputPin* UArticyIncomingConnection::GetSourcePin() const
{
	if(!SourcePinObj)
	{
		auto db = UArticyDatabase::Get(this);
		SourcePinObj = db ? Cast<UArticyOutputPin>(db->GetObject(SourcePin)) : nullptr;
	}

	return SourcePinObj;
}

void UArticyIncomingConnection::InitFromJson(TSharedPtr<FJsonValue> Json)
{
	Super::InitFromJson(Json);

	if(!Json.IsValid() || !ensure(Json->Type == EJson::Object))
		return;
	
	auto obj = Json->AsObject();
	JSON_TRY_HEX_ID(obj, Source);
	JSON_TRY_HEX_ID(obj, SourcePin);
}

//---------------------------------------------------------------------------//

FArticyLocationAnchor::FArticyLocationAnchor(TSharedPtr<FJsonValue> Json)
{
	if(!Json.IsValid() || !ensure(Json->Type == EJson::Object))
		return;

	auto obj = Json->AsObject();

	JSON_TRY_FLOAT(obj, AnchorPositionX);
	JSON_TRY_FLOAT(obj, AnchorPositionY);
	
	auto colorJson = obj->TryGetField(STRINGIFY(AnchorColor));
	if(colorJson.IsValid())
		AnchorColor = ArticyHelpers::ParseColorFromJson(colorJson);

	JSON_TRY_ENUM(obj, AnchorSize);
}

void UArticyTransformation::InitFromJson(TSharedPtr<FJsonValue> Json)
{
	Super::InitFromJson(Json);

	if(!Json.IsValid() || !ensure(Json->Type == EJson::Object))
		return;

	auto obj = Json->AsObject();
	
	{
		auto pivotJson = obj->TryGetField(STRINGIFY(Pivot));
		if(pivotJson.IsValid())
			Pivot = ArticyHelpers::ParseFVector2DFromJson(pivotJson);
	}

	{
		auto translationJson = obj->TryGetField(STRINGIFY(Translation));
		if(translationJson.IsValid())
			Translation = ArticyHelpers::ParseFVector2DFromJson(translationJson);
	}

	{
		auto matrixJson = obj->TryGetField(STRINGIFY(Matrix));
		if (matrixJson.IsValid())
			Matrix = ArticyHelpers::ParseFMatrixFromJson(matrixJson);
	}

	JSON_TRY_FLOAT(obj, Rotation);
}
