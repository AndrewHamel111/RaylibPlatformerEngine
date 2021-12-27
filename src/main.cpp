/// TODO
/* 
** - don't worry too much about optimizing the number of player fields as there is only one instance a little bit of sloppy code is okay. The game's optimization will come from optimizing env_objects and env_level rendering / collision checking / etc.
** - trace entire SRC folder and note all changes. Do not attempt to fix, document at the top of each file and then add a master list to Project TODOs

** - implement the following missing features:
** - stairs and slopes
** - make spikes work
** - create a RETRY screen
** - keys and doors
** - level loading system
** - main menu
** - level select
** - some decent levels
** - 
** - Improvements to hamlib finished:
** - working animation system
** - well-round the centered and scaled draw calls
** - create a Sprite object that contains a reference to an atlas and source rectangle
** - 
** - Consider the following (2.0 Ideas):
** - some sort of 2D mesh system, to enable slopes and less flat geometry
*/

#include "raylib.h"
#define NEARBLACK CLITERAL(Color){ 20, 20, 20, 255}
#define MUSTARD CLITERAL(Color){ 203, 182, 51, 255}

//#define DEV_SHOW_MOUSE_POS

#include "constants.h"

#include "environment.h"
#include "player.h"

#include "files.h"

// DEV SHIT
#include <iostream>

/// Sound and Texture Declarations
Sound coinSound;

void UpdateLevel(env_level* level);
void ResetLevel(player*, env_level);

