// Copyright 2023 German Lopez. All Rights Reserved.

#include "FixedCameraSystem.h"
#include "PlacementMode/Public/IPlacementModeModule.h"
#include "ActorFactories/ActorFactoryBlueprint.h"
#include "Interfaces/IPluginManager.h"
#include "Runtime/Launch/Resources/Version.h"
#include "Styling/SlateStyleRegistry.h"
#include "Styling/SlateTypes.h"

#define LOCTEXT_NAMESPACE "FFixedCameraSystemModule"

void FFixedCameraSystemModule::StartupModule()
{
	int Priority = 41;
	FPlacementCategoryInfo FixedCameraSystem( LOCTEXT("FixedCamera", "Fixed Camera"), "FixedCamera", TEXT("FixedCamera"), Priority);
	IPlacementModeModule::Get().RegisterPlacementCategory(FixedCameraSystem);

	// Find and register actors to category
	UBlueprint* FixedCamera = Cast<UBlueprint>(FSoftObjectPath(TEXT("/FixedCameraSystem/Blueprints/FixedCamera.FixedCamera")).TryLoad());
	if (FixedCamera) {
		IPlacementModeModule::Get().RegisterPlaceableItem(FixedCameraSystem.UniqueHandle, MakeShareable(new FPlaceableItem(
			*UActorFactory::StaticClass(),
			FAssetData(FixedCamera, true),
			FName("FixedCamera.Thumbnail"),
			TOptional<FLinearColor>(),
			TOptional<int32>(),
			NSLOCTEXT("PlacementMode", "Fixed Camera", "Fixed Camera")
		)));
	}

	UBlueprint* FixedCameraPath = Cast<UBlueprint>(FSoftObjectPath(TEXT("/FixedCameraSystem/Blueprints/FixedCameraRail.FixedCameraRail")).TryLoad());
	if (FixedCameraPath) {
		IPlacementModeModule::Get().RegisterPlaceableItem(FixedCameraSystem.UniqueHandle, MakeShareable(new FPlaceableItem(
			*UActorFactory::StaticClass(),
			FAssetData(FixedCameraPath, true),
			FName("Spline.Thumbnail"),
			TOptional<FLinearColor>(),
			TOptional<int32>(),
			NSLOCTEXT("PlacementMode", "Fixed Camera | Path", "Fixed Camera | Path")
		)));
	}

	UBlueprint* FixedCameraTrigger = Cast<UBlueprint>(FSoftObjectPath(TEXT("/FixedCameraSystem/Blueprints/FixedCameraTrigger.FixedCameraTrigger")).TryLoad());
	if (FixedCameraTrigger) {
		IPlacementModeModule::Get().RegisterPlaceableItem(FixedCameraSystem.UniqueHandle, MakeShareable(new FPlaceableItem(
			*UActorFactory::StaticClass(),
			FAssetData(FixedCameraTrigger, true),
			FName("Trigger.Thumbnail"),
			TOptional<FLinearColor>(),
			TOptional<int32>(),
			NSLOCTEXT("PlacementMode", "Fixed Camera | Trigger", "Fixed Camera | Trigger")
		)));
	}

	StyleSet = MakeShareable(new FSlateStyleSet("FixedCameraSystemStyle"));

	FString CameraIconPath = IPluginManager::Get().FindPlugin(TEXT("FixedCameraSystem"))->GetBaseDir() + TEXT("/Resources/");
	
	StyleSet->Set("FixedCamera.Thumbnail", new FSlateImageBrush(CameraIconPath + TEXT("FixedCamera_Thumbnail.png"), FVector2D(64.f, 64.f)));
	StyleSet->Set("Spline.Thumbnail", new FSlateImageBrush(CameraIconPath + TEXT("Spline_Thumbnail.png"), FVector2D(64.f, 64.f)));
	StyleSet->Set("Trigger.Thumbnail", new FSlateImageBrush(CameraIconPath + TEXT("Trigger_Thumbnail.png"), FVector2D(64.f, 64.f)));
	StyleSet->Set("Target.Thumbnail", new FSlateImageBrush(CameraIconPath + TEXT("Target_Thumbnail.png"), FVector2D(64.f, 64.f)));
		
	FSlateStyleRegistry::RegisterSlateStyle(*StyleSet.Get());
}

void FFixedCameraSystemModule::ShutdownModule()
{
	if (IPlacementModeModule::IsAvailable())
	{
		IPlacementModeModule::Get().UnregisterPlacementCategory("FixedCameraSystem");
	}

	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet.Get());
	StyleSet.Reset();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FFixedCameraSystemModule, FixedCameraSystem)