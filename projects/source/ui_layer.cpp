
#include "ui_layer.h"
#include "raylib.h"

void UIPlayerHealthBar::Draw() const
{
	// Draw the health bar background
	DrawRectangle(10, 10, 200, 20, DARKGRAY);
	// Draw the health bar foreground based on healthPercentage
	DrawRectangle(10, 10, static_cast<int>(200 * healthPercentage), 20, RED);
}

void UIPlayerHealthBar::Update()
{
	// For demonstration purposes, let's decrease health over time
	healthPercentage -= 0.0001f; // Decrease health by 1% per update
	if (healthPercentage < 0.0f) {
		healthPercentage = 0.0f; // Clamp to 0
	}
}

void UILayer::Draw() const
{
	playerHealthBar.Draw();
}

void UILayer::Update()
{
	playerHealthBar.Update();
}

