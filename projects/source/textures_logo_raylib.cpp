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

#include <stdlib.h>

#ifdef SWITCH_BUILD
#include <switch.h>
#endif

#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#ifdef SWITCH_BUILD 
#include "raygui.h"
#else 
#include "raygui_win.h"
#endif

#include "sprites.h"
#include "timer.h"
#include "controls.h"
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
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1280;
    const int screenHeight = 720;

    char textBoxText[64] = "Text box";
    bool textBoxEditMode = false;

    InitWindow(screenWidth, screenHeight, "raylib [textures] example - texture loading and drawing");
	printf("\n=====================damnson1=================================\n");

    // NOTE: Textures MUST be loaded after Window initialization (OpenGL context is required)
    Texture2D texture;// = LoadTexture("romfs:/resources/raylib_logo.png");        // Texture loading
    //loadTexturesFromFolder("romfs:/resources/sprites");
    //texture = textures[0].texture;
	SpriteAnimation* garchompAnim0 = new SpriteAnimation();
    garchompAnim0->advanceRate = 5;
	SpriteAnimation* garchompAnim1 = new SpriteAnimation();
	SpriteAnimation* garchompAnim2 = new SpriteAnimation();
	SpriteAnimation* garchompAnim3 = new SpriteAnimation();

	SpriteAnimation** garchompAnims = new SpriteAnimation*[4];
	garchompAnims[0] = garchompAnim0;
	garchompAnims[1] = garchompAnim1;
	garchompAnims[2] = garchompAnim2;
	garchompAnims[3] = garchompAnim3;
    
    loadTexturesFromFolder("sprites/garchomp/attack", garchompAnims, 4, 6, "garchomp_attack");
    texture = garchompAnims[0]->textures[0].texture;
    printf("\ndamnson2=================================\n");
    //---------------------------------------------------------------------------------------
    // Main game loop

    Timer timer;

    std::string text = "Frame ";


	void SetGesturesEnabled(unsigned int flags);      // Enable a set of gestures using flags

    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        
        //input code should be decoupled from render 60fps limit

        timer.Update();

		//printf("frame %d\n",frame);
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

            if (GuiTextBox(Rectangle({ 25, 215, 125, 30 }), textBoxText, 64, textBoxEditMode)) textBoxEditMode = !textBoxEditMode;

            //DrawTexture(texture, screenWidth/2 - texture.width/2, screenHeight/2 - texture.height/2, WHITE);
            garchompAnim0->Draw(screenWidth / 2 - texture.width / 2, screenHeight / 2 - texture.height / 2);
            garchompAnim0->advanceFrame(timer.frame);


            prbs::Vector2 touch = GetTouch();

			DrawText(("touch " + std::to_string(touch.x) + " " +std::to_string(touch.y)).c_str(), 360, 200, 40, GRAY);
            DrawText((text + std::to_string(timer.frame)).c_str() , 360, 370, 40, GRAY);
			DrawText(("Time " + std::to_string(timer.get_time_ms()) + " deltaTime " + std::to_string(timer.get_delta_time())).c_str(),
                360, 230, 40, GRAY);

        EndDrawing();
        //----------------------------------------------------------------------------------
		//wait or end of frame
		timer.FrameSleep();
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(texture);       // Texture unloading

    CloseWindow();                // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}