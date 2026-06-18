#include "FPCharacter.h"
#include "FPEnemy.h"
#include "FPPlayerController.h"
#include "FPGameState.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerStart.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Net/UnrealNetwork.h"
#include "Engine/DamageEvents.h"
#include "Engine/World.h"
#include "InputCoreTypes.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

AFPCharacter::AFPCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	// 弹簧臂
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 0.0f;          // 真第一人称，不拉远
	SpringArm->bUsePawnControlRotation = true;

	// 摄像机
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	// 枪模型（占位方块，后续替换为正式枪械模型）
	GunMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GunMesh"));
	GunMesh->SetupAttachment(Camera);
	GunMesh->SetRelativeLocation(FVector(30.0f, 15.0f, -15.0f));
	GunMesh->SetRelativeScale3D(FVector(0.08f, 0.05f, 0.05f));
	GunMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 基础值
	CurrentHealth = MaxHealth;
	bReplicates = true;
}

void AFPCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AFPCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!Input) return;

	Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFPCharacter::Move);
	Input->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFPCharacter::Look);
	Input->BindAction(JumpAction, ETriggerEvent::Started,   this, &AFPCharacter::StartJump);
	Input->BindAction(JumpAction, ETriggerEvent::Completed, this, &AFPCharacter::StopJump);
	Input->BindAction(ShootAction, ETriggerEvent::Started,  this, &AFPCharacter::Shoot);
	PlayerInputComponent->BindKey(EKeys::R, IE_Pressed, this, &AFPCharacter::RestartLevel);
}

// --- 移动 ---
void AFPCharacter::Move(const FInputActionValue& Value)
{
	FVector2D V = Value.Get<FVector2D>();
	if (Controller && !V.IsNearlyZero())
	{
		AddMovementInput(GetActorForwardVector(), V.Y);
		AddMovementInput(GetActorRightVector(),   V.X);
	}
}

void AFPCharacter::Look(const FInputActionValue& Value)
{
	FVector2D V = Value.Get<FVector2D>();
	if (Controller)
	{
		AddControllerYawInput(V.X);
		AddControllerPitchInput(V.Y);
	}
}

void AFPCharacter::StartJump()  { Jump(); }
void AFPCharacter::StopJump()   { StopJumping(); }

void AFPCharacter::RestartLevel()
{
	if (HasAuthority())
	{
		ServerRestartLevel_Implementation();
	}
	else
	{
		ServerRestartLevel();
	}
}

void AFPCharacter::ServerRestartLevel_Implementation()
{
	UWorld* World = GetWorld();
	if (!World) return;

	if (AFPGameState* GS = World->GetGameState<AFPGameState>())
	{
		GS->ResetMatch();
	}

	TArray<APlayerStart*> PlayerStarts;
	for (TActorIterator<APlayerStart> It(World); It; ++It)
	{
		PlayerStarts.Add(*It);
	}

	int32 PlayerIndex = 0;
	for (TActorIterator<AFPCharacter> It(World); It; ++It)
	{
		AFPCharacter* PlayerChar = *It;
		PlayerChar->CurrentHealth = PlayerChar->MaxHealth;
		PlayerChar->KillCount = 0;
		PlayerChar->OnRep_CurrentHealth();

		if (PlayerStarts.Num() > 0)
		{
			APlayerStart* StartPoint = PlayerStarts[PlayerIndex % PlayerStarts.Num()];
			PlayerChar->SetActorLocation(StartPoint->GetActorLocation());
			PlayerChar->SetActorRotation(StartPoint->GetActorRotation());
			PlayerIndex++;
		}
	}

	for (TActorIterator<AFPEnemy> It(World); It; ++It)
	{
		It->ResetEnemy();
	}

	for (auto It = World->GetPlayerControllerIterator(); It; ++It)
	{
		if (AFPPlayerController* PC = Cast<AFPPlayerController>(It->Get()))
		{
			PC->ClientResetHUD();
		}
	}
}

// --- 射击（核心）---
void AFPCharacter::Shoot()
{
	FVector Start = GetPawnViewLocation();
	FVector Dir = GetViewRotation().Vector();

	float HalfAngleRad = FMath::DegreesToRadians(BulletSpread * 0.5f);
	Dir = FMath::VRandCone(Dir, HalfAngleRad);

	FVector End = Start + Dir * ShootRange;

	// 客户端：通知服务端做权威判定和统一广播
	if (!HasAuthority())
	{
		ServerShoot();
		return;
	}

	// 服务端：权威射线检测 + 伤害
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);
	if (!bHit)
	{
		bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Pawn, Params);
	}

	if (bHit)
	{
		End = Hit.ImpactPoint;

		if (AFPEnemy* Enemy = Cast<AFPEnemy>(Hit.GetActor()))
		{
			FDamageEvent DamageEvent;
			Enemy->TakeDamage(ShootDamage, DamageEvent, GetController(), this);
		}
	}

	// 广播弹道到所有玩家窗口
	for (auto It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (AFPPlayerController* PC = Cast<AFPPlayerController>(It->Get()))
		{
			PC->ClientShowShootEffect(Start, End);
		}
	}
}

void AFPCharacter::ServerShoot_Implementation()
{
	Shoot();
}

void AFPCharacter::AddKill()
{
	KillCount++;
	// 通知 GameState 加分（仅服务端）
	if (HasAuthority())
	{
		if (AFPPlayerController* PC = Cast<AFPPlayerController>(GetController()))
		{
			PC->Server_ReportKill();
		}
	}
}

// --- 血量 ---
float AFPCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	if (!HasAuthority()) return 0;

	float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage <= 0.0f)
	{
		ActualDamage = Damage;
	}

	CurrentHealth = FMath::Max(0, CurrentHealth - (int32)ActualDamage);

	OnRep_CurrentHealth();

	if (CurrentHealth <= 0)
	{
		if (AFPCharacter* Killer = Cast<AFPCharacter>(DamageCauser))
		{
			if (Killer != this)
			{
				Killer->AddKill();
			}
		}

		CurrentHealth = MaxHealth;
		OnRep_CurrentHealth();

		for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
		{
			SetActorLocation(It->GetActorLocation());
			SetActorRotation(It->GetActorRotation());
			break;
		}
	}
	return ActualDamage;
}

void AFPCharacter::OnRep_CurrentHealth()
{
	// 客户端收到血量更新时触发（UI 绑定此变量即可刷新）
}

// === [扩展点] 换弹 ===
// void AFPCharacter::Reload()
// {
//     if (CurrentAmmo >= MaxAmmo) return;
//     // 播放换弹动画: PlayReloadMontage();
//     CurrentAmmo = MaxAmmo;
// }

// === [扩展点] 血包拾取 ===
// void AFPCharacter::Heal(int32 Amount)
// {
//     CurrentHealth = FMath::Min(MaxHealth, CurrentHealth + Amount);
//     OnRep_CurrentHealth();
// }

// --- 网络复制 ---
void AFPCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFPCharacter, CurrentHealth);
}
