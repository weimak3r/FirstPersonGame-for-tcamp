#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "FPEnemyAIController.generated.h"

UCLASS()
class FIRSTPERSONGAME_API AFPEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AFPEnemyAIController();

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;

private:
	class AFPEnemy* ControlledEnemy;
	class AFPCharacter* TargetPlayer;

	void ChasePlayer();
	void AttackIfInRange();

	UPROPERTY(EditAnywhere, Category = "AI")
	float AcceptanceRadius = 120.0f;

	// === [扩展点] 行为树/高级AI ===
	// 后续可将 Tick 中的逻辑替换为 Behavior Tree：
	// - EnemyDetected: 追击分支
	// - InAttackRange: 攻击分支
	// - EnemyLost: 巡逻分支
};
