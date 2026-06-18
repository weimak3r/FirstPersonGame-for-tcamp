#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FPGameMode.generated.h"

UCLASS()
class FIRSTPERSONGAME_API AFPGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AFPGameMode();

	// 由 GameState 调用，宣布胜利
	void AnnounceVictory(class APlayerController* Winner);

	// === [扩展点] 游戏模式 ===
	// 后续可继承此类实现不同模式：
	// - TeamDeathMatch: 队伍计分
	// - TimeAttack: 限时击杀
	// - Survival: 无限波次敌人
};