int main()
{
	// Initialization
	//--------------------------------------------------------------------------------------
	const int screenWidth = 800;
	const int screenHeight = 600;

	player p1(Vector2{0,0});
	p1.setHitboxSize(Vector2{50,50});

/** Load Levels */
#ifdef DEV_LEVEL_TEST
	std::string window_label = "Level Tester";
#else
	std::string window_label = "Platformer Engine";
#endif // DEV_LEVEL_TEST

	InitWindow(screenWidth, screenHeight, window_label.c_str());
	InitAudioDevice();
	SetMasterVolume(0.5f);

	/////////////////////////////////////////////////////////////
	// TODO: Load resources / Initialize variables at this point
	/////////////////////////////////////////////////////////////

	/** TEXTURES
	=============*/

	Texture2D tex_terrain_bg = LoadTexture("resources/spr/terrain/bg.png");
	RectSet rect_terrain_bg; /**< Mapping from string names to source rects (BACKGROUND). */
	rect_terrain_bg.emplace("flag", Rectangle{0,0,50,50});

	Texture2D tex_terrain_fg = LoadTexture("resources/spr/terrain/fg.png");
	RectSet rect_terrain_fg; /**< Mapping from string names to source rects (FOREGROUND). */

	/** LEVELS
	=============*/
#ifdef DEV_LEVEL_TEST

	bool levelIsLoaded = false;
	std::string file_name;
	env_level level;

#else

	std::vector<env_level> levels;
	unsigned short level = 0; /**< acts as an index to std::vector<> levels. level 0 is an interactive menu, level 1 marks the first level in the game. */
	// load all levels in the levels folder
	try
	{
		levels = LoadLevelsFromFolder("levels");
	}
	catch (JSONLevelException except)
	{
		/// TODO handle the case where there is an invalid level in levels/
		exit(0);
	}
	catch (std::string msg)
	{
		/// TODO handle the case where the folder has no files in it whatsoever
		exit(0);
	}
	ResetLevel(&p1, levels[level]);

	// add the atlases (default atlases as placeholder)
	auto i = levels.begin();
	auto iEnd = levels.end();
	while(i != iEnd)
	{
		i->foregroundAtlas = tex_terrain_fg;
		i->foregroundRect = rect_terrain_fg;
		i->backgroundAtlas = tex_terrain_bg;
		i->backgroundRect = rect_terrain_bg;

		i++;
	}

#endif // DEV_LEVEL_TEST

	/** SOUNDS
	=============*/

	coinSound = LoadSound("resources/snd/coin.ogg");


	/** SETUP
	=============*/

	/// set fps of window
	SetTargetFPS(TARGET_FPS);

	/// remove exit key functionality (alt-f4 will always work)
	SetExitKey(-1);


	/** ENVIRONMENT
	=============*/

	/// Create the camera object
	Camera2D camera{Vector2{0,0}, Vector2{0,0}, 0, 1.0f};
	//camera.offset = Vector2{screenWidth/2, screenHeight/2 - 50};

	//--------------------------------------------------------------------------------------

	// Main game loop
	while (!WindowShouldClose())    // Detect window close button or ESC key
	{
		/// TEMPORARY FLAGS
		bool flag_drawCoinCount = false;

		// Update
		//----------------------------------------------------------------------------------
		// TODO: Update variables
		//----------------------------------------------------------------------------------

#ifdef DEV_LEVEL_TEST
		/// kill player if needed
		if (p1.isDead)
			ResetLevel(&p1, level);

		/// check for win condition
		if (p1.inEndZone && p1.coins >= level.coinsRequired)
		{
				// FOR NOW IT ENDS GAME
				CloseAudioDevice();
				CloseWindow();
				return 0;
			}
		else if (p1.inEndZone && !(p1.coins >= level.coinsRequired)) // show the player's progress
		{
			flag_drawCoinCount = true;
		}

		if (levelIsLoaded)
		{
			p1.update(&level.env_objects);
		}

#else
		/// kill player if needed
		if (p1.isDead)
			ResetLevel(&p1, levels[level]);

		p1.update(&levels[level].env_objects);
#endif

		/// CAMERA UPDATE
		camera.zoom += ((float)GetMouseWheelMove()*0.1f);
		camera.target = p1.pos + Vector2{0, p1.hitboxSize.y / 2};
		camera.offset = Vector2{400, 300 + p1.hitboxSize.y / 2};

		if (camera.zoom > 3.0f) camera.zoom = 3.0f;
		else if (camera.zoom < 0.25f) camera.zoom = 0.25f;

		if (IsKeyPressed(KEY_R))
		{
			camera.zoom = 1.0f;

#ifdef DEV_LEVEL_TEST
			level = LoadLevelFromFile(file_name);

			// add the atlases (DEV)
			level.foregroundAtlas = tex_terrain_fg;
			level.foregroundRect = rect_terrain_fg;
			level.backgroundAtlas = tex_terrain_bg;
			level.backgroundRect = rect_terrain_bg;

			ResetLevel(&p1, level);
			p1.coins = 0;
#endif // DEV_TEST_LEVEL
		}

		/// LEVEL UPDATE
#ifdef DEV_LEVEL_TEST
		if (IsKeyPressed(KEY_ESCAPE))
		{
			levelIsLoaded = false;
			file_name = "";
			SetWindowTitle("Level Tester");
		}
		UpdateLevel(&level);
		if (IsFileDropped())
		{
			int count;
			char** files = GetDroppedFiles(&count);
			if (count > 0)
				file_name = files[0];

			level = LoadLevelFromFile(file_name);

			// add the atlases (DEV)
			level.foregroundAtlas = tex_terrain_fg;
			level.foregroundRect = rect_terrain_fg;
			level.backgroundAtlas = tex_terrain_bg;
			level.backgroundRect = rect_terrain_bg;

			ClearDroppedFiles();
			SetWindowTitle(GetFileName(file_name.c_str()));
			ResetLevel(&p1, level);
			levelIsLoaded = true;
		}
#else
		UpdateLevel(&levels[level]);
#endif



		// Draw
		//----------------------------------------------------------------------------------
		BeginDrawing();

		ClearBackground(RAYWHITE);

#ifdef DEV_LEVEL_TEST
		if (!levelIsLoaded)
		{
			//DrawTextRec(GetFontDefault(), "DRAG AND DROP A LEVEL ON THIS WINDOW TO LOAD IT", Rectangle{200, 150, 400, 400}, 30, 1.0, true, LIGHTGRAY);
			DrawText("DRAG AND DROP A LEVEL ON THIS WINDOW TO LOAD IT", 200, 200, 30, LIGHTGRAY);

			EndDrawing();
			continue;
		}
#endif
		DrawText(TextFormat("player pos: %3.1f %3.1f", p1.pos.x, p1.pos.y), 10, 10, 20, BLACK);
		DrawText(TextFormat("player vel: %3.1f %3.1f", p1.vel.x, p1.vel.y), 10, 30, 20, BLACK);
		BeginMode2D(camera);

#ifdef DEV_LEVEL_TEST
		DrawLevel(level);
#else
		DrawLevel(levels[level]);
#endif

		/// draw player
		p1.DrawPlayer();

		EndMode2D();

		if (flag_drawCoinCount)
		{
			char text[30];
#ifdef DEV_LEVEL_TEST
			//auto text = ;
			strcpy(text, TextFormat("%d / %d COINS COLLECTED", (int)p1.coins, (int)level.coinsRequired));
#else
			//auto text = TextFormat("%d / %d COINS COLLECTED", (int)p1.coins, (int)levels[level].coinsRequired);
			strcpy(text, TextFormat("%d / %d COINS COLLECTED", (int)p1.coins, (int)levels[level].coinsRequired));
#endif
			int off = MeasureText(text, 30);
			DrawRectangle(screenWidth/2 - off/2 - 10, 40, off + 20, 50, Color{0,0,0,140});
			DrawText(text, screenWidth/2 - off/2, 50, 30, RAYWHITE);
		}

#ifdef DEV_SHOW_MOUSE_POS
		Vector2 v = GetScreenToWorld2D(GetMousePosition(), camera);
		DrawText(TextFormat("x=%4.2f\ny=%4.2f", v.x, v.y), GetMouseX(), GetMouseY() + 20, 20, NEARBLACK);
		DrawText(TextFormat("x=%3.2f\ny=%3.2f",p1.vel.x, p1.vel.y),0,0, 40, BLACK);
		//DrawText(TextFormat("x=%3.2f\ny=%3.2f",p1.externAcc.x, p1.externAcc.y),0,0, 40, BLACK);
#endif

		EndDrawing();
		//----------------------------------------------------------------------------------

	}

	// De-Initialization
	//--------------------------------------------------------------------------------------

	// TODO: Unload all loaded resources at this point

	CloseAudioDevice();
	CloseWindow();        // Close window and OpenGL context
	//--------------------------------------------------------------------------------------

	return 0;
}

void UpdateLevel(env_level* level)
{
	auto i = level->env_objects.begin();
	for(; i != level->env_objects.end(); i++)
	{
		if (i->func != STATIC)
		{
			// do some update on that object
		}
		if (i->type == COIN && i->isCollected)
		{
			level->env_objects.erase(i);
			PlaySound(coinSound);
		}
		// since erasing changes the def'n of end()
		if (i == level->env_objects.end()) return;
	}
}

void ResetLevel(player* p, env_level level)
{
	p->acc = p->vel = Vector2{0,0};
	p->pos = level.player_start;
}
