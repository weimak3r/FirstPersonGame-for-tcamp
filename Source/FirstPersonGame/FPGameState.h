#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "FPGameState.generated.h"

UCLASS()
class FIRSTPERSONGAME_API AFPGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	// 击杀目标
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game")
	int32 TargetKills = 5;

	// 玩家得分表（PlayerControllerPtr → Score）
	UPROPERTY(Replicated)
	TArray<int32> PlayerScores;

	// 胜利玩家ID（-1 = 无人胜利）
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game")
	int32 WinnerPlayerID = -1;

	void AddScore(int32 PlayerID, int32 Points);
	void ResetMatch();

	// === [扩展点] 更多统计数据 ===
	// 后续可添加：
	// - PlayerDeaths: 死亡次数
	// - MatchTime: 比赛时长
	// - TeamScores: 队伍分

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void CheckWinCondition();
};
