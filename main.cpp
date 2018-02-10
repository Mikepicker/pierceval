// Using SDL and standard IO
#ifdef LINUX
#include <SDL2/SDL.h>
#elif WIN
#include <SDL.h>
#endif
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <stdlib.h> 
#include <time.h> 
#include <math.h>

#include "framework.cpp"

// Constants
const float TIME_STEP = 0.02f;
const double PI = 3.14159265;
const float CAMERA_SPEED = 1;

const Sint32 ENTITY_SIZE = 64;
const Sint32 ENTITY_ANIM_SPEED = 5000;
const float PLAYER_SPEED = 0.005f;
const Sint32 MAX_ARROWS = 100;
const Sint32 MAX_ENEMIES = 100;
const Sint32 TILE_SIZE = 64;
const Sint32 MAX_WALLS = 100;
const float ARROW_FLY_TIME = 1000;
const float GROUND_POS = SCREEN_HEIGHT - 64;

// Textures
SDL_Texture* archerTexture;
SDL_Texture* spearmanTexture;
SDL_Texture* arrowTexture;
SDL_Texture* ramTexture;

SDL_Texture* bg0Texture;
SDL_Texture* bg1Texture;
SDL_Texture* sunTexture;
SDL_Texture* groundTexture;

SDL_Texture* wallsTexture;
SDL_Texture* silhouetteTexture;

// Font
TTF_Font* font = NULL;

// Team
enum Team {
  BLUE = 0,
  RED = 1
};

enum Anim {
  ANIM_IDLE = 0,
  ANIM_MOVE = 1,
  ANIM_ATTACK = 2,
  ANIM_DIE = 3
};

enum State {
  STATE_IDLE = 0,
  STATE_MOVE = 1,
  STATE_ATTACK = 2,
  STATE_DIE = 3
};

// Camera
struct Camera {
  float x, y;
  float vx, vy;
  Sint32 w, h;
} camera;

// Game entities
struct Entity {
  float x, y;
  float vx, vy;
  Sint32 w, h;
  SDL_RendererFlip flip;
  State state;
  Sint32 frameX;
  Anim anim;
  SDL_Texture* texture;
  bool animCompleted;
  bool alive;
};

struct Arrow {
  float x, y;
  Sint32 w, h;
  float vx, vy;
  float startx, starty;
  float angle; 
  float time;
  Entity* target;
  bool alive;
};

struct Wall {
  Sint32 w, h;
  bool alive;
};

struct World {
  float gravity = 0.001f;
} world;

struct Ground {
  float x, y;
} ground1, ground2;

// Variables
Entity player;
Entity spearman, archer, ram;
Arrow arrows[MAX_ARROWS];
Wall walls[MAX_WALLS][4];
Entity* enemies[MAX_ENEMIES];
float lastAnimTick;

// Custom Blend Mode for Silhouettes
SDL_BlendMode silhouetteBlendMode;

// Quit game
bool quit = false;

// Game state
enum GameState {
  STATE_MENU,
  STATE_GAME
} gameState;

#include "game.cpp"
#include "entity.cpp"

bool load() {

    // Loading success flag
    bool success = true;
    
    // Init font
    font = TTF_OpenFont("assets/PixelText.ttf", 32);
    if (font == NULL) {
        printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
    }

    // Init camera
    camera.x = 0;
    camera.y = 0;
    camera.w = SCREEN_WIDTH;
    camera.h = SCREEN_HEIGHT;

    // Init ground
    ground1.x = camera.x;
    ground1.y = SCREEN_HEIGHT-256-64;
    ground2.x = 1024;
    ground2.y = SCREEN_HEIGHT-256-64;

    // Init textures
    spearmanTexture = loadTexture("assets/spearman.png");
    spearman.texture = spearmanTexture;
    spearman.x = 600;
    spearman.y = SCREEN_HEIGHT - 64 - 64;
    spearman.w = 64;
    spearman.h = 64;
    spearman.frameX = 0;
    spearman.anim = ANIM_IDLE;
    spearman.state = STATE_IDLE;

    archerTexture = loadTexture("assets/archer.png");
    archer.texture = archerTexture;
    archer.x = 100;
    archer.y = SCREEN_HEIGHT - 64 - 64;
    archer.w = 64;
    archer.h = 64;
    archer.frameX = 0;
    archer.anim = ANIM_IDLE;
    archer.state = STATE_IDLE;

    ramTexture = loadTexture("assets/ram.png");
    ram.texture = ramTexture;
    ram.x = 100;
    ram.y = SCREEN_HEIGHT - 64 - 64;
    ram.w = 128;
    ram.h = 64;
    ram.frameX = 0;
    ram.anim = ANIM_MOVE;

    arrowTexture = loadTexture("assets/arrow.png");
    for (Sint32 i = 0; i < MAX_ARROWS; i++) {
      arrows[i].w = 12;
      arrows[i].h = 2;
    }

    bg0Texture = loadTexture("assets/bg_0.png");
    bg1Texture = loadTexture("assets/bg_1.png");
    sunTexture = loadTexture("assets/sun.png");
    groundTexture = loadTexture("assets/ground.png");

    /* Walls */
    wallsTexture = loadTexture("assets/walls.png");
    walls[6][0].w = 64;
    walls[6][0].h = 64;
    walls[6][0].alive = true;

    // Player
    player.texture = archerTexture;
    player.x = 200;
    player.y = GROUND_POS - 64;
    player.w = 64;
    player.h = 64;
    player.frameX = 0;
    player.anim = ANIM_IDLE;
    player.state = STATE_IDLE;

    // Silhouettes custom blend mode
    /*silhouetteBlendMode = SDL_ComposeCustomBlendMode(
        SDL_BLENDFACTOR_ONE,
        SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
        SDL_BLENDOPERATION_ADD,
        SDL_BLENDFACTOR_ZERO,
        SDL_BLENDFACTOR_ONE,
        SDL_BLENDOPERATION_ADD
        );*/

    // Blank texture for silhouette blitting
    silhouetteTexture = createBlankTexture(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_TEXTUREACCESS_TARGET); 

    return success;
}

