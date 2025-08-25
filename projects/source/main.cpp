
#include <stdlib.h>

#ifdef SWITCH_BUILD
#include <switch.h>
#endif

#include "raylib.h"
#define GRAPHICS_API_OPENGL_33
#include "rlgl.h" //rlFramebuffer
//#include "glad.h" //glBindBuffer
#include <glad.h>

#define RAYGUI_IMPLEMENTATION
#ifdef SWITCH_BUILD 
#include "raygui.h"
#else 
#include "raygui_win.h"
#include <crtdbg.h> //memory leaks check
#endif

#define GLSL_VERSION 420

#ifdef DEBUG
#ifdef WINDOWS_BUILD
#include "logging_manager.h"
#endif
#endif

#include "constants.h"
#include "geometry_shader_support.h"
#include "sprites.h"
#include "timer.h"
#include "controls.h"
#include "FIFO.h"
#include "player_polygon.h"
#include "ranger_top.h"

#include "world_object.h"

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


	//initialize logging
	SetLogFileEX("prbs_log.txt");
#endif


    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1280;
    const int screenHeight = 720;

    char textBoxText[64] = "Text box";
    bool textBoxEditMode = false;

    InitWindow(screenWidth, screenHeight, "raylib [textures] example - texture loading and drawing");
	printf("\n=====================damnson1=================================\n");


	//unsigned int stylus_framebuffer = rlLoadFramebuffer();
	//unsigned int main_framebuffer = rlLoadFramebuffer();

	//rlEnableFramebuffer(main_framebuffer);
	//rlEnableFramebuffer(stylus_framebuffer);
	//unsigned int main_texture = rlLoadTexture(NULL, screenWidth, screenHeight, RL_PIXELFORMAT_UNCOMPRESSED_R16G16B16, 1);
	//unsigned int stylus_texture = rlLoadTexture(NULL, screenWidth, screenHeight, RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8/*important for transparency*/, 1);

 //   rlActiveDrawBuffers(2);
	//rlFramebufferAttach(main_framebuffer, main_texture, RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_TEXTURE2D, 0);
	//rlFramebufferAttach(stylus_framebuffer, stylus_texture, RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_TEXTURE2D, 0);

	////texture of stylus shader in Texture2D version to be able to do DrawTexture
	//Texture2D stylusOverlay;
	//stylusOverlay.id = stylus_texture;
	//stylusOverlay.width = screenWidth;
	//stylusOverlay.height = screenHeight;
	//stylusOverlay.mipmaps = 1;
	//stylusOverlay.format = RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;


	// Make sure our framebuffer is complete.
    // NOTE: rlFramebufferComplete() automatically unbinds the framebuffer, so we don't have
    // to rlDisableFramebuffer() here.
#ifndef SWITCH_BUILD
	if (!rlFramebufferComplete(main_framebuffer))
	{
		printf("Framebuffer is not complete");
		CloseWindow();
	}
	if (!rlFramebufferComplete(stylus_framebuffer))
	{
		printf("Framebuffer is not complete");
		CloseWindow();
	}
