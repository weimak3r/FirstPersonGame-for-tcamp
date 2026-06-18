#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UInputMappingContext;
class UInputAction;
class UStaticMeshComponent;
class AFPPlayerController;

UCLASS()
class FIRSTPERSONGAME_API AFPCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AFPCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// --- 摄像机 ---
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;

	// --- 枪模型 ---
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* GunMesh;

	// --- 输入 ---
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* ShootAction;

	void Move(const struct FInputActionValue& Value);
	void Look(const struct FInputActionValue& Value);
	void StartJump();
	void StopJump();
	void RestartLevel();

	UFUNCTION(Server, Reliable)
	void ServerRestartLevel();

	// --- 射击 ---
	void Shoot();

	UFUNCTION(Server, Reliable)
	void ServerShoot();

	UPROPERTY(EditAnywhere, Category = "Combat")
	float ShootRange = 10000.0f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float ShootDamage = 20.0f;

	// 弹道散布角度（度），越小越精准
	UPROPERTY(EditAnywhere, Category = "Combat")
	float BulletSpread = 2.0f;

	// === [扩展点] 换弹系统 ===
	// 后续添加: CurrentAmmo, MaxAmmo, Reload() 函数
	// UPROPERTY(EditAnywhere, Category = "Combat")
	// int32 MaxAmmo = 30;
	// int32 CurrentAmmo;

	// === [扩展点] 动画系统 ===
	// 后续在动画蓝图中绑定对应的 Animation Montage
	// UFUNCTION(BlueprintCallable)
	// void PlayFireMontage();
	// void PlayHitMontage();

	// --- 血量 & 得分 ---
	UPROPERTY(EditAnywhere, Category = "Combat")
	int32 MaxHealth = 100;

	UPROPERTY(Replicated)
	int32 CurrentHealth;

	int32 KillCount = 0;

	UFUNCTION()
	void OnRep_CurrentHealth();

public:
	// 玩家得分（在 GameState 中累计，此处仅缓存本地击杀数供 UI 显示）
	void AddKill();

	// === [扩展点] 血包/道具拾取 ===
	// 后续 APickupItem 类调用此函数
	// void Heal(int32 Amount);

	// 获取当前血量用于 UI 绑定
	FORCEINLINE int32 GetCurrentHealth() const { return CurrentHealth; }
	FORCEINLINE int32 GetMaxHealth() const { return MaxHealth; }

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;
};
