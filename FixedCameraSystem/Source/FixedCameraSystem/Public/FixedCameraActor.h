// Copyright 2023 German Lopez. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "Components/BillboardComponent.h"
#include "FixedCameraPath.h"
#include "Camera/PlayerCameraManager.h"
#include "FixedCameraActor.generated.h"

UENUM()
enum class ECameraFocus
{
	NoFocus        UMETA(DisplayName = "No Focus"),
	FocusOnPlayer  UMETA(DisplayName = "Focus on Player"),
	FocusOnObject  UMETA(DisplayName = "Focus on Target"),
	MiddleLocationPlayerAndInitialFocus  UMETA(DisplayName = "Middle location (Player and Initial Focus)"),
	MiddleLocationPlayerAndObject		 UMETA(DisplayName = "Middle location (Player and Target)"),
};

UENUM()
enum class ECameraType
{
	Static        UMETA(DisplayName = "Static Camera"),
	Rail          UMETA(DisplayName = "On Rail Camera")
};


UCLASS()
class FIXEDCAMERASYSTEM_API AFixedCameraActor : public AActor
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Fixed Camera Settings", DisplayName = "Activate on Play"))
	bool bDefaultCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Fixed Camera Settings", InlineCategoryProperty))
	ECameraType CameraType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Fixed Camera Settings", EditCondition = "CameraType == ECameraType::Rail", EditConditionHides))
	class AFixedCameraPath* CameraRail;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Fixed Camera Settings", DisplayName = "Rail Travelling Distance", EditCondition = "CameraType == ECameraType::Rail", EditConditionHides))
	float fRailTravellingDistance = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Fixed Camera Settings", DisplayName = "Smooth Movement", EditCondition = "CameraType == ECameraType::Rail", EditConditionHides))
	bool bSmoothMovement = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Fixed Camera Settings", DisplayName = "Smooth Movement Speed", EditCondition = "bSmoothMovement && CameraType == ECameraType::Rail", EditConditionHides, ClampMin = "0.0"))
	float fSmoothMovementSpeed = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Fixed Camera Settings|Focus Parameters"))
	ECameraFocus CameraFocus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Fixed Camera Settings|Focus Parameters", EditCondition = "CameraFocus == ECameraFocus::FocusOnObject || CameraFocus == ECameraFocus::MiddleLocationPlayerAndObject", EditConditionHides))
	class AActor* FocusTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Fixed Camera Settings|Focus Parameters", EditCondition = "CameraFocus == ECameraFocus::MiddleLocationPlayerAndObject || CameraFocus == ECameraFocus::MiddleLocationPlayerAndInitialFocus", EditConditionHides, DisplayName="Middle Point Alpha", ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float fMiddlePointAlpha = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Fixed Camera Settings|Focus Parameters", DisplayName = "Smooth Rotation", EditCondition = "CameraFocus != ECameraFocus::NoFocus", EditConditionHides))
	bool bSmoothRotation = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Fixed Camera Settings|Focus Parameters", DisplayName = "Smooth Rotation Speed", EditCondition = "bSmoothRotation && CameraFocus != ECameraFocus::NoFocus", EditConditionHides, ClampMin = "0.0"))
	float fSmoothRotationSpeed = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Fixed Camera Settings|Optimization", DisplayName = "Auto-Deactivate Tick Method", Tooltip="Disables tick calls after deacticating the camera."))
	bool bAutoDeactivateTickMethod;

	

	/*UPROPERTY(VisibleAnywhere, meta = (Category = "Fixed Camera Settings"))
	bool bCanShowObjectMessage = true;*/
private:
	FRotator originalCameraRotation;
public:	
	// Sets default values for this actor's properties
	AFixedCameraActor();
	//UPROPERTY(VisibleAnywhere)
	UPROPERTY(VisibleDefaultsOnly)
	UBillboardComponent* Root;
	UPROPERTY(VisibleDefaultsOnly)
	UCameraComponent* Camera;

	AActor* PlayerCharacterActorReference;

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ActivateFixedCamera(float fSmoothTransition, TEnumAsByte<EViewTargetBlendFunction> BlendFunction, float fBlendExponent);
	
	void DeactivateFixedCamera();
};