#endif


    // NOTE: Textures MUST be loaded after Window initialization (OpenGL context is required)

	// texture = LoadTexture("romfs:/resources/raylib_logo.png");        // Texture loading
    //loadTexturesFromFolder("romfs:/resources/sprites");
    //texture = textures[0].texture;

	//SpriteAnimation* garchompAnim0 = new SpriteAnimation();
 //   garchompAnim0->advanceRate = 5;
	//SpriteAnimation* garchompAnim1 = new SpriteAnimation();
	//SpriteAnimation* garchompAnim2 = new SpriteAnimation();
	//SpriteAnimation* garchompAnim3 = new SpriteAnimation();

	//SpriteAnimation** garchompAnims = new SpriteAnimation*[4];
	//garchompAnims[0] = garchompAnim0;
	//garchompAnims[1] = garchompAnim1;
	//garchompAnims[2] = garchompAnim2;
	//garchompAnims[3] = garchompAnim3;
 //   
 //   //loadTexturesFromFolder("sprites/garchomp/attack", garchompAnims, 4, 6, "garchomp_attack");

	//SpriteAnimation* stylusAnim0 = new SpriteAnimation();
	//stylusAnim0->advanceRate = 5;

	//SpriteAnimation** stylusAnims = new SpriteAnimation * [1];
	//stylusAnims[0] = stylusAnim0;

	//loadTexturesFromFolder("sprites/stylus/top", stylusAnims, 1, 3, "top_idle_spin");

    printf("\ndamnson2=================================\n");
	//---------------------------------------------------------------------------------------
	// Main game loop

	std::string text = "Frame ";
	SetTargetFPS(60);

	Top top;

	//WorldObject* wo = new WorldObject(garchompAnim0);
	//wo->position = Vector2({ 100, 100 });

	//WorldObject* topObject = new WorldObject(stylusAnim0);
	//topObject->position = Vector2({ -100, -100 }); //offscreen
	//topObject->scale = 5.0f;

	int frame = 0;
	double currTime = 0.0;
	double lastTime = 0;

	while (!WindowShouldClose())    // Detect window close button or ESC key
	{

		//input code should be decoupled from render 60fps limit

		// ---------------------------------------------------------------------------------
		// Update
		 
		
		//timer.Update();

		//printf("frame %d\n",frame);

		//prbs::Vector2 touch = GetTouch();

		//if (touch.x == 0 && touch.y == 0)
		//{
		//	if (top.stack != nullptr)
		//	{
		//		ResetTop(&top);

		//		//TODO: propper disable just in case instead of offscreen
		//		topObject->position.x = -100;
		//		topObject->position.y = -100;
		//	}
		//}
		//else
		//{
		//	InsertTopCoord(&top, touch.x, touch.y);

		//	topObject->position.x = touch.x;
		//	topObject->position.y = touch.y;
		//}

		//ComputeAndUpdateDistance(&top);
		//ForceTopDistanceLimit(&top);

		//if (checkSnakeIntersection(&top) == true)
		//{
		//	//printf("INTERSECTED\n");
		//}

		////update every single world object
		//wo->Update();

		//
		//topObject->Update();

		// End of Update
		// ---------------------------------------------------------------------------------


		//----------------------------------------------------------------------------------
		// Draw

		BeginDrawing();

		//rlDisableDepthTest();

		//// === STYLUS TEXTURE === 

		//rlEnableFramebuffer(stylus_framebuffer);
		//rlClearColor(0, 1, 0, 1);
		//rlClearScreenBuffers();
		//rlDisableColorBlend();


		//DrawRectangle(0, 0, screenWidth, screenHeight, WHITE);


		////draw stlyus tail

		//if (touch.x != 0 && touch.y != 0)
		//{
		//	//DrawTexture(stylusOverlay, 0, 0, WHITE);



		//	int numberOfNodes = GetStackDepth(top.stack);

		//	//printf("num of nodes: %d\n", numberOfNodes);
		//	float trailThickness = 5.0f;

		//	if (numberOfNodes > 2)
		//	{
		//		Coord* current = top.stack;
		//		Coord* next = top.stack->nextCoord;

		//		for (int i = 0; i < numberOfNodes - 1; i++)
		//		{
		//			int current_x_int = (int)std::round(current->x);
		//			int current_y_int = (int)std::round(current->y);
		//			int next_x_int = (int)std::round(next->x);
		//			int next_y_int = (int)std::round(next->y);

		//			Vector2 startPos = { current_x_int, current_y_int };
		//			Vector2 endPos = { next_x_int, next_y_int };

		//			//trail de fuera
		//			DrawCircleV(startPos, trailThickness, DARKBLUE);
		//			DrawLineEx(startPos, endPos, trailThickness * 2.0f, DARKBLUE);

		//			current = next;
		//			next = next->nextCoord;
		//		}
		//		current = top.stack;
		//		next = top.stack->nextCoord;

		//		for (int i = 0; i < numberOfNodes - 1; i++)
		//		{
		//			int current_x_int = (int)std::round(current->x);
		//			int current_y_int = (int)std::round(current->y);
		//			int next_x_int = (int)std::round(next->x);
		//			int next_y_int = (int)std::round(next->y);

		//			Vector2 startPos = { current_x_int, current_y_int };
		//			Vector2 endPos = { next_x_int, next_y_int };

		//			//trail 2
		//			DrawCircleV(startPos, trailThickness * 2 / 3, BLUE);
		//			DrawLineEx(startPos, endPos, trailThickness * 2 / 3 * 2.0f, BLUE);

		//			current = next;
		//			next = next->nextCoord;
		//		}

		//		current = top.stack;
		//		next = top.stack->nextCoord;

		//		for (int i = 0; i < numberOfNodes - 1; i++)
		//		{
		//			int current_x_int = (int)std::round(current->x);
		//			int current_y_int = (int)std::round(current->y);
		//			int next_x_int = (int)std::round(next->x);
		//			int next_y_int = (int)std::round(next->y);

		//			Vector2 startPos = { current_x_int, current_y_int };
		//			Vector2 endPos = { next_x_int, next_y_int };


		//			//trail 3
		//			DrawCircleV(startPos, trailThickness * 1 / 3, WHITE);
		//			DrawLineEx(startPos, endPos, trailThickness * 1 / 3 * 2.0f, WHITE);

		//			current = next;
		//			next = next->nextCoord;
		//		}

		//	}
		//}


		//rlEnableColorBlend();

		//rlDisableFramebuffer();

		// =======================

		// === MAIN TEXTURE ===

		//rlEnableFramebuffer(main_framebuffer);

		//ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

		//DrawCurrentPolygonOnlyLines(top.stack);



		//DrawText(("fps " + std::to_string(GetFPS())).c_str(), 360, 90, 40, GRAY);
		//DrawText(("touch " + std::to_string(touch.x) + " " + std::to_string(touch.y)).c_str(), 360, 190, 40, GRAY);
		//DrawText((text + std::to_string(frame)).c_str(), 360, 370, 40, GRAY);
		//DrawText(("Time " + std::to_string(currTime) + " deltaTime " + std::to_string(GetFrameTime())).c_str(),
		//	360, 230, 40, GRAY);

		////========== 

		//if (GuiTextBox(Rectangle({ 25, 215, 125, 30 }), textBoxText, 64, textBoxEditMode)) textBoxEditMode = !textBoxEditMode;


		////draw every world object

		//wo->Draw(frame);
		////topObject->Draw(frame);


		////=========================
		////timer related stuff

		//frame++;
		//currTime += GetFrameTime();

		////         if (currTime > lastTime + 0.017) {

		//		 //	Coord* lastCoord = ExtractFIFO(&stack);
		//		 //	if (lastCoord != nullptr)
		//		 //	{
		//		 //		free(lastCoord); lastCoord = nullptr;
		//		 //	}
		//		 //	lastTime = currTime;
		//		 //}

		//rlDisableFramebuffer();
		//rlClearScreenBuffers();

		EndDrawing();


		//----------------------------------------------------------------------------------
		//wait or end of frame
		//timer.FrameSleep();
	}

	ResetTop(&top);

	// De-Initialization
	//--------------------------------------------------------------------------------------
	//UnloadShader(stylus_shader_first_pass);
	//UnloadTexture(texture);       // Texture unloading

	CloseWindow();                // Close window and OpenGL context
	//--------------------------------------------------------------------------------------

    return 0;
}