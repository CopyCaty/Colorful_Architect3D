// Fill out your copyright notice in the Description page of Project Settings.


#include "BlockActor.h"

// Sets default values
ABlockActor::ABlockActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	IsLocked = false;
	Cube = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cube"));
	Cube->OnComponentHit.AddDynamic(this, &ABlockActor::OnHit);
}

// Called when the game starts or when spawned
void ABlockActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABlockActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!IsLocked && Cube->GetPhysicsLinearVelocity().Size() <= 0) {
		//UE_LOG(LogTemp, Log, TEXT("LOCK"));
		SetLock(true);
	}
}

void ABlockActor::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
	if (IsLocked) return;
	if (ABlockActor* OtherBlock = Cast<ABlockActor>(OtherActor)) {
		if (!OtherBlock->GetLock()) return;
		UE_LOG(LogTemp, Warning, TEXT("Collision detected with another block."));
		Cube->SetSimulatePhysics(false);

		FVector OtherLocation = OtherBlock->Cube->GetComponentLocation();
		FRotator OtherRotation = OtherBlock->Cube->GetComponentRotation();
		// ��ȡ OtherBlock �İ�Χ�гߴ磨�������ߣ�
		FVector BoxExtent = OtherBlock->Cube->Bounds.BoxExtent;
		FVector Size = BoxExtent * 2;  // ����ʵ�ʵĳ����

		// ���� 6 �����λ��
		FVector ForwardVector = OtherRotation.RotateVector(FVector(Size.X, 0, 0));  // ǰ��
		FVector RightVector = OtherRotation.RotateVector(FVector(0, Size.Y, 0));    // ����
		FVector UpVector = OtherRotation.RotateVector(FVector(0, 0, Size.Z));       // ����

		FVector FrontPoint = OtherLocation + ForwardVector;   // ǰ
		FVector BackPoint = OtherLocation - ForwardVector;    // ��
		FVector RightPoint = OtherLocation + RightVector;     // ��
		FVector LeftPoint = OtherLocation - RightVector;      // ��
		FVector UpPoint = OtherLocation + UpVector;           // ��
		FVector DownPoint = OtherLocation - UpVector;         // ��

		// �洢 6 �����λ��
		TArray<FVector> Points = { FrontPoint, BackPoint, RightPoint, LeftPoint, UpPoint, DownPoint };

		// ��������ĵ�
		FVector NearestPoint = Points[0];
		float MinDistance = FVector::Dist(Cube->GetComponentLocation(), Points[0]);

		for (const FVector& Point : Points) {
			float Distance = FVector::Dist(Cube->GetComponentLocation(), Point);
			if (Distance < MinDistance) {
				MinDistance = Distance;
				NearestPoint = Point;
			}
		}

		// ����ǰ�����ƶ�������ĵ�
		SetActorLocation(NearestPoint);
		Cube->SetWorldRotation(OtherRotation);

		
		SetLock(true);
	}
	
}

void ABlockActor::SetLock(bool lock) 
{
	//UE_LOG(LogTemp, Log, TEXT("�����־��Ϣ"));
	IsLocked = lock;
	if (IsLocked)
	{
		// ��������״̬
		Cube->SetConstraintMode(EDOFMode::SixDOF);
		Cube->SetMobility(EComponentMobility::Stationary);
	}
	else {
		Cube->SetMobility(EComponentMobility::Movable);
		Cube->SetConstraintMode(EDOFMode::Default);
	}
	// // ����Cube�����Ժͽ��ٶ�
	//Cube->SetAllPhysicsLinearVelocity(FVector::ZeroVector);
	//Cube->SetAllPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	// ����λ��
	Cube->BodyInstance.bLockXTranslation = IsLocked;
	Cube->BodyInstance.bLockYTranslation = IsLocked;
	Cube->BodyInstance.bLockZTranslation = IsLocked;
	// ������ת
	Cube->BodyInstance.bLockXRotation = IsLocked;
	Cube->BodyInstance.bLockYRotation = IsLocked;
	Cube->BodyInstance.bLockZRotation = IsLocked;
	
	Cube->BodyInstance.UpdatePhysicsFilterData();
}
bool ABlockActor::GetLock() {
	return IsLocked;
}