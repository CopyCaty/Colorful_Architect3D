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
		// 获取 OtherBlock 的包围盒尺寸（长、宽、高）
		FVector BoxExtent = OtherBlock->Cube->Bounds.BoxExtent;
		FVector Size = BoxExtent * 2;  // 计算实际的长宽高

		// 计算 6 个点的位置
		FVector ForwardVector = OtherRotation.RotateVector(FVector(Size.X, 0, 0));  // 前后
		FVector RightVector = OtherRotation.RotateVector(FVector(0, Size.Y, 0));    // 左右
		FVector UpVector = OtherRotation.RotateVector(FVector(0, 0, Size.Z));       // 上下

		FVector FrontPoint = OtherLocation + ForwardVector;   // 前
		FVector BackPoint = OtherLocation - ForwardVector;    // 后
		FVector RightPoint = OtherLocation + RightVector;     // 右
		FVector LeftPoint = OtherLocation - RightVector;      // 左
		FVector UpPoint = OtherLocation + UpVector;           // 上
		FVector DownPoint = OtherLocation - UpVector;         // 下

		// 存储 6 个点的位置
		TArray<FVector> Points = { FrontPoint, BackPoint, RightPoint, LeftPoint, UpPoint, DownPoint };

		// 查找最近的点
		FVector NearestPoint = Points[0];
		float MinDistance = FVector::Dist(Cube->GetComponentLocation(), Points[0]);

		for (const FVector& Point : Points) {
			float Distance = FVector::Dist(Cube->GetComponentLocation(), Point);
			if (Distance < MinDistance) {
				MinDistance = Distance;
				NearestPoint = Point;
			}
		}

		// 将当前方块移动到最近的点
		SetActorLocation(NearestPoint);
		Cube->SetWorldRotation(OtherRotation);

		
		SetLock(true);
	}
	
}

void ABlockActor::SetLock(bool lock) 
{
	//UE_LOG(LogTemp, Log, TEXT("你的日志消息"));
	IsLocked = lock;
	if (IsLocked)
	{
		// 更新物理状态
		Cube->SetConstraintMode(EDOFMode::SixDOF);
		Cube->SetMobility(EComponentMobility::Stationary);
	}
	else {
		Cube->SetMobility(EComponentMobility::Movable);
		Cube->SetConstraintMode(EDOFMode::Default);
	}
	// // 锁定Cube的线性和角速度
	//Cube->SetAllPhysicsLinearVelocity(FVector::ZeroVector);
	//Cube->SetAllPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	// 锁定位置
	Cube->BodyInstance.bLockXTranslation = IsLocked;
	Cube->BodyInstance.bLockYTranslation = IsLocked;
	Cube->BodyInstance.bLockZTranslation = IsLocked;
	// 锁定旋转
	Cube->BodyInstance.bLockXRotation = IsLocked;
	Cube->BodyInstance.bLockYRotation = IsLocked;
	Cube->BodyInstance.bLockZRotation = IsLocked;
	
	Cube->BodyInstance.UpdatePhysicsFilterData();
}
bool ABlockActor::GetLock() {
	return IsLocked;
}