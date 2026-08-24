
#include <stdlib.h>

// debug
#ifdef DEBUG
#ifdef SWITCH_BUILD
#include <crtdbg.h> //memory leaks check
#endif
#ifdef WINDOWS_BUILD
#include "logging_manager.h"
#endif
#endif

// release
// TOOD: no existe RELEASE tag ahora mismo
#define RAYGUI_IMPLEMENTATION
#ifdef SWITCH_BUILD
#define PLATFORM_NX 1
#include <switch.h>
#include "raygui.h"	
#endif
#ifdef WINDOWS_BUILD
#include "fix_win32_compatibility.h"
#include "raygui_win.h"
#endif

#include "raylib.h"
#include "raymath.h"

//#define GRAPHICS_API_OPENGL_33
#include "rlgl.h" //rlFramebuffer
//#include "glad.h" //glBindBuffer
//#include <glad.h>

//#define GLSL_VERSION 420

#include "constants.h"
#include "animation_database.h"
//#include "geometry_shader_support.h"
#include "backdrop.h"
#include "sprites.h"
#include "timer.h"
#include "controls.h"
#include "FIFO.h"
#include "player_polygon.h"
#include "ranger_top.h"

#include "world.h"
#include "world_object.h"
#include "hitbox.h"
#include "pokemon.h"

#include "state_machine.h"
//#include "states/idle_state.h"
//#include "states/move_state.h"
//#include "states/attack_state.h"

// NEW STATE MACHINE
#include "actions/action_factory.h"

#include "actions/move_towards_target_action.h"
#include "actions/set_target_action.h"
#include "actions/start_timer_action.h"
#include "actions/tick_timer_action.h"
#include "actions/spawn_hitbox_action.h"
#include "actions/set_animation_action.h"
#include "actions/spawn_hazard_object_action.h"
#include "actions/spawn_projectile_object_action.h"

#include "conditions/condition_factory.h"

#include "conditions/distance_condition.h"
#include "conditions/timer_finished_condition.h"
#include "conditions/animation_finished_condition.h"

