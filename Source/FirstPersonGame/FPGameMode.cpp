#include "FPGameMode.h"
#include "FPGameState.h"
#include "FPPlayerController.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

AFPGameMode::AFPGameMode()
{
	GameStateClass = AFPGameState::StaticClass();
}

void AFPGameMode::AnnounceVictory(APlayerController* Winner)
{
	if (AFPPlayerController* PC = Cast<AFPPlayerController>(Winner))
	{
		PC->ClientNotifyVictory();
	}
}
