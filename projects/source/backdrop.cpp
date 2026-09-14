
#include "backdrop.h"
#include "raymath.h"

void Backdrop::Load(const std::string& backdrop_path)
{
	Texture2D texture = LoadTexture(backdrop_path.c_str());
	backdropTexture.texture = texture;
	backdropTexture.name = backdrop_path;
}

float Backdrop::GetWidth(int renderHeight)
{
    const float scale = static_cast<float>(renderHeight) / backdropTexture.texture.height;
    const float backdropWidth = backdropTexture.texture.width * scale;
    return backdropWidth;
}

void Backdrop::DrawCenteredBackdrop(int renderWidth, int renderHeight) const
{
    const Rectangle destination = GetPlayableArea(renderWidth, renderHeight);

    // Replace this with colors sampled from the image or side-panel art.
    ClearBackground(Color{ 34, 57, 48, 255 });

    Rectangle source = { 0, 0, static_cast<float>(backdropTexture.texture.width),
                         static_cast<float>(backdropTexture.texture.height) };

	//Rectangle destination = { 0, 0 , static_cast<float>(backdropTexture.texture.width), static_cast<float>(backdropTexture.texture.height) };

    DrawTexturePro(backdropTexture.texture, source, destination, Vector2Zero(), 0.0f, WHITE);
}

Rectangle Backdrop::GetPlayableArea(int renderWidth, int renderHeight) const
{
    const float scale = static_cast<float>(renderHeight) / backdropTexture.texture.height;
    const float backdropWidth = backdropTexture.texture.width * scale;
    return {
        (static_cast<float>(renderWidth) - backdropWidth) * 0.5f,
        0.0f,
        backdropWidth,
        static_cast<float>(renderHeight)
    };
}