void close() {

    //Free loaded image
    SDL_DestroyTexture(archerTexture);
    SDL_DestroyTexture(spearmanTexture);

    // Free font
    TTF_CloseFont(font);
    font = NULL;
}

// Game logic
void input(SDL_Event* e) {

  SDL_Scancode code = e->key.keysym.scancode;
  if (e->type == SDL_KEYUP) {
    if (code == SDL_SCANCODE_SPACE) {
      fireArrow(&player, &spearman);
    }

    if (code == SDL_SCANCODE_LEFT) {
      player.vx = 0;
      toStateIdle(&player);
    } else if (code == SDL_SCANCODE_RIGHT) {
      player.vx = 0;
      toStateIdle(&player);
    }
  }

  if (e->type == SDL_KEYDOWN) {
    if (code == SDL_SCANCODE_LEFT) {
      player.vx = -PLAYER_SPEED;
      toStateMove(&player);
    } else if (code == SDL_SCANCODE_RIGHT) {
      player.vx = PLAYER_SPEED;
      toStateMove(&player);
    }
  }

}

void update(float dt) {

  /* Camera */
  updateCamera(dt);

  /* Entities */
  updateEntity(&player);
  updateEntity(&archer);
  updateEntity(&spearman);
  updateEntity(&ram);
  ram.x += 0.01f * dt;

  /* Arrows */
  updateArrows(dt);

}

void render() {

  // Clear screen
  SDL_RenderClear(renderer);

  // Render environment
  renderSun();
  renderBackground();
  renderGround();

  // Render entities
  // renderEntity(&player);
  //    renderEntity(&archer);
  renderEntity(&spearman);
  // renderEntity(&ram);
  renderArrows();

  // Render buildings
  renderWalls();

  // Render silhouettes
  //renderSilhouette(&player);

  // Bg color
  SDL_SetRenderDrawColor(renderer, 0xff, 0xf8, 0xec, 0xc9);
  
  // Update the screen
  SDL_RenderPresent(renderer);
}

int main(Sint32 argc, char* args[]) {

	//Start up SDL and create window
	if(!initFramework()) {
		printf("Failed to initialize!\n");
	}
	else {
		// Load media
		if(!load()) {
			printf("Failed to load media!\n");
		}
		else {

      // Event handler
      SDL_Event e;

      Sint32 countedFrames = 0;

      float currentTime = SDL_GetTicks();
      float accumulator = 0.0f;

      // While application is running
      while(!quit) {

        // Handle events on queue
        while(SDL_PollEvent(&e) != 0) {

          // User requests quit
          if(e.type == SDL_QUIT) {
            quit = true;
          }

          input(&e);
        }

        float avgFPS = countedFrames / (SDL_GetTicks() / 1000.f);
        if(avgFPS > 2000000) {
          avgFPS = 0;
        }

        float newTime = SDL_GetTicks();
        float frameTime = newTime - currentTime;
        currentTime = newTime;

        accumulator += frameTime;

        while (accumulator >= TIME_STEP) {

          // Update game
          update(TIME_STEP);
          accumulator -= TIME_STEP;

        }
        
        // Render game
        render();

        countedFrames++;
      }
    }
	}

	//Free resources and close SDL
	close();

	return 0;
}

