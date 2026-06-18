#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "FPGameHUDWidget.generated.h"

UCLASS()
class FIRSTPERSONGAME_API UFPGameHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* ScoreText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* VictoryText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* Crosshair;

	void UpdateHealth(int32 Current, int32 Max);
	void UpdateScore(int32 Score);
	void ShowVictory();
	void HideVictory();

	// === [扩展点] 更多 UI 元素 ===
	// 后续在蓝图中添加并绑定：
	// - AmmoText: 当前弹药 / 最大弹药
	// - ReloadProgress: 换弹进度条
	// - Minimap: 小地图（调用 UMG_Image 动态材质）
};
