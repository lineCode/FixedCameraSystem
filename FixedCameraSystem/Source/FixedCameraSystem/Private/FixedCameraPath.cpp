// Copyright 2023 German Lopez. All Rights Reserved.

#include "FixedCameraPath.h"

#pragma region UNREAL ENGINE EVENTS
AFixedCameraPath::AFixedCameraPath()
{
	// Disable Tick for Optimization
	PrimaryActorTick.bCanEverTick = false;

	// Camera Rail Configuration
	CameraPath = CreateDefaultSubobject<USplineComponent>("Camera Path");
	CameraPath->SetSelectedSplineSegmentColor(FColor::Green);
	CameraPath->SetUnselectedSplineSegmentColor(FColor::Green);
	CameraPath->SetTangentColor(FColor::Green);
	CameraPath->bVisualizeComponent = true;
	CameraPath->bShouldVisualizeScale = true;
	CameraPath->ScaleVisualizationWidth = 10;
	CameraPath->bAllowDiscontinuousSpline = false;

	// Setup Root Component
	RootComponent = CameraPath;
}

void AFixedCameraPath::BeginPlay()
{
	Super::BeginPlay();
}

void AFixedCameraPath::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
#pragma endregion

#pragma region CLASS EVENTS
float AFixedCameraPath::GetRailLength()
{
	return CameraPath->GetSplineLength();
}

FVector AFixedCameraPath::GetLocationAlongRail(float TravellingDistance)
{
	return CameraPath->GetLocationAtDistanceAlongSpline(TravellingDistance, ESplineCoordinateSpace::World);
}

FVector AFixedCameraPath::GetInitialLocation()
{
	return CameraPath->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);
}
#pragma endregion