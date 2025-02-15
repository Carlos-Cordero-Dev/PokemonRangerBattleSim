
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

#define GLSL_VERSION 430

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
	Shader stylus_shader = LoadShader(0/*null so no vs*/, absolute_shader_path.c_str());

	int stylusTexLocation = GetShaderLocation(stylus_shader, "tailTexture_in");

	// =======================
	//int startLocation = GetShaderLocation(stylus_shader, "start");
	//int endLocation = GetShaderLocation(stylus_shader, "end");
	//int topLeftLocation = GetShaderLocation(stylus_shader, "topLeft");
	//int topRightLocation = GetShaderLocation(stylus_shader, "topRight");
	//int botLeftLocation = GetShaderLocation(stylus_shader, "botLeft");
	//int botRightLocation = GetShaderLocation(stylus_shader, "botRight");

	//pre allocate max points
	TopPointData* topPointData = (TopPointData*)calloc(MAX_TOP_POINTS, sizeof(TopPointData));

	unsigned int ssbo = rlLoadShaderBuffer(sizeof(TopPointData) * MAX_TOP_POINTS, NULL, RL_STREAM_DRAW);

	rlBindShaderBuffer(ssbo, 1);

	//======================================
	int nodeCountLocation = GetShaderLocation(stylus_shader, "node_count");

	int stylusTexUnitPosition = 0;
	SetShaderValue(stylus_shader, stylusTexLocation, &stylusTexUnitPosition, RL_SHADER_UNIFORM_SAMPLER2D);

	unsigned int main_framebuffer = rlLoadFramebuffer();
	unsigned int stylus_framebuffer = rlLoadFramebuffer();

	rlEnableFramebuffer(main_framebuffer);
	rlEnableFramebuffer(stylus_framebuffer);
	unsigned int main_texture = rlLoadTexture(NULL, screenWidth, screenHeight, RL_PIXELFORMAT_UNCOMPRESSED_R16G16B16, 1);
	unsigned int stylus_texture = rlLoadTexture(NULL, screenWidth, screenHeight, RL_PIXELFORMAT_UNCOMPRESSED_R16G16B16, 1);
    rlActiveDrawBuffers(2);
	rlFramebufferAttach(main_framebuffer, main_texture, RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_TEXTURE2D, 0);
	rlFramebufferAttach(stylus_framebuffer, stylus_texture, RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_TEXTURE2D, 0);

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


		// Draw
		//----------------------------------------------------------------------------------
		BeginDrawing();



		// === STYLUS TEXTURE === 

		rlEnableFramebuffer(stylus_framebuffer);
		rlClearColor(0, 1, 0, 1);
		rlClearScreenBuffers();
		rlDisableColorBlend();
		rlEnableShader(stylus_shader.id);

		if (touch.x == 0 && touch.y == 0)
		{
		}
		else 
		{

			int numberOfNodes = GetStackDepth(top.stack);


			printf("num of nodes: %d\n", numberOfNodes);

			if (numberOfNodes > 2)
			{
				Coord* current = top.stack;
				Coord* next = top.stack->nextCoord;

				for (int i = 0; i < numberOfNodes - 1; i++)
				{

					topPointData[i].start[0] = current->x;
					topPointData[i].start[1] = current->y;

					topPointData[i].end[0] = next->x;
					topPointData[i].end[1] = next->y;

					current = next;
					next = next->nextCoord;
				}

				PopulateTopPoints(topPointData, numberOfNodes);
				
				rlUpdateShaderBuffer(ssbo, topPointData, numberOfNodes * sizeof(TopPointData), 0);
				rlBindShaderBuffer(ssbo, 1);

			}

			SetShaderValue(stylus_shader, nodeCountLocation, &numberOfNodes, SHADER_UNIFORM_INT);
		}


		BeginShaderMode(stylus_shader);

			//

			rlActiveTextureSlot(stylusTexUnitPosition);
			rlEnableTexture(stylus_texture);

			//

			//rlActiveTextureSlot(texUnitPosition);
			//rlEnableTexture(gBuffer.positionTexture);
			DrawRectangle(0, 0, screenWidth, screenHeight, WHITE);
			//rlLoadDrawQuad();


			EndShaderMode();

			rlEnableColorBlend();

			rlDisableFramebuffer();

			// =======================

			// === MAIN TEXTURE ===

			rlEnableFramebuffer(main_framebuffer);

            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
            
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
			rlClearScreenBuffers();

		EndDrawing();


        //----------------------------------------------------------------------------------
		//wait or end of frame
		//timer.FrameSleep();
    }

	ResetTop(&top);

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadShader(stylus_shader);
    UnloadTexture(texture);       // Texture unloading

    CloseWindow();                // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}