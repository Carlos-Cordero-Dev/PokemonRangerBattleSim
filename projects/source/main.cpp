
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
#include "FIFO.h"
#include "player_polygon.h"
/*
what do I need man:


-touchscreen support [x]
-importing sprites + anims [x]
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

	Coord* stack = nullptr;

    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        
        //input code should be decoupled from render 60fps limit

        timer.Update();

		//printf("frame %d\n",frame);
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));


            prbs::Vector2 touch = GetTouch();
            
            if (touch.x == 0 && touch.y == 0)
            {
                if (stack != nullptr)
                {
                    DestroyStack(&stack); stack = nullptr;
                }
            }
            else
            {
                InsertCoord(&stack, touch.x, touch.y);
            }
            // == DRAWING ==

			DrawCurrentPolygon(stack);

            //===============

            //DrawTexture(texture, screenWidth/2 - texture.width/2, screenHeight/2 - texture.height/2, WHITE);
            garchompAnim0->Draw(screenWidth / 2 - texture.width / 2, screenHeight / 2 - texture.height / 2);
            garchompAnim0->advanceFrame(timer.frame);


            //======================
			DrawText(("touch " + std::to_string(touch.x) + " " +std::to_string(touch.y)).c_str(), 360, 200, 40, GRAY);
            DrawText((text + std::to_string(timer.frame)).c_str() , 360, 370, 40, GRAY);
			DrawText(("Time " + std::to_string(timer.get_time_ms()) + " deltaTime " + std::to_string(timer.get_delta_time())).c_str(),
                360, 230, 40, GRAY);

            //========== 

            if (GuiTextBox(Rectangle({ 25, 215, 125, 30 }), textBoxText, 64, textBoxEditMode)) textBoxEditMode = !textBoxEditMode;

        EndDrawing();

        static int lastTime = 0;
		if (timer.get_time_ms() > lastTime + 1000/50) {

			Coord* lastCoord = ExtractFIFO(&stack);
			free(lastCoord); lastCoord = nullptr;
			lastTime = timer.get_time_ms();
		}

        //----------------------------------------------------------------------------------
		//wait or end of frame
		timer.FrameSleep();
    }

	DestroyStack(&stack); stack = nullptr;

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(texture);       // Texture unloading

    CloseWindow();                // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}