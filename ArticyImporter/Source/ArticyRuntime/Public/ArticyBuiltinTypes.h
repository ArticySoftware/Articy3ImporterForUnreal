//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#pragma once

#include "ArticyPins.h"
#include "ArticyBaseTypes.h"
#include "ArticyScriptFragment.h"

#include "ArticyBuiltinTypes.generated.h"

UCLASS(BlueprintType)
class ARTICYRUNTIME_API UArticyPreviewImage : public UArticyBaseObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FArticyRect ViewBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EArticyPreviewImageViewBoxModes Mode;

	/** The ID of the asset. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FArticyId Asset;

private:
	template<typename Type, typename PropType>
		friend struct ArticyObjectTypeInfo;
	void InitFromJson(TSharedPtr<FJsonValue> Json) override;
};

//---------------------------------------------------------------------------//

UCLASS(BlueprintType)
class ARTICYRUNTIME_API UArticyConnection : public UArticyObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Label"))
	FString Label;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Color"))
	FLinearColor Color;

	void InitFromJson(TSharedPtr<FJsonValue> Json) override;
};

UCLASS(BlueprintType)
class ARTICYRUNTIME_API UArticyOutgoingConnection : public UArticyConnection
{
	GENERATED_BODY()
	
public:

	UArticyPrimitive* GetTarget() const;
	/** Can be an InputPin (next node) or an OutputPin (emerge to parent node). */
	UArticyFlowPin* GetTargetPin() const;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="TargetPin"))
	FArticyId TargetPin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Target"))
	FArticyId Target;

private:
	UPROPERTY(VisibleAnywhere, Transient)
	mutable UArticyFlowPin* TargetPinObj;
	UPROPERTY(VisibleAnywhere, Transient)
	mutable UArticyPrimitive* TargetObj;

	template<typename Type, typename PropType>
	friend struct ArticyObjectTypeInfo;
	friend void UArticyFlowPin::InitFromJson(TSharedPtr<FJsonValue> Json);

	void InitFromJson(TSharedPtr<FJsonValue> Json) override;
};

UCLASS(BlueprintType)
class ARTICYRUNTIME_API UArticyIncomingConnection : public UArticyConnection
{
	GENERATED_BODY()
	
public:

	UArticyPrimitive* GetSource() const;
	UArticyOutputPin* GetSourcePin() const;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="SourcePin"))
	FArticyId SourcePin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Source"))
	FArticyId Source;

private:
	UPROPERTY(VisibleAnywhere, Transient)
	mutable UArticyOutputPin* SourcePinObj;
	UPROPERTY(VisibleAnywhere, Transient)
	mutable UArticyPrimitive* SourceObj;

	template<typename Type, typename PropType>
		friend struct ArticyObjectTypeInfo;

	void InitFromJson(TSharedPtr<FJsonValue> Json) override;
};

//---------------------------------------------------------------------------//

USTRUCT(BlueprintType)
struct ARTICYRUNTIME_API FArticyLocationAnchor
{
	GENERATED_BODY()
	
public:
	FArticyLocationAnchor() = default;
	FArticyLocationAnchor(TSharedPtr<FJsonValue> Json);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="AnchorPositionX"))
	float AnchorPositionX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="AnchorPositionY"))
	float AnchorPositionY;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="AnchorColor"))
	FLinearColor AnchorColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="AnchorSize"))
	EArticyLocationAnchorSize AnchorSize;
};

UCLASS(BlueprintType)
class ARTICYRUNTIME_API UArticyTransformation : public UArticyBaseObject
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D Pivot;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Rotation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D Translation;

private:
	template<typename Type, typename PropType>
		friend struct ArticyObjectTypeInfo;
	void InitFromJson(TSharedPtr<FJsonValue> Json) override;
};
