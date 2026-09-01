
#pragma once


struct UIElement
{
    virtual void Update() = 0;
    virtual void Draw() const = 0;
};

struct UIPlayerHealthBar : public UIElement
{
    void Update() override;
    void Draw() const override;
    float healthPercentage = 1.0f; // 0.0 to 1.0
};

class UILayer
{
public:
    void Update(); // only for menu state, button state, animation, etc.
    void Draw() const;

	UIPlayerHealthBar playerHealthBar;
};