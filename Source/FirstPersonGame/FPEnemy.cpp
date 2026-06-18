#include "FPEnemy.h"
#include "FPCharacter.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Engine/DamageEvents.h"
#include "Net/UnrealNetwork.h"

AFPEnemy::AFPEnemy()
{
	PrimaryActorTick.bCanEverTick = false;
	CurrentHealth = MaxHealth;
	Tags.Add(TEXT("Enemy"));
	bReplicates = true;
	SetReplicateMovement(true);
}

void AFPEnemy::BeginPlay()
{
	Super::BeginPlay();
	InitialLocation = GetActorLocation();
	InitialRotation = GetActorRotation();
}

// --- 攻击 ---
void AFPEnemy::AttackPlayer()
{
	if (!CanAttack() || IsDead()) return;

	LastAttackTime = GetWorld()->GetTimeSeconds();

	// 直接伤害范围内的玩家
	FVector MyLoc = GetActorLocation();
	UWorld* World = GetWorld();
	for (TActorIterator<AFPCharacter> It(World); It; ++It)
	{
		if (FVector::Dist(MyLoc, It->GetActorLocation()) <= AttackRange)
		{
			FDamageEvent DmgEvent;
			It->TakeDamage(AttackDamage, DmgEvent, GetController(), this);
			break;
		}
	}

	// === [扩展点] 攻击动画 ===
	// PlayAttackMontage();
}

bool AFPEnemy::CanAttack() const
{
	return GetWorld()->GetTimeSeconds() - LastAttackTime >= AttackCooldown;
}

bool AFPEnemy::IsDead() const
{
	return CurrentHealth <= 0;
}

// --- 受伤 ---
float AFPEnemy::TakeDamage(float Damage, FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	if (!HasAuthority()) return 0.0f;

	float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	// 基类可能返回 0，直接用传入的 Damage 保证扣血
	if (ActualDamage <= 0.0f)
	{
		ActualDamage = Damage;
	}

	CurrentHealth -= (int32)ActualDamage;

	if (CurrentHealth <= 0)
	{
		CurrentHealth = 0;

		// 通知杀敌玩家加分
		if (AFPCharacter* Killer = Cast<AFPCharacter>(DamageCauser))
		{
			Killer->AddKill();
		}

		MulticastDie();
	}
	return ActualDamage;
}

void AFPEnemy::MulticastDie_Implementation()
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);
}

void AFPEnemy::ResetEnemy()
{
	if (!HasAuthority()) return;

	CurrentHealth = MaxHealth;
	SetActorLocation(InitialLocation);
	SetActorRotation(InitialRotation);
	MulticastResetEnemy();
}

void AFPEnemy::MulticastResetEnemy_Implementation()
{
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(false);
}

void AFPEnemy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFPEnemy, CurrentHealth);
}
