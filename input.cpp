/* Input logic */
void updateInput(SDL_Event* e) {
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
    
    if (code == SDL_SCANCODE_UP) {
      climbWallUp(&player);
    } else if (code == SDL_SCANCODE_DOWN) {
      climbWallDown(&player);
    }
  }
}