#include "attack_effect/attack_effect_factory.h"
#include "attack_effect/hazard_object_attack_effect.h"
#include "attack_effect/projectile_object_attack_effect.h"
#include "attack_effect/attack_effect_database.h"
/*
what do I need man:


-touchscreen support [x]
-importing sprites + anims [x]
-spiny thingy [x]

AI:
- movement
- atacks: single hitboxes


-special player atacks
-terrains?

-menu
-level selector

MECHANIC IDEAS:
-gestures: press a button (maybe time slows down, screen goes gray) 
and during a duration can make shape which triggers buff etc

-pokemon summon obstacles you have to catch
-multiple pokemon spawned at once like flocks of pidgeys
-one pokemon cries for help many appear
-collecting instruments or pokemon with instruments (pichu ukelele starting, you find more in a potential game)
- ^ alternatively, the goal of the game is collecting instruments, starting ukelele, rework how ukelele works
-powerup: draw one circle and another one is drawn either to the side or one bigger is drawn on top

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
	srand(time(0));
	
	int screenWidth = 1280;
    int screenHeight = 720;
	int renderWidth = screenWidth;
	int renderHeight = screenHeight;

    char textBoxText[64] = "Text box";
    bool textBoxEditMode = false;


    InitWindow(screenWidth, screenHeight, "raylib [textures] example - texture loading and drawing");

#ifdef WINDOWS_BUILD
	HWND windowHandle = reinterpret_cast<HWND>(GetWindowHandle());

	LONG_PTR style = GetWindowLongPtr(windowHandle, GWL_STYLE);

	style &= ~WS_THICKFRAME;       // Prevent border/corner drag resizing
	style |= WS_MAXIMIZEBOX;       // Enable the native maximize button

	SetWindowLongPtr(windowHandle, GWL_STYLE, style);

	// Tell Windows to redraw the title bar using the changed style.
	SetWindowPos(
		windowHandle,
		nullptr,
		0,
		0,
		0,
		0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED
	);
#endif

	printf("\n=====================damnson1=================================\n");
	//fflush(stdout);
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

	//unsigned int stylus_framebuffer = rlLoadFramebuffer();
	//unsigned int main_framebuffer = rlLoadFramebuffer();

	//rlEnableFramebuffer(main_framebuffer);
	//rlEnableFramebuffer(stylus_framebuffer);
	//unsigned int main_texture = rlLoadTexture(NULL, screenWidth, screenHeight, RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1);
	//unsigned int stylus_texture = rlLoadTexture(NULL, screenWidth, screenHeight, RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8/*important for transparency*/, 1);

 //   //rlActiveDrawBuffers(2);
	//rlFramebufferAttach(main_framebuffer, main_texture, RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_TEXTURE2D, 0);
	//rlFramebufferAttach(stylus_framebuffer, stylus_texture, RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_TEXTURE2D, 0);

	RenderTexture2D mainTexOverlayRenTex = LoadRenderTexture(screenWidth, screenHeight);
	RenderTexture2D UITexOverlayRenTex = LoadRenderTexture(screenWidth, screenHeight);
	RenderTexture2D stylusOverlayRenTex = LoadRenderTexture(screenWidth, screenHeight);

	//texture of stylus shader in Texture2D version to be able to do DrawTexture
	//Texture2D stylusOverlay;
	//stylusOverlay.id = stylus_texture;
	//stylusOverlay.width = screenWidth;
	//stylusOverlay.height = screenHeight;
	//stylusOverlay.mipmaps = 1;
	//stylusOverlay.format = RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;

	//Texture2D mainTexOverlay;
	//mainTexOverlay.id = main_texture;
	//mainTexOverlay.width = screenWidth;
	//mainTexOverlay.height = screenHeight;
	//mainTexOverlay.mipmaps = 1;
	//mainTexOverlay.format = RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;


	// Make sure our framebuffer is complete.
    // NOTE: rlFramebufferComplete() automatically unbinds the framebuffer, so we don't have
    // to rlDisableFramebuffer() here.
//#ifndef SWITCH_BUILD
	//if (!rlFramebufferComplete(main_framebuffer))
	//{
	//	printf("Framebuffer is not complete");
	//	CloseWindow();
	//}
	//if (!rlFramebufferComplete(stylus_framebuffer))
	//{
	//	printf("Framebuffer is not complete");
	//	CloseWindow();
	//}
