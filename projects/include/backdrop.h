
#pragma once

#include "sprites.h" //for TextureData

class Backdrop
{
public:
    //void Draw() const;
    void DrawCenteredBackdrop(int renderWidth, int renderHeight) const;

    void Load(const std::string& backdrop_path);

private:
	TextureData backdropTexture;
};

