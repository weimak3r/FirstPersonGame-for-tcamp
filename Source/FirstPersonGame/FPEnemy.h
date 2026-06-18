#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPEnemy.generated.h"

UCLASS()
class FIRSTPERSONGAME_API AFPEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	AFPEnemy();

protected:
	virtual void BeginPlay() override;

	FVector InitialLocation;
	FRotator InitialRotation;

	UPROPERTY(EditAnywhere, Category = "Combat")
	int32 MaxHealth = 60;

	UPROPERTY(Replicated)
	int32 CurrentHealth;

	float LastAttackTime = -999.0f;

public:
	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackRange = 150.0f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackDamage = 15.0f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackCooldown = 1.5f;

	void AttackPlayer();
	bool CanAttack() const;
	bool IsDead() const;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastDie();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastResetEnemy();

	void ResetEnemy();

	// === [扩展点] 动画系统 ===
	// 后续在动画蓝图中绑定:
	// UFUNCTION(BlueprintCallable)
	// void PlayAttackMontage();
	// void PlayDeathMontage();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;
};