//#endif


    // NOTE: Textures MUST be loaded after Window initialization (OpenGL context is required) 

	Backdrop backdrop;
	backdrop.Load(RESOURCES_FOLDER + std::string("sprites/backdrop.png"));

	AnimationDatabase& animDatabase = AnimationDatabase::Instance();

	//TODO: if we assume we are doing it with the todo below: remove the random ints and just take path and custom name, cause you know its
	// 1 animation in the folder and just count the files

	animDatabase.LoadAnimDataFromFolder("sprites/garchomp/attack/left_down", "garchomp_attack_left_down");
	animDatabase.LoadAnimDataFromFolder("sprites/garchomp/attack/left_up", "garchomp_attack_left_up");
	animDatabase.LoadAnimDataFromFolder("sprites/garchomp/attack/right_down", "garchomp_attack_right_down");
	animDatabase.LoadAnimDataFromFolder("sprites/garchomp/attack/right_up", "garchomp_attack_right_up");

	animDatabase.LoadAnimDataFromFolder("sprites/garchomp/run", "garchomp_run");
	animDatabase.LoadAnimDataFromFolder("sprites/garchomp/idle","garchomp_idle");

	animDatabase.LoadAnimDataFromFolder("sprites/stylus/top", "top_idle_spin");

	//TODO: A bit lengthy, you have to make it so animations can only be in one folder so instead of all 4 attack anim in the same folder
	// there must be 1 folder per anim and so when you call GetAnimationDataFromName make it match to the one at LoadAnimDataFromFolder you gave it.
	// prkf files should be fine cause they support single anims

	std::vector<SpriteAnimation*> garchompAnims;
	garchompAnims.emplace_back(new SpriteAnimation{ animDatabase.GetAnimationDataFromName("garchomp_idle") }); //0 = idle
	garchompAnims.emplace_back(new SpriteAnimation{ animDatabase.GetAnimationDataFromName("garchomp_run") }); // 1 = move
	garchompAnims.emplace_back(new SpriteAnimation{ animDatabase.GetAnimationDataFromName("garchomp_attack_left_down") }); // 2 = attack

	std::vector<SpriteAnimation*> stylusAnims;
	stylusAnims.emplace_back(new SpriteAnimation{ animDatabase.GetAnimationDataFromName("top_idle_spin") });


    printf("\ndamnson2=================================\n");
	//---------------------------------------------------------------------------------------
	// Main game loop
	GameManager& gameManager = GameManager::GetInstance();

	std::string text = "Frame ";
	SetTargetFPS(500);

	Top top;
	Coord* enclosedPoly = nullptr;
	std::vector<Vector2> enclosedAuxPoints;
	std::vector<Vector2> enclosedIndicatorParticlePositions; //initial positions
	std::vector<Vector2> lerpingEIParticlePositions; //during lerping positions
	Vector2 enclosedTrackingPosition;

	//state machines
	 
	//	//default pokemon state machie
	//StateMachine* pokemondefaultStateMachine = new StateMachine();
	//{//scope cause im like that (no reason whatsoever)
	//	//TODO: this should probably be a function but tbh this WILL be an external application passing in the FSM
	//	IdleState* idleState = new IdleState();
	//	MoveState* moveState = new MoveState();
	//	AttackState* attackState = new AttackState();
	//	std::vector<State*> pdsm_ownedStates = { idleState , moveState, attackState };

	//	idleState->on_idle_timer_finished = moveState;

	//	moveState->on_move_action_completed = attackState;

	//	attackState->on_attack_completed = idleState;

	//	pokemondefaultStateMachine->currentState = idleState;
	//	pokemondefaultStateMachine->ownedStates = pdsm_ownedStates;

	//}

	// READ SM FROM JSON 
	auto& conditionFactory = ConditionFactory::Instance();

	conditionFactory.Register("DistanceToTargetLess", [](const Json& j) {
		auto c = std::make_unique<DistanceToTargetLess>();
		c->value = j["value"];
		return c;
		});

	conditionFactory.Register("TimerFinished", [](const Json& j) {
		auto c = std::make_unique<TimerFinishedCondition>();
		c->name = j["name"];
		return c;
		});

	conditionFactory.Register("AnimationFinished", [](const Json& j) {
			auto c = std::make_unique<AnimationFinishedCondition>();
			//c->name = j["name"];
			return c;
		});

	auto& actionFactory = ActionFactory::Instance();

	actionFactory.Register("MoveTowardsTarget", [](const Json& j) {
		auto a = std::make_unique<MoveTowardsTargetAction>();
		a->speed = ParseFloatValue(j["speed"]);
		return a;
		});

	actionFactory.Register("SetTarget", [](const Json& j) {
		auto a = std::make_unique<SetTargetAction>();

		a->targetX = ParseFloatValue(j["x"]);
		a->targetY = ParseFloatValue(j["y"]);

		return a;
		});

	actionFactory.Register("SpawnHitbox", [](const Json& j) {
		auto a = std::make_unique<SpawnHitboxAction>();

		a->width = ParseFloatValue(j["width"]);
		a->height = ParseFloatValue(j["height"]);
		a->speed = ParseFloatValue(j["speed"]);
		a->dirX = ParseFloatValue(j["dirX"]);
		a->dirY = ParseFloatValue(j["dirY"]);
		a->delaySec = ParseFloatValue(j["delaySec"]);

		return a;
		});

	actionFactory.Register("StartTimer", [](const Json& j) {
		auto a = std::make_unique<StartTimerAction>();
		a->name = j["name"];
		a->duration = j["duration"];
		return a;
		});

	actionFactory.Register("TickTimer", [](const Json& j) {
		auto a = std::make_unique<TickTimerAction>();
		a->name = j["name"];
		return a;
		});

	actionFactory.Register("SetAnimation", [](const Json& j) {
		auto a = std::make_unique<SetAnimationAction>();
		a->animationName = j["animation"];
		return a;
		});

	actionFactory.Register("SpawnHazardObject", [](const Json& j) {
		auto a = std::make_unique<SpawnHazardObjectAction>();
		a->hazardObjectType = j["effect"];
		a->offsetX = ParseFloatValue(j["offsetX"]);
		a->offsetY = ParseFloatValue(j["offsetY"]);
		a->spawnDelaySec = ParseFloatValue(j["spawnDelaySec"]);
		return a;
		});

	actionFactory.Register("SpawnProjectileObject", [](const Json& j) {
		auto a = std::make_unique<SpawnProjectileObjectAction>();
		a->projectileObjectType = j["effect"];
		a->offsetX = ParseFloatValue(j["offsetX"]);
		a->offsetY = ParseFloatValue(j["offsetY"]);
		a->dirX = ParseFloatValue(j["dirX"]);
		a->dirY = ParseFloatValue(j["dirY"]);
		a->speed = ParseFloatValue(j["speed"]);
		a->spawnDelaySec = ParseFloatValue(j["spawnDelaySec"]);
		return a;
		});

	auto& attackEffectFactory = AttackEffectFactory::Instance();

	attackEffectFactory.Register("HazardObject", [](const Json& j) {
		auto effect = std::make_unique<HazardObjectAttackEffect>();
		effect->visualAnimation = j["visualAnimation"];
		effect->width = j.value("width", 0.0f);
		effect->height = j.value("height", 0.0f);
		effect->lifetimeSec = j.value("lifetimeSec", 0.0f);
		return effect;
		});

	attackEffectFactory.Register("ProjectileObject", [](const Json& j) {
		auto effect = std::make_unique<ProjectileObjectAttackEffect>();
		effect->visualAnimation = j["visualAnimation"];
		effect->width = j.value("width", 0.0f);
		effect->height = j.value("height", 0.0f);
		effect->lifetimeSec = j.value("lifetimeSec", 0.0f);
		return effect;
		});

	AttackEffectDatabase::Instance().LoadFromFile(RESOURCES_FOLDER + std::string("attack_effects/hazard_object_af.json"));
	AttackEffectDatabase::Instance().LoadFromFile(RESOURCES_FOLDER + std::string("attack_effects/projectile_object_af.json"));

	Json j = LoadJson(RESOURCES_FOLDER + std::string("state_machines/sm.json"));
	//TODO: either leave the owner here or in AssignMachine state, but not in both
	StateMachine* sm = BuildStateMachine(j, nullptr/*owner*/);
	//world object creation


	// === AUDIO LOADING ===
	// Note: Audio must be loaded after ever other external resource
	InitAudioDevice();



	// =====================

	WorldObject* wo = new WorldObject(garchompAnims);
	wo->position = Vector2({ 100, 100 });

	WorldObject* topObject = new WorldObject(stylusAnims);
	topObject->position = Vector2({ -100, -100 }); //offscreen
	topObject->scale = 3.0f;

	Pokemon* p = new Pokemon(garchompAnims);
	p->position = Vector2({ 500, 100 });
	p->AssignStateMachine(sm);

	//TODO: below tasks to be automated so basically find a way to register anyway object created, it can be a function 
	// ex: register(pokemon) and it tries to cast to each type of object dynamic_cast<EnclosedObject*> for example and if != nullptr
	// it will add to allEnclosableObjs

	//std::vector<EnclosableObject*> allEnclosableObjs;
	gameManager.allEnclosableObjs.emplace_back(p);

	//std::vector<WorldObject*> allWorldObjs;
	gameManager.allWorldObjs.emplace_back(wo);
	gameManager.allWorldObjs.emplace_back(p);


	Timer& timer = Timer::GetInstance();
	timer.SetTimeScale(1.0f);

	float yellowTransitionElapsedTime = 0.0f;
	const float KEnclosingParticlesTravelTimeFromOutToCenterSec = 0.3f;
	float enclosingParticlesElapsedTime = 0.0f;

	while (!WindowShouldClose())    // Detect window close button or ESC key
	{

		//input code should be decoupled from render 60fps limit

		// ---------------------------------------------------------------------------------
		// Update
		PollInputEvents();

		screenWidth = GetScreenWidth();
		screenHeight = GetScreenHeight();

		timer.Update();


		const float outputWidth = static_cast<float>(screenWidth);
		const float outputHeight = static_cast<float>(screenHeight);

		const float scale = std::min<float>(
			outputWidth / static_cast<float>(renderWidth),
			outputHeight / static_cast<float>(renderHeight)
		);

		const float viewportWidth = renderWidth * scale;
		const float viewportHeight = renderHeight * scale;
		const float viewportX = (outputWidth - viewportWidth) * 0.5f;
		const float viewportY = (outputHeight - viewportHeight) * 0.5f;

		// Convert physical mouse/touch coordinates to fixed 1920x1080 coordinates.
		prbs::Vector2 touch = GetTouch();
		touch.x = (touch.x - viewportX) / scale;
		touch.y = (touch.y - viewportY) / scale;

		if (touch.x == 0 && touch.y == 0)
		{
			if (top.stack != nullptr)
			{
				printf("\nA");
				ResetTop(&top);
				DestroyStackNoDepth(&enclosedPoly);
				enclosedAuxPoints.clear();
				//printf("\nReset");

				//TODO: propper disable just in case instead of offscreen
				topObject->position.x = -100;
				topObject->position.y = -100;

				printf("\nA end");
			}
			// reset damaged flag on touch up
			if (top.wasDamaged) top.wasDamaged = false;
		}
		else if(!top.wasDamaged)
		{
			InsertTopCoord(&top, touch.x, touch.y);

			topObject->position.x = touch.x;
			topObject->position.y = touch.y;
		}

		ComputeAndUpdateDistance(&top);
		ForceTopDistanceLimit(&top);

		// poly closed, calculate enclosed particles positions
		Coord* currEnclosedPoly = nullptr;
		if (checkTopIntersection(&top, gameManager.allEnclosableObjs, currEnclosedPoly, &enclosedTrackingPosition))
		{
			//printf("INTERSECTED\n");
			//ShowStack(enclosedPoly);
			//DestroyStackNoDepth(&enclosedPoly);
			printf("\nB");

			// prior frame existed enclosed poly > clear it, set it to new one
			if (enclosedPoly)
			{
				printf("\nB2");

				//yellow transition is always destroyed if another appears
				DestroyStackNoDepth(&enclosedPoly);
				enclosedAuxPoints.clear();
				printf("\nB2 end");

			}

			//reset enclosing particles
			enclosedIndicatorParticlePositions.clear();
			enclosingParticlesElapsedTime = 0.0f;

			enclosedPoly = currEnclosedPoly;
			CalculateEnclosedShaderAreaPoints(enclosedPoly, enclosedAuxPoints);
			CalculateClosingIndicatorParticlePoints(enclosedPoly, enclosedIndicatorParticlePositions);
			lerpingEIParticlePositions.resize(enclosedIndicatorParticlePositions.size());
			printf("\nB end");

			//NOTE: yellow transition is always destroyed if another appears
			// catch circles can stack, so circles have lifetime (particles)
		}


		//update points - time dependent
		yellowTransitionElapsedTime += timer.GetDeltaTime();
		int enclosedPointsCount = enclosedAuxPoints.size();
		 
		if (yellowTransitionElapsedTime > kYellowTransitionShrinkingFrequencySec)
		{
			yellowTransitionElapsedTime = 0.0f;

			for (int i = 0; i < enclosedPointsCount; i += 4)
			{
				// horizontal lines higher "thickness"
				// vertical lines little to no thickness

				//counter-clockwise
				Vector2 topLeft = enclosedAuxPoints[i + 0];
				Vector2 topRight = enclosedAuxPoints[i + 1];
				Vector2 botLeft = enclosedAuxPoints[i + 2];
				Vector2 botRight = enclosedAuxPoints[i + 3];
				//determine up direction
				//TODO: this has to go somewhere else and make it deltatime dependant and kYellowTransitionUpdateSpeed dependant
				Vector2 upDir = Vector2Normalize(Vector2Subtract(botLeft, topLeft));

				enclosedAuxPoints[i + 0].y += upDir.y * KYellowTransitionShrinkingFactor;
				enclosedAuxPoints[i + 1].y += upDir.y * KYellowTransitionShrinkingFactor;
				enclosedAuxPoints[i + 2].y -= upDir.y * KYellowTransitionShrinkingFactor;
				enclosedAuxPoints[i + 3].y -= upDir.y * KYellowTransitionShrinkingFactor;
			}
		}

		//enclosed particles (TODO: I think the timer always matches 1to1 with the yellow transiton, maybe combine them)

		if (enclosedIndicatorParticlePositions.size() > 0)
		{
			//continuous transition, same time spent regardless of distance

			enclosingParticlesElapsedTime += timer.GetDeltaTime();

			float interpFactor = enclosingParticlesElapsedTime / KEnclosingParticlesTravelTimeFromOutToCenterSec;

			Vector2 center = enclosedTrackingPosition;

			//move every particle
			for (int i = 0; i < enclosedIndicatorParticlePositions.size(); i++)
			{
				Vector2 currParticlePos = Vector2Add(enclosedIndicatorParticlePositions[i],
					Vector2Scale(Vector2Subtract(center, enclosedIndicatorParticlePositions[i]), interpFactor));

				lerpingEIParticlePositions[i] = currParticlePos;
			}

			//reset on lerp > 1
			if (interpFactor >= 1.0f)
			{
				enclosingParticlesElapsedTime = 0.0f;
				enclosedIndicatorParticlePositions.clear();
			}
		}

		// === CLEAR SINGLE FRAME HITBOXES ===
		// must be done before any update
		gameManager.singleFrameHitboxes.clear();

		//update every single world object
		topObject->Update();

		//wo->Update();
		//p->Update();

		//fix in case allWorldsObj array is modified mid iteration (afaik this is the only spot where it can be modified mid loop)
		for (int i = 0; i < gameManager.allWorldObjs.size(); ++i)
		{
			gameManager.allWorldObjs[i]->Update();
		}

		for (Hitbox* hitbox : gameManager.activeHitboxes)
		{
			hitbox->Update(timer.GetDeltaTime());
		}


		//check top collision with hitboxes
		for (Hitbox* hitbox : gameManager.activeHitboxes)
		{
			if (PolygonCollidingWithBox(top.stack, hitbox->boundingBox))
			{
				hitbox->OnCollision();

				//reset stylus
				ResetTop(&top);
				DestroyStackNoDepth(&enclosedPoly);
				enclosedAuxPoints.clear();
				topObject->position.x = -100;
				topObject->position.y = -100;

				top.wasDamaged = true;
				break;

			}
			//clear hitbox as it only exists this frame
			//TODO: okey maybe dont clear the hitbox bc what if the hitbox interacts with the environment
		}
		//check to collision with single frame hitboxes
		for (Hitbox* hitbox : gameManager.singleFrameHitboxes)
		{
			if (PolygonCollidingWithBox(top.stack, hitbox->boundingBox))
			{
				hitbox->OnCollision();

				//reset stylus
				ResetTop(&top);
				DestroyStackNoDepth(&enclosedPoly);
				enclosedAuxPoints.clear();
				topObject->position.x = -100;
				topObject->position.y = -100;

				top.wasDamaged = true;
				break;

			}
			//clear hitbox as it only exists this frame
			//TODO: okey maybe dont clear the hitbox bc what if the hitbox interacts with the environment
		}

		//check top collision with world objects
		for (WorldObject* wobj : gameManager.allWorldObjs)
		{
			if (PolygonCollidingWithBox(top.stack, wobj->boundingBox))
			{
				wobj->OnCollision();

				//reset stylus
				ResetTop(&top);
				DestroyStackNoDepth(&enclosedPoly);
				enclosedAuxPoints.clear();
				topObject->position.x = -100;
				topObject->position.y = -100;

				top.wasDamaged = true;
				break;
			}
		}



		// End of Update
		// ---------------------------------------------------------------------------------


		//----------------------------------------------------------------------------------
		// Draw

		
		BeginDrawing();

		rlDisableDepthTest();

		// === STYLUS TEXTURE === 

		BeginTextureMode(stylusOverlayRenTex);
		ClearBackground(BLANK);


		//DrawRectangle(0, 0, screenWidth, screenHeight, WHITE);

		rlSetBlendMode(BLEND_ALPHA);


		// yellow transition - (needs to be rendered even if player doesnt touch screen)

		//rlDisableBackfaceCulling(); //this does nothing, cool!
		if (!enclosedAuxPoints.empty())
		{
			for (int i = 0; i < enclosedPointsCount; i += 4)
			{
				// horizontal lines higher "thickness"
				// vertical lines little to no thickness

				//counter-clockwise
				Vector2 topLeft = enclosedAuxPoints[i + 0];
				Vector2 topRight = enclosedAuxPoints[i + 1];
				Vector2 botLeft = enclosedAuxPoints[i + 2];
				Vector2 botRight = enclosedAuxPoints[i + 3];

				DrawTriangle(topLeft, botLeft, topRight, YELLOW);
				DrawTriangle(topLeft, topRight, botLeft, YELLOW); //dupe

				DrawTriangle(topRight, botLeft, botRight, YELLOW);
				DrawTriangle(topRight, botRight, botLeft, YELLOW); //dupe

			}
		}
		//enclosed particles
		if (enclosedIndicatorParticlePositions.size() > 0)
		{
			for (Vector2 particlePos : lerpingEIParticlePositions)
			{
				DrawCircleV(particlePos,3.0f,RED);
			}
		}


		//draw stlyus tail
		int numberOfNodes = GetStackDepth(top.stack);

		if (touch.x != 0 && touch.y != 0 && numberOfNodes > 0)
		{

			//printf("num of nodes: %d\n", numberOfNodes);
			float trailThickness = 10.0f;

			Coord* current = top.stack;
			Coord* next = top.stack->nextCoord;


			// --- First pass: draw all circles ---

			SetShaderValue(stylus_circle_shader, trailThicknessCircleLoc, &trailThickness, SHADER_UNIFORM_FLOAT);

			for (int i = 0; i < numberOfNodes - 1; i++)
			{
				float startPos[2] = { current->x, current->y };
				// Set uniforms
				SetShaderValue(stylus_circle_shader, circleCenterLoc, startPos, SHADER_UNIFORM_VEC2);

				BeginShaderMode(stylus_circle_shader);
				DrawCircleV({ startPos[0], startPos[1] }, trailThickness, WHITE);
				EndShaderMode();

				current = next;
				next = next->nextCoord;
			}

			// --- Second pass: draw all lines ---
			current = top.stack;
			next = current->nextCoord;

			SetShaderValue(stylus_line_shader, trailThicknessStylusLoc, &trailThickness, SHADER_UNIFORM_FLOAT);

			for (int i = 0; i < numberOfNodes - 2; i++)
			{
				float startPos[2] = { current->x, current->y };
				float endPos[2] = { next->x, next->y };

				// Set uniforms
				SetShaderValue(stylus_line_shader, lineStartLoc, startPos, SHADER_UNIFORM_VEC2);
				SetShaderValue(stylus_line_shader, lineEndLoc, endPos, SHADER_UNIFORM_VEC2);
				BeginShaderMode(stylus_line_shader);
				DrawLineEx({ startPos[0], startPos[1] }, { endPos[0], endPos[1] }, trailThickness * 2.0f, WHITE);
				EndShaderMode();

				current = next;
				next = next->nextCoord;
			}


			//rlEnableBackfaceCulling();

			//TODO: update yellow transition
			// horizontal lines higher "thickness"
			// vertical lines little to no thickness
		}

		topObject->Draw();

		//DrawCurrentPolygonOnlyLines(top.stack);

		EndTextureMode();

		// =======================

		// === UI TEXTURE ===

		BeginTextureMode(UITexOverlayRenTex);
		ClearBackground(BLANK);

		//ui.draw()

		EndTextureMode();

		// === MAIN TEXTURE ===

		//rlEnableDepthTest();

		BeginTextureMode(mainTexOverlayRenTex);
		ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

		backdrop.DrawCenteredBackdrop(renderWidth, renderHeight);


		DrawText(("fps " + std::to_string(GetFPS())).c_str(), 360, 90, 40, GRAY);
		DrawText(("touch " + std::to_string(touch.x) + " " + std::to_string(touch.y)).c_str(), 360, 190, 40, GRAY);
		DrawText((text + std::to_string(timer.GetFrame())).c_str(), 360, 370, 40, GRAY);
		DrawText(("Time " + std::to_string(timer.GetGameTime()) + " deltaTime " + std::to_string(GetFrameTime())).c_str(),
			360, 230, 40, GRAY);

		
		if (GuiTextBox(Rectangle({ 25, 215, 125, 30 }), textBoxText, 64, textBoxEditMode)) textBoxEditMode = !textBoxEditMode;


		//draw backdrops


		//draw every world object

		for (WorldObject* wobj :gameManager.allWorldObjs)
		{
			wobj->Draw();
		}

		//debug draw hitboxes
		for (Hitbox* hitbox : GameManager::GetInstance().activeHitboxes)
		{
			hitbox->ShowHitbox();
		}
		for (Hitbox* hitbox : GameManager::GetInstance().singleFrameHitboxes)
		{
			hitbox->ShowHitbox();
		}

		EndTextureMode();

		// =====================

		// Draw all the created textures (order: background -> main -> UI)  
		// (flipped vertically to correct for upside-down rendering)

		Rectangle sourceRec = {
			0,
			0,
			static_cast<float>(renderWidth),
			static_cast<float>(-renderHeight)
		};

		Rectangle destRec = {
			viewportX,
			viewportY,
			viewportWidth,
			viewportHeight
		};
		Vector2 origin = { 0, 0 };

		DrawTexturePro(mainTexOverlayRenTex.texture, sourceRec, destRec, origin, 0.0f, WHITE);
		DrawTexturePro(stylusOverlayRenTex.texture, sourceRec, destRec, origin, 0.0f, WHITE);
		DrawTexturePro(UITexOverlayRenTex.texture, sourceRec, destRec, origin, 0.0f, WHITE);


		EndDrawing();
		//SwapScreenBuffer();
		


		//----------------------------------------------------------------------------------
		//wait or end of frame
		//timer.FrameSleep();
		//NOTE: EndDrawing() does this by default check custom swap functions
	}

	ResetTop(&top);

	// De-Initialization
	//--------------------------------------------------------------------------------------
	//UnloadShader(stylus_shader_first_pass);
	//UnloadTexture(texture);       // Texture unloading
	//UnloadSound();
	CloseAudioDevice();

	CloseWindow();                // Close window and OpenGL context
	//--------------------------------------------------------------------------------------

    return 0;
}