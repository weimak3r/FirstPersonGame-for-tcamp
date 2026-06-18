#include "FPGameState.h"
#include "FPGameMode.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"

void AFPGameState::AddScore(int32 PlayerID, int32 Points)
{
	if (!HasAuthority()) return;

	if (PlayerID >= PlayerScores.Num())
	{
		PlayerScores.SetNum(PlayerID + 1);
	}

	PlayerScores[PlayerID] += Points;
	CheckWinCondition();
}

void AFPGameState::ResetMatch()
{
	if (!HasAuthority()) return;
	PlayerScores.Empty();
	WinnerPlayerID = -1;
}

void AFPGameState::CheckWinCondition()
{
	if (WinnerPlayerID != -1) return;

	for (int32 i = 0; i < PlayerScores.Num(); i++)
	{
		if (PlayerScores[i] >= TargetKills * 10)
		{
			WinnerPlayerID = i;

			if (AFPGameMode* GM = GetWorld()->GetAuthGameMode<AFPGameMode>())
			{
				APlayerController* Winner = nullptr;
				for (auto It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
				{
					APlayerController* PC = It->Get();
					if (PC)
					{
						if (APlayerState* PS = PC->GetPlayerState<APlayerState>())
						{
							if (PS->GetPlayerId() == i)
							{
								Winner = PC;
								break;
							}
						}
					}
				}
				if (Winner) GM->AnnounceVictory(Winner);
			}
			break;
		}
	}
}

void AFPGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFPGameState, TargetKills);
	DOREPLIFETIME(AFPGameState, PlayerScores);
	DOREPLIFETIME(AFPGameState, WinnerPlayerID);
}
