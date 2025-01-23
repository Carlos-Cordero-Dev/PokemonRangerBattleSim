
#include <stdlib.h>

#ifdef SWITCH_BUILD
#include <switch.h>
#endif

#include "raylib.h"
#include "rlgl.h" //rlFramebuffer

#define RAYGUI_IMPLEMENTATION
#ifdef SWITCH_BUILD 
#include "raygui.h"
#else 
#include "raygui_win.h"
#include <crtdbg.h> //memory leaks check
#endif

#define GLSL_VERSION 330

#include "constants.h"
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

#ifdef WINDOWS_BUILD 
    //memory leaks check
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1280;
    const int screenHeight = 720;

    char textBoxText[64] = "Text box";
    bool textBoxEditMode = false;

    InitWindow(screenWidth, screenHeight, "raylib [textures] example - texture loading and drawing");
	printf("\n=====================damnson1=================================\n");

	std::string shader_path = "shaders/stylus_tail.fs";
	std::string absolute_shader_path = RESOURCES_FOLDER + shader_path;
	Shader testShader = LoadShader(0/*null so no vs*/, absolute_shader_path.c_str());

	unsigned int framebuffer = rlLoadFramebuffer();

	rlEnableFramebuffer(framebuffer);
    unsigned int stylus_texture = rlLoadTexture(NULL, screenWidth, screenHeight, RL_PIXELFORMAT_UNCOMPRESSED_R16G16B16, 1);
    rlActiveDrawBuffers(1);
	rlFramebufferAttach(framebuffer, stylus_texture, RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_TEXTURE2D, 0);

	// Make sure our framebuffer is complete.
    // NOTE: rlFramebufferComplete() automatically unbinds the framebuffer, so we don't have
    // to rlDisableFramebuffer() here.
	if (!rlFramebufferComplete(framebuffer))
	{
		printf("Framebuffer is not complete");
		CloseWindow();
	}

    // NOTE: Textures MUST be loaded after Window initialization (OpenGL context is required)
    Texture2D texture;
    // texture = LoadTexture("romfs:/resources/raylib_logo.png");        // Texture loading
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

        ComputeAndUpdateDistance(&top);
        ForceTopDistanceLimit(&top);

		if (checkSnakeIntersection(&top) == true)
		{
			//printf("INTERSECTED\n");
		}

        

		//if (IsPolygonClosed(top.stack, touch.x, touch.y))
		//{
		//	printf("closed circle\n");
		//	ResetTop(&top);
		//}
		BeginShaderMode(testShader);

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            // ==== 
	        // Draw to the geometry buffer by first activating it
	        rlEnableFramebuffer(framebuffer);
	        rlClearColor(0, 0, 0, 0);
	        rlClearScreenBuffers();
            // ==== 


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

		// Go back to the default framebuffer (0) and draw our deferred shading.
		rlDisableFramebuffer();
		rlClearScreenBuffers(); // Clear color & depth buffer
    
        EndDrawing();

		EndShaderMode();

        //----------------------------------------------------------------------------------
		//wait or end of frame
		//timer.FrameSleep();
    }

	ResetTop(&top);

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadShader(testShader);
    UnloadTexture(texture);       // Texture unloading

    CloseWindow();                // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}