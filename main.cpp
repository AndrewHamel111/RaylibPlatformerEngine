#include "raylib.h"
#define NEARBLACK CLITERAL(Color){ 20, 20, 20, 255}
#define MUSTARD CLITERAL(Color){ 203, 182, 51, 255}

#include "constants.h"

#include "environment.h"
#include "player.h"

int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
    
	/////////////////////////////////////////////////////////////
    // TODO: Load resources / Initialize variables at this point 
    /////////////////////////////////////////////////////////////
	
	//{ INIT PLAYER
	player p1(Vector2{screenWidth/2, screenWidth/2});
	//}
	
	//{ init env_list (truly init per level)
	env_list environment{0};
	//}
	
	/// set fps of window
    SetTargetFPS(TARGET_FPS);
	
	/// remove exit key functionality (alt-f4 will always work)
	SetExitKey(-1);
	
	/// Create the camera object
	Camera2D camera{Vector2{0,0}, Vector2{0,0}, 0, 1.0f};
	//camera.offset = Vector2{screenWidth/2, screenHeight/2 - 50};
	
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update variables / Implement example logic at this point
        //----------------------------------------------------------------------------------

		p1.update(&environment);
		
		//{ CAMERA UPDATE
        camera.zoom += ((float)GetMouseWheelMove()*0.05f);
        
        if (camera.zoom > 3.0f) camera.zoom = 3.0f;
        else if (camera.zoom < 0.25f) camera.zoom = 0.25f;
        
        if (IsKeyPressed(KEY_R)) 
        {
            camera.zoom = 1.0f;
        }
		//}
		
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            BeginMode2D(camera);

				// TODO: Draw everything that requires to be drawn at this point:
				
				DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);  // Example
				
				/// draw player
				p1.DrawPlayer();
				
			EndMode2D();

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    
    // TODO: Unload all loaded resources at this point
    
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}