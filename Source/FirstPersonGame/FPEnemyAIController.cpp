#include "FPEnemyAIController.h"
#include "FPEnemy.h"
#include "FPCharacter.h"
#include "Engine/World.h"
#include "EngineUtils.h"

AFPEnemyAIController::AFPEnemyAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AFPEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	ControlledEnemy = Cast<AFPEnemy>(InPawn);
}

void AFPEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!ControlledEnemy || ControlledEnemy->IsDead()) return;

	// 找最近的玩家
	TargetPlayer = nullptr;
	float MinDist = FLT_MAX;
	for (TActorIterator<AFPCharacter> It(GetWorld()); It; ++It)
	{
		float D = FVector::Dist(ControlledEnemy->GetActorLocation(), It->GetActorLocation());
		if (D < MinDist) { MinDist = D; TargetPlayer = *It; }
	}

	if (!TargetPlayer) return;

	AttackIfInRange();
	ChasePlayer();
}

void AFPEnemyAIController::ChasePlayer()
{
	if (!TargetPlayer || !ControlledEnemy) return;

	FVector ToTarget = TargetPlayer->GetActorLocation() - ControlledEnemy->GetActorLocation();

	// 在攻击范围内就不动了
	if (ToTarget.Size() <= AcceptanceRadius) return;

	// 直接朝玩家移动（比 AI MoveTo 更轻量）
	ControlledEnemy->AddMovementInput(ToTarget.GetSafeNormal(), 1.0f);

	// 面向玩家
	FRotator LookAt = ToTarget.Rotation();
	LookAt.Pitch = 0;
	ControlledEnemy->SetActorRotation(LookAt);
}

void AFPEnemyAIController::AttackIfInRange()
{
	if (!TargetPlayer || !ControlledEnemy) return;

	float Dist = FVector::Dist(ControlledEnemy->GetActorLocation(), TargetPlayer->GetActorLocation());
	if (Dist <= ControlledEnemy->AttackRange)
	{
		ControlledEnemy->AttackPlayer();
	}
}

// === [扩展点] 行为树升级 ===
// 当前在 Tick() 中手动追玩家。后续可以改为：
// 1. 创建 BehaviorTree 资产
// 2. RunBehaviorTree(LoadedBT);
// 3. 在 BT 中使用 EQS 找玩家、Service 更新状态
