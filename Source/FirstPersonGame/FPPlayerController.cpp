#include "FPPlayerController.h"
#include "FPGameHUDWidget.h"
#include "FPCharacter.h"
#include "FPGameState.h"
#include "Blueprint/UserWidget.h"
#include "TimerManager.h"
#include "GameFramework/PlayerState.h"
#include "DrawDebugHelpers.h"

void AFPPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() && HUDWidgetClass)
	{
		HUDWidget = CreateWidget<UFPGameHUDWidget>(this, HUDWidgetClass);
		if (HUDWidget)
		{
			HUDWidget->AddToViewport();
			// 定时刷新 HUD（每秒更新一次血量/得分）
			FTimerHandle Timer;
			GetWorldTimerManager().SetTimer(Timer,
				this, &AFPPlayerController::UpdateHUD, 0.2f, true);
		}
	}
}

void AFPPlayerController::Server_ReportKill_Implementation()
{
	if (AFPGameState* GS = GetWorld()->GetGameState<AFPGameState>())
	{
		APlayerState* PS = GetPlayerState<APlayerState>();
		if (PS)
		{
			GS->AddScore(PS->GetPlayerId(), 10);
		}
	}
}

void AFPPlayerController::ClientNotifyVictory_Implementation()
{
	if (HUDWidget) HUDWidget->ShowVictory();
}

void AFPPlayerController::ClientResetHUD_Implementation()
{
	if (HUDWidget) HUDWidget->HideVictory();
}

void AFPPlayerController::ClientShowShootEffect_Implementation(FVector Start, FVector End)
{
	DrawDebugLine(GetWorld(), Start, End, FColor::Blue, false, 0.12f, 0, 1.0f);
}

void AFPPlayerController::UpdateHUD()
{
	if (!HUDWidget) return;

	AFPCharacter* MyChar = Cast<AFPCharacter>(GetPawn());
	if (MyChar)
	{
		HUDWidget->UpdateHealth(MyChar->GetCurrentHealth(), MyChar->GetMaxHealth());
	}

	// 从 GameState 读取得分（已复制到所有客户端）
	AFPGameState* GS = GetWorld()->GetGameState<AFPGameState>();
	APlayerState* PS = GetPlayerState<APlayerState>();
	if (GS && PS && PS->GetPlayerId() < GS->PlayerScores.Num())
	{
		int32 MyScore = GS->PlayerScores[PS->GetPlayerId()];
		HUDWidget->UpdateScore(MyScore);
	}
}
