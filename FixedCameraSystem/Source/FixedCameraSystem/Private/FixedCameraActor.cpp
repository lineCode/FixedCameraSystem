// Copyright 2023 German Lopez. All Rights Reserved.

#include "FixedCameraActor.h"

#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "Math/UnrealMathVectorCommon.h"
#include "Misc/MessageDialog.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Math/Rotator.h"
#include "Math/UnrealMathVectorCommon.h"
#include "Math/Quat.h"

#define LOCTEXT_NAMESPACE "FFixedCameraToolsModule"

// Sets default values
AFixedCameraActor::AFixedCameraActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Root = CreateDefaultSubobject<USceneComponent>("Root");

	/*static ConstructorHelpers::FObjectFinder<UTexture2D> RetrievedSprite(TEXT("/FixedCameraTools/Textures/CameraIcon.CameraIcon"));   
	UTexture2D* Icon = RetrievedSprite.Object;
	Root->SetSprite(Icon);*/

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(Root);
	RootComponent = Camera;

	SetActorTickEnabled(false);
}


void AFixedCameraActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (CameraType == ECameraType::Rail && CameraRail)
	{
		SetActorLocation(CameraRail->GetInitialLocation());
		//CameraRail->SetCameraReference(this);
	}
}

// Called when the game starts or when spawned
void AFixedCameraActor::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacterActorReference = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	
	SetActorTickEnabled(false);

	if (bDefaultCamera) 
	{
		Cast<APlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0))->SetViewTarget(this);
		Camera->SetActive(true);
	}
	else 
	{ 
		Camera->SetActive(false); 
	}


	FText DialogText;

	switch (CameraType)
	{
	case ECameraType::Rail:
		if (!CameraRail)
		{
			DialogText = FText::Format(
				LOCTEXT("PluginButtonDialogText", "ON RAIL CAMERA MODE\n------------------------\nPlease, ensure that a {0} reference is set in {1}."),
				FText::FromString(TEXT("Rail")),
				FText::FromString(UKismetSystemLibrary::GetDisplayName(this))
			);
			FMessageDialog::Open(EAppMsgType::Ok, DialogText);
			UKismetSystemLibrary::QuitGame(GetWorld(), UGameplayStatics::GetPlayerController(GetWorld(), 0), EQuitPreference::Quit, false);
			return;
		}
		break;
	default:
		break;
	}

	switch (CameraFocus)
	{
	case ECameraFocus::FocusOnObject:
		if (!FocusTarget)
		{
			DialogText = FText::Format(
				LOCTEXT("PluginButtonDialogText", "FOCUS ON TARGET MODE\n--------------------------\nPlease, ensure that a {0} reference is set in {1}."),
				FText::FromString(TEXT("Target")),
				FText::FromString(UKismetSystemLibrary::GetDisplayName(this))
			);
			FMessageDialog::Open(EAppMsgType::Ok, DialogText);
			UKismetSystemLibrary::QuitGame(GetWorld(), UGameplayStatics::GetPlayerController(GetWorld(), 0), EQuitPreference::Quit, false);
			return;
		}
		break;
	case ECameraFocus::MiddleLocationPlayerAndObject:
		if (!FocusTarget)
		{
			DialogText = FText::Format(
				LOCTEXT("PluginButtonDialogText", "FOCUS ON MIDDLE LOCATION BETWEEN PLAYER AND TARGET MODE\n--------------------------------------------------------------------\nPlease, ensure that a {0} reference is set in {1}."),
				FText::FromString(TEXT("Target")),
				FText::FromString(UKismetSystemLibrary::GetDisplayName(this))
			);
			FMessageDialog::Open(EAppMsgType::Ok, DialogText);
			UKismetSystemLibrary::QuitGame(GetWorld(), UGameplayStatics::GetPlayerController(GetWorld(), 0), EQuitPreference::Quit, false);
			return;
		}
		break;
	default:
		break;
	}

	SetActorTickEnabled(true);
	originalCameraRotation = Camera->GetComponentRotation();
}

// Called every frame
void AFixedCameraActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!PlayerCharacterActorReference) 
	{
		PlayerCharacterActorReference = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
		return;
	}
		

	if (CameraType == ECameraType::Rail) 
	{
		if (bSmoothMovement)
			SetActorLocation(FMath::Lerp(GetActorLocation(), CameraRail->GetLocationAlongRail(CameraRail->GetRailLength() * FMath::Clamp(FVector::Distance(PlayerCharacterActorReference->GetActorLocation(), GetActorLocation()) / fRailTravellingDistance, 0.f, 1.f)), GetWorld()->GetDeltaSeconds() * fSmoothMovementSpeed));
		else
			SetActorLocation(CameraRail->GetLocationAlongRail(CameraRail->GetRailLength() * FMath::Clamp(FVector::Distance(PlayerCharacterActorReference->GetActorLocation(), GetActorLocation()) / fRailTravellingDistance, 0.f, 1.f)));
	}

	if (CameraFocus == ECameraFocus::NoFocus)
	{
		return;
	}

	FRotator targetRotation;

	switch (CameraFocus) 
	{
		case ECameraFocus::FocusOnPlayer:
			targetRotation = UKismetMathLibrary::FindLookAtRotation(Camera->GetComponentLocation(), PlayerCharacterActorReference->GetActorLocation());
			break;
		case ECameraFocus::FocusOnObject:
			targetRotation = UKismetMathLibrary::FindLookAtRotation(Camera->GetComponentLocation(), FocusTarget->GetActorLocation());
			break;
		case ECameraFocus::MiddleLocationPlayerAndInitialFocus:
			targetRotation = FRotator(FQuat::Slerp(FQuat(originalCameraRotation), FQuat(UKismetMathLibrary::FindLookAtRotation(Camera->GetComponentLocation(), PlayerCharacterActorReference->GetActorLocation())), fMiddlePointAlpha));
			break;
		case ECameraFocus::MiddleLocationPlayerAndObject:
			targetRotation = FRotator(FQuat::Slerp(FQuat(UKismetMathLibrary::FindLookAtRotation(Camera->GetComponentLocation(), FocusTarget->GetActorLocation())), FQuat(UKismetMathLibrary::FindLookAtRotation(Camera->GetComponentLocation(), PlayerCharacterActorReference->GetActorLocation())), fMiddlePointAlpha));
			//targetRotation = UKismetMathLibrary::FindLookAtRotation(Camera->GetComponentLocation(), UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorLocation() / 2 + FocusTarget->GetActorLocation() / 2);
			break;
		default:
			break;
	}

	if (bSmoothRotation)
		Camera->SetWorldRotation(FMath::Lerp(Camera->GetComponentRotation(), targetRotation, GetWorld()->GetDeltaSeconds() * fSmoothRotationSpeed));
	else
		Camera->SetWorldRotation(targetRotation);
}

void AFixedCameraActor::ActivateFixedCamera(float fSmoothTransition, TEnumAsByte<EViewTargetBlendFunction> BlendFunction, float fBlendExponent)
{
	SetActorTickEnabled(true);
	Camera->SetActive(true);
	UGameplayStatics::GetPlayerController(this, 0)->SetViewTargetWithBlend(this, fSmoothTransition, BlendFunction, fBlendExponent);
}

void AFixedCameraActor::DeactivateFixedCamera()
{
	if(bAutoDeactivateTickMethod)
		SetActorTickEnabled(false);

	Camera->SetActive(false);
}