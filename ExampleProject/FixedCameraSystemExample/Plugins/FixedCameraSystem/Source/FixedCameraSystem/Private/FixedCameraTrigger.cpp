// Fill out your copyright notice in the Description page of Project Settings.

//#include "SetViewTargetBlend.h"
#include "UObject/ConstructorHelpers.h"
#include "FixedCameraTrigger.h"

// Sets default values
AFixedCameraTrigger::AFixedCameraTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>("Root Component");

	RootComponent = Root;

	Trigger1 = CreateDefaultSubobject<UBoxComponent>("Trigger 1");
	Trigger1->SetupAttachment(RootComponent);
	Trigger2 = CreateDefaultSubobject<UBoxComponent>("Trigger 2");
	Trigger2->SetupAttachment(RootComponent);

	DebugCollider1 = CreateDefaultSubobject<UStaticMeshComponent>("Debug Collider 1");
	DebugCollider1->SetupAttachment(RootComponent);

	DebugCollider2 = CreateDefaultSubobject<UStaticMeshComponent>("Debug Collider 2");
	DebugCollider2->SetupAttachment(RootComponent);

	#if WITH_EDITOR
	if (GEngine)
	{
		static ConstructorHelpers::FObjectFinder<UStaticMesh> DebugMeshRef(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
		if (DebugMeshRef.Object != nullptr && GEngine)
			DebugCollider1->SetStaticMesh(DebugMeshRef.Object);

		static ConstructorHelpers::FObjectFinder<UMaterial> DebugMeshMaterialRef(TEXT("Material'/FixedCameraSystem/Materials/M_FixedCameraTrigger_Blue.M_FixedCameraTrigger_Blue'"));
		if (DebugMeshMaterialRef.Object != nullptr)
		{
			UMaterial* M = (UMaterial*)DebugMeshMaterialRef.Object;
			DebugCollider1->SetMaterial(0, M);
		}

		if (DebugMeshRef.Object != nullptr && GEngine)
			DebugCollider2->SetStaticMesh(DebugMeshRef.Object);

		static ConstructorHelpers::FObjectFinder<UMaterial> DebugMeshMaterialRef2(TEXT("Material'/FixedCameraSystem/Materials/M_FixedCameraTrigger_Orange.M_FixedCameraTrigger_Orange'"));
		if (DebugMeshMaterialRef2.Object != nullptr)
		{
			UMaterial* M = (UMaterial*)DebugMeshMaterialRef2.Object;
			DebugCollider2->SetMaterial(0, M);
		}
	}
	#endif

	SetActorScale3D(FVector(1.f, 5, 5));

	Trigger1->OnComponentBeginOverlap.AddDynamic(this, &AFixedCameraTrigger::OnBoxBeginOverlap1);
	Trigger1->OnComponentBeginOverlap.AddDynamic(this, &AFixedCameraTrigger::OnBoxBeginOverlap2);

	Trigger1->OnComponentEndOverlap.AddDynamic(this, &AFixedCameraTrigger::OnComponentEndOverlap1);
	Trigger2->OnComponentEndOverlap.AddDynamic(this, &AFixedCameraTrigger::OnComponentEndOverlap2);
	
}
	

// Called when the game starts or when spawned
void AFixedCameraTrigger::BeginPlay()
{
	Super::BeginPlay();

	DebugCollider1->SetVisibility(false);
	DebugCollider2->SetVisibility(false);
}

void AFixedCameraTrigger::OnConstruction(const FTransform& Transform)
{
	SetActorScale3D(FVector(.1f, GetActorScale3D().Y, GetActorScale3D().Z));

	Trigger1->SetRelativeLocation(FVector(-Trigger1->GetCollisionShape().GetExtent().X/ GetActorScale3D().X, Trigger1->GetCollisionShape().GetExtent().Y / GetActorScale3D().Y, Trigger1->GetCollisionShape().GetExtent().Z / GetActorScale3D().Z));
	Trigger2->SetRelativeLocation(FVector(Trigger2->GetCollisionShape().GetExtent().X/ GetActorScale3D().X, Trigger1->GetCollisionShape().GetExtent().Y / GetActorScale3D().Y, Trigger2->GetCollisionShape().GetExtent().Z / GetActorScale3D().Z));
	#if WITH_EDITOR
	if (GEngine)
	{
		DebugCollider1->SetWorldLocation(Trigger1->GetComponentLocation());
		DebugCollider1->SetWorldRotation(Trigger1->GetComponentRotation());
		DebugCollider1->SetWorldScale3D((Trigger1->GetCollisionShape().GetExtent() / 40.f) / 1.25f);

		DebugCollider2->SetWorldLocation(Trigger2->GetComponentLocation());
		DebugCollider2->SetWorldRotation(Trigger2->GetComponentRotation());
		DebugCollider2->SetWorldScale3D((Trigger2->GetCollisionShape().GetExtent() / 40.f) / 1.25f);
	}
	#endif

	DebugCollider1->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DebugCollider1->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	DebugCollider2->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DebugCollider2->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
}

// Called every frame
void AFixedCameraTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFixedCameraTrigger::OnBoxBeginOverlap1(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}

void AFixedCameraTrigger::OnBoxBeginOverlap2(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void AFixedCameraTrigger::OnComponentEndOverlap1(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!Camera1)
		return;

	if (Trigger2->IsOverlappingComponent(OtherComp))
	{
		return;
	}

	Camera2->DeactivateFixedCamera();
	Camera1->ActivateFixedCamera(fSmoothTransition1, BlendFunc1, fBlendExp1);
}

void AFixedCameraTrigger::OnComponentEndOverlap2(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!Camera2)
		return;

	if (Trigger1->IsOverlappingComponent(OtherComp))
	{
		return;
	}

	Camera1->DeactivateFixedCamera();
	Camera2->ActivateFixedCamera(fSmoothTransition2, BlendFunc2, fBlendExp2);
}
