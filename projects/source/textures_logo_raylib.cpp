/*******************************************************************************************
*
*   raylib [textures] example - Texture loading and drawing
*
*   This example has been created using raylib 1.0 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2014 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#ifdef SWITCH_BUILD
#include <switch.h>
#endif // TMPDEFINE 

#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#ifdef SWITCH_BUILD 
#include "raygui.h"
#else 
#include "raygui_win.h"
#endif

#include "test.h"
#include "sprites.h"

/*
what do I need man:


-touchscreen support
-importing sprites + anims
-spiny thingy

AI:
- movement
- atacks: single hitboxes + 


-special player atacks
-terrains?

-menu
-level selector
*/

int main(void)
{
    MyClass* h = new MyClass();

    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1280;
    const int screenHeight = 720;

    char textBoxText[64] = "Text box";
    bool textBoxEditMode = false;

    InitWindow(screenWidth, screenHeight, "raylib [textures] example - texture loading and drawing");

    // NOTE: Textures MUST be loaded after Window initialization (OpenGL context is required)
    Texture2D texture;// = LoadTexture("romfs:/resources/raylib_logo.png");        // Texture loading
    loadTexturesFromFolder("romfs:/resources/sprites");
    texture = textures[0].texture;
    //---------------------------------------------------------------------------------------
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------
        static int frame = 0;
        frame++;
		//printf("frame %d\n",frame);
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

            if (GuiTextBox(Rectangle({ 25, 215, 125, 30 }), textBoxText, 64, textBoxEditMode)) textBoxEditMode = !textBoxEditMode;

            DrawTexture(texture, screenWidth/2 - texture.width/2, screenHeight/2 - texture.height/2, WHITE);

            DrawText("this IS a texture!", 360, 370, 10, GRAY);

        EndDrawing();
        //----------------------------------------------------------------------------------

    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(texture);       // Texture unloading

    CloseWindow();                // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}