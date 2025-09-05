
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

	// === GRADIENT SHADERS ===

	// line

	std::string shader_path_vs = "shaders/gradient.vs";
	std::string absolute_shader_path_vs = RESOURCES_FOLDER + shader_path_vs;
	std::string shader_path_fs = "shaders/gradient_line.fs";
	std::string absolute_shader_path_fs = RESOURCES_FOLDER + shader_path_fs;

	Shader stylus_line_shader = LoadShader(absolute_shader_path_vs.c_str(), absolute_shader_path_fs.c_str());

	int trailThicknessStylusLoc = GetShaderLocation(stylus_line_shader, "trailThickness");
	int lineStartLoc = GetShaderLocation(stylus_line_shader, "lineStart");
	int lineEndLoc = GetShaderLocation(stylus_line_shader, "lineEnd");

	// circle

	shader_path_fs = "shaders/gradient_circle.fs";
	absolute_shader_path_fs = RESOURCES_FOLDER + shader_path_fs;

	Shader stylus_circle_shader = LoadShader(absolute_shader_path_vs.c_str(), absolute_shader_path_fs.c_str());

	int trailThicknessCircleLoc = GetShaderLocation(stylus_circle_shader, "trailThickness");
	int circleCenterLoc = GetShaderLocation(stylus_circle_shader, "circleCenter");

	// === === === 


	unsigned int stylus_framebuffer = rlLoadFramebuffer();
	unsigned int main_framebuffer = rlLoadFramebuffer();

	rlEnableFramebuffer(main_framebuffer);
	rlEnableFramebuffer(stylus_framebuffer);
	unsigned int main_texture = rlLoadTexture(NULL, screenWidth, screenHeight, RL_PIXELFORMAT_UNCOMPRESSED_R16G16B16, 1);
	unsigned int stylus_texture = rlLoadTexture(NULL, screenWidth, screenHeight, RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8/*important for transparency*/, 1);

    rlActiveDrawBuffers(2);
	rlFramebufferAttach(main_framebuffer, main_texture, RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_TEXTURE2D, 0);
	rlFramebufferAttach(stylus_framebuffer, stylus_texture, RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_TEXTURE2D, 0);

	RenderTexture2D mainTexOverlayRenTex = LoadRenderTexture(screenWidth, screenHeight);
	RenderTexture2D stylusOverlayRenTex = LoadRenderTexture(screenWidth, screenHeight);

	//texture of stylus shader in Texture2D version to be able to do DrawTexture
	Texture2D stylusOverlay;
	stylusOverlay.id = stylus_texture;
	stylusOverlay.width = screenWidth;
	stylusOverlay.height = screenHeight;
	stylusOverlay.mipmaps = 1;
	stylusOverlay.format = RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;

	Texture2D mainTexOverlay;
	mainTexOverlay.id = main_texture;
	mainTexOverlay.width = screenWidth;
	mainTexOverlay.height = screenHeight;
	mainTexOverlay.mipmaps = 1;
	mainTexOverlay.format = RL_PIXELFORMAT_UNCOMPRESSED_R16G16B16;


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

	SpriteAnimation* stylusAnim0 = new SpriteAnimation();
	stylusAnim0->advanceRate = 5;

	SpriteAnimation** stylusAnims = new SpriteAnimation * [1];
	stylusAnims[0] = stylusAnim0;

	loadTexturesFromFolder("sprites/stylus/top", stylusAnims, 1, 3, "top_idle_spin");

    printf("\ndamnson2=================================\n");
	//---------------------------------------------------------------------------------------
	// Main game loop

	std::string text = "Frame ";
	SetTargetFPS(60);

	Top top;

	WorldObject* wo = new WorldObject(garchompAnim0);
	wo->position = Vector2({ 100, 100 });

	WorldObject* topObject = new WorldObject(stylusAnim0);
	topObject->position = Vector2({ -100, -100 }); //offscreen
	topObject->scale = 5.0f;

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

		prbs::Vector2 touch = GetTouch();

		if (touch.x == 0 && touch.y == 0)
		{
			if (top.stack != nullptr)
			{
				ResetTop(&top);

				//TODO: propper disable just in case instead of offscreen
				topObject->position.x = -100;
				topObject->position.y = -100;
			}
		}
		else
		{
			InsertTopCoord(&top, touch.x, touch.y);

			topObject->position.x = touch.x;
			topObject->position.y = touch.y;
		}

		ComputeAndUpdateDistance(&top);
		ForceTopDistanceLimit(&top);

		if (checkSnakeIntersection(&top) == true)
		{
			//printf("INTERSECTED\n");
		}

		//update every single world object
		wo->Update();

		
		topObject->Update();

		// End of Update
		// ---------------------------------------------------------------------------------


		//----------------------------------------------------------------------------------
		// Draw

		BeginDrawing();

		//rlDisableDepthTest();

		//// === STYLUS TEXTURE === 

		//rlEnableFramebuffer(stylus_framebuffer);
		//rlClearScreenBuffers();
		//rlClearColor(0, 0, 0, 0); //transparent so the stylus overlay doesnt opaque the main texture

		BeginTextureMode(stylusOverlayRenTex);
		ClearBackground(BLANK);

		//DrawRectangle(0, 0, screenWidth, screenHeight, WHITE);

		rlSetBlendMode(BLEND_ALPHA);

		//draw stlyus tail

		if (touch.x != 0 && touch.y != 0)
		{

			int numberOfNodes = GetStackDepth(top.stack);

			//printf("num of nodes: %d\n", numberOfNodes);
			float trailThickness = 10.0f;

			Coord* current = top.stack;
			Coord* next = top.stack->nextCoord;


			// --- First pass: draw all circles ---
			rlSetBlendMode(BLEND_CUSTOM);     // enter custom mode
			rlSetBlendFactors(GL_ONE, GL_ZERO, GL_FUNC_ADD);

			SetShaderValue(stylus_circle_shader, trailThicknessCircleLoc, &trailThickness, SHADER_UNIFORM_FLOAT);

			for (int i = 0; i < numberOfNodes - 1; i++)
			{
				float startPos[2] = { current->x, current->y };
				// Set uniforms
				SetShaderValue(stylus_circle_shader, circleCenterLoc, startPos, SHADER_UNIFORM_VEC2);

				BeginShaderMode(stylus_circle_shader);
				DrawCircleV({ startPos[0], startPos[1] }, trailThickness, BLANK);
				EndShaderMode();

				current = next;
				next = next->nextCoord;
			}

			// --- Second pass: draw all lines ---
			current = top.stack;
			next = current->nextCoord;

			rlSetBlendMode(BLEND_ALPHA);

			SetShaderValue(stylus_line_shader, trailThicknessStylusLoc, &trailThickness, SHADER_UNIFORM_FLOAT);
			
			for (int i = 0; i < numberOfNodes - 2; i++)
			{
				float startPos[2] = { current->x, current->y };
				float endPos[2] = { next->x, next->y };

				// Set uniforms
				SetShaderValue(stylus_line_shader, lineStartLoc, startPos, SHADER_UNIFORM_VEC2);
				SetShaderValue(stylus_line_shader, lineEndLoc, endPos, SHADER_UNIFORM_VEC2);
				BeginShaderMode(stylus_line_shader);
				DrawLineEx({ startPos[0], startPos[1] }, { endPos[0], endPos[1] }, trailThickness * 2.0f, BLANK);
				EndShaderMode();

				current = next;
				next = next->nextCoord;
			}

		}

		topObject->Draw(frame);

		//rlDisableFramebuffer();
		EndTextureMode();


		// =======================

		// === MAIN TEXTURE ===

		//rlEnableFramebuffer(main_framebuffer);

		//ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

		BeginTextureMode(mainTexOverlayRenTex);
		ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

		//DrawCurrentPolygonOnlyLines(top.stack);



		DrawText(("fps " + std::to_string(GetFPS())).c_str(), 360, 90, 40, GRAY);
		DrawText(("touch " + std::to_string(touch.x) + " " + std::to_string(touch.y)).c_str(), 360, 190, 40, GRAY);
		DrawText((text + std::to_string(frame)).c_str(), 360, 370, 40, GRAY);
		DrawText(("Time " + std::to_string(currTime) + " deltaTime " + std::to_string(GetFrameTime())).c_str(),
			360, 230, 40, GRAY);

		//========== 

		if (GuiTextBox(Rectangle({ 25, 215, 125, 30 }), textBoxText, 64, textBoxEditMode)) textBoxEditMode = !textBoxEditMode;


		//draw every world object

		wo->Draw(frame);

		////=========================
		////timer related stuff

		frame++;
		currTime += GetFrameTime();

		//rlDisableFramebuffer();
		//rlClearScreenBuffers();
		EndTextureMode();

		// Draw the main texture (flipped vertically to correct for upside-down rendering)
		Rectangle sourceRec = { 0, 0, (float)screenWidth, (float)-screenHeight };
		Rectangle destRec = { 0, 0, (float)screenWidth, (float)screenHeight };
		Vector2 origin = { 0, 0 };

		DrawTexturePro(mainTexOverlayRenTex.texture, sourceRec, destRec, origin, 0.0f, WHITE);

		// Draw the stylus texture on top (also flipped)
		DrawTexturePro(stylusOverlayRenTex.texture, sourceRec, destRec, origin, 0.0f, WHITE);


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