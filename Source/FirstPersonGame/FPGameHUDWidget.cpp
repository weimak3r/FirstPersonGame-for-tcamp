#include "FPGameHUDWidget.h"

void UFPGameHUDWidget::UpdateHealth(int32 Current, int32 Max)
{
	if (HealthBar) HealthBar->SetPercent((float)Current / (float)Max);
}

void UFPGameHUDWidget::UpdateScore(int32 Score)
{
	if (ScoreText) ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), Score)));
}

void UFPGameHUDWidget::ShowVictory()
{
	if (VictoryText)
	{
		VictoryText->SetVisibility(ESlateVisibility::Visible);
		VictoryText->SetText(FText::FromString(TEXT("YOU WIN!")));
	}
}

void UFPGameHUDWidget::HideVictory()
{
	if (VictoryText)
	{
		VictoryText->SetVisibility(ESlateVisibility::Hidden);
	}
}
