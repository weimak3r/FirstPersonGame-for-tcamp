#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FPPlayerController.generated.h"

UCLASS()
class FIRSTPERSONGAME_API AFPPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	// HUD Widget 类（在编辑器中指定蓝图子类）
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UFPGameHUDWidget> HUDWidgetClass;

	// 服务端 RPC：玩家击杀敌人，通知 GameState 加分
	UFUNCTION(Server, Reliable)
	void Server_ReportKill();

	// 客户端 RPC：弹出胜利提示
	UFUNCTION(Client, Reliable)
	void ClientNotifyVictory();

	// 客户端 RPC：隐藏胜利提示
	UFUNCTION(Client, Reliable)
	void ClientResetHUD();

	// 客户端 RPC：显示射击弹道
	UFUNCTION(Client, Reliable)
	void ClientShowShootEffect(FVector Start, FVector End);

	// === [扩展点] 语音/聊天系统 ===
	// 后续添加：
	// UFUNCTION(Server, Reliable)
	// void Server_SendChatMessage(const FString& Msg);
	// UFUNCTION(Client, Reliable)
	// void Client_ReceiveChatMessage(const FString& Msg, const FString& Sender);

private:
	UPROPERTY()
	UFPGameHUDWidget* HUDWidget;

	void UpdateHUD();
};
