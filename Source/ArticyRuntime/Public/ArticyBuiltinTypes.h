//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
 
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Articy")
	FArticyRect ViewBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Articy")
	EArticyPreviewImageViewBoxModes Mode;

	/** The ID of the asset. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Articy")
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Articy", meta=(DisplayName="Label"))
	FString Label;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Articy", meta=(DisplayName="Color"))
	FLinearColor Color;

	void InitFromJson(TSharedPtr<FJsonValue> Json) override;
};

UCLASS(BlueprintType)
class ARTICYRUNTIME_API UArticyOutgoingConnection : public UArticyConnection
{
	GENERATED_BODY()
	
public:

	UArticyPrimitive* GetTarget() const;
	FArticyId GetTargetID() const { return Target;  }
	/** Can be an InputPin (next node) or an OutputPin (emerge to parent node). */
	UArticyFlowPin* GetTargetPin() const;
	FArticyId GetTargetPinID() const { return TargetPin;  }
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Articy", meta=(DisplayName="TargetPin"))
	FArticyId TargetPin;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Articy", meta=(DisplayName="Target"))
	FArticyId Target;

private:
	UPROPERTY(VisibleAnywhere, Transient, Category = "Articy")
	mutable UArticyFlowPin* TargetPinObj;
	UPROPERTY(VisibleAnywhere, Transient, Category = "Articy")
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Articy", meta=(DisplayName="SourcePin"))
	FArticyId SourcePin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Articy", meta=(DisplayName="Source"))
	FArticyId Source;

private:
	UPROPERTY(VisibleAnywhere, Transient, Category = "Articy")
	mutable UArticyOutputPin* SourcePinObj;
	UPROPERTY(VisibleAnywhere, Transient, Category = "Articy")
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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Articy", meta=(DisplayName="AnchorPositionX"))
	float AnchorPositionX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Articy", meta=(DisplayName="AnchorPositionY"))
	float AnchorPositionY;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Articy", meta=(DisplayName="AnchorColor"))
	FLinearColor AnchorColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Articy", meta=(DisplayName="AnchorSize"))
	EArticyLocationAnchorSize AnchorSize;
};

UCLASS(BlueprintType)
class ARTICYRUNTIME_API UArticyTransformation : public UArticyBaseObject
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Articy")
	FVector2D Pivot;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Articy")
	float Rotation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Articy")
	FVector2D Translation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Articy")
	FMatrix Matrix;

private:
	template<typename Type, typename PropType>
		friend struct ArticyObjectTypeInfo;
	void InitFromJson(TSharedPtr<FJsonValue> Json) override;
};

UCLASS()
class ARTICYRUNTIME_API UArticyUserFolder : public UArticyObject
{
	GENERATED_BODY()

};

UCLASS()
class ARTICYRUNTIME_API UArticyZone : public UArticyObject
{
	GENERATED_BODY()

};

UCLASS()
class ARTICYRUNTIME_API UArticyLocation : public UArticyObject
{
	GENERATED_BODY()

};

UCLASS()
class ARTICYRUNTIME_API UArticyDocument : public UArticyObject
{
	GENERATED_BODY()

};

UCLASS()
class ARTICYRUNTIME_API UArticyPath : public UArticyObject
{
	GENERATED_BODY()

};

UCLASS()
class ARTICYRUNTIME_API UArticySpot : public UArticyObject
{
	GENERATED_BODY()

};

UCLASS()
class ARTICYRUNTIME_API UArticyLocationImage : public UArticyObject
{
	GENERATED_BODY()

};

UCLASS()
class ARTICYRUNTIME_API UArticyLocationText : public UArticyObject
{
	GENERATED_BODY()

};


UCLASS()
class ARTICYRUNTIME_API UArticyLink : public UArticyObject
{
	GENERATED_BODY()

};

UCLASS()
class ARTICYRUNTIME_API UArticyComment : public UArticyObject
{
	GENERATED_BODY()

};

UCLASS()
class ARTICYRUNTIME_API UArticyTextObject : public UArticyObject
{
	GENERATED_BODY()

};