
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
#include "ranger_top.h"
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


    std::string text = "Frame ";
    SetTargetFPS(60);

    Top top;
    int frame = 0;
    double currTime = 0.0;
	double lastTime = 0;

    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        
        //input code should be decoupled from render 60fps limit

        //timer.Update();

		//printf("frame %d\n",frame);
        

		prbs::Vector2 touch = GetTouch();

		if (touch.x == 0 && touch.y == 0)
		{
			if (top.stack != nullptr)
			{
				ResetTop(&top);
			}
		}
		else
		{
			InsertTopCoord(&top, touch.x, touch.y);
		}
        //ForceTopDistanceLimit(&top);

		if (checkSnakeIntersection(&top) == true)
		{
			//printf("INTERSECTED\n");
		}

        

		//if (IsPolygonClosed(top.stack, touch.x, touch.y))
		//{
		//	printf("closed circle\n");
		//	ResetTop(&top);
		//}

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
            //ClearBackground(GRAY);

            
            // == DRAWING ==

            DrawCurrentPolygonOnlyLines(top.stack);

            //===============

            //DrawTexture(texture, screenWidth/2 - texture.width/2, screenHeight/2 - texture.height/2, WHITE);
            garchompAnim0->Draw(screenWidth / 2 - texture.width / 2, screenHeight / 2 - texture.height / 2);
            garchompAnim0->advanceFrame(frame);


            //======================
			DrawText(("fps " + std::to_string(GetFPS())).c_str(), 360, 90, 40, GRAY);
			DrawText(("touch " + std::to_string(touch.x) + " " +std::to_string(touch.y)).c_str(), 360, 190, 40, GRAY);
            DrawText((text + std::to_string(frame)).c_str() , 360, 370, 40, GRAY);
			DrawText(("Time " + std::to_string(currTime) + " deltaTime " + std::to_string(GetFrameTime())).c_str(),
                360, 230, 40, GRAY);

            //========== 

            if (GuiTextBox(Rectangle({ 25, 215, 125, 30 }), textBoxText, 64, textBoxEditMode)) textBoxEditMode = !textBoxEditMode;

            //=========================
            //timer related stuff

			frame++;
            currTime += GetFrameTime();
			
   //         if (currTime > lastTime + 0.017) {

			//	Coord* lastCoord = ExtractFIFO(&stack);
			//	if (lastCoord != nullptr)
			//	{
			//		free(lastCoord); lastCoord = nullptr;
			//	}
			//	lastTime = currTime;
			//}

        EndDrawing();


        //----------------------------------------------------------------------------------
		//wait or end of frame
		//timer.FrameSleep();
    }

	ResetTop(&top);

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(texture);       // Texture unloading

    CloseWindow();                // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}