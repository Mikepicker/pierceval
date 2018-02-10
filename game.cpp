/* Camera */
void updateCamera(float dt) {

  if (camera.x < 0) { camera.x = 0; }

  camera.x += camera.vx * dt;

}

/* Entities */
void changeAnim(Entity* entity, Anim newAnim) {
  
  entity->frameX = 0;
  entity->anim = newAnim;

}

void updateEntity(Entity* entity) {

  // Update position
  entity->x += entity->vx;

  // Update flip
  if (entity->vx != 0) {
    entity->flip = entity->vx < 0 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
  }

  // Update animations
  if ((++entity->frameX / ENTITY_ANIM_SPEED) >= 4) {
    entity->frameX = 0;
    entity->animCompleted = true;
  } else if (entity->state != STATE_DIE){
    entity->animCompleted = false;
  }

  // Update state
  switch(entity->state) {
    case STATE_ATTACK:
      if (entity->animCompleted) {
        changeAnim(entity, ANIM_IDLE);
        entity->state = STATE_IDLE;
        entity->animCompleted = false;
      }
      break;
    case STATE_DIE:
      if (entity->animCompleted) {
        entity->frameX = 3 * ENTITY_ANIM_SPEED;
      }
      break;
  }

}

void renderEntity(Entity* entity) {
  Sint32 currFrameX = (entity->frameX / ENTITY_ANIM_SPEED) * entity->w; 
  Sint32 currFrameY = entity->anim * entity->h; 
  renderSprite(entity->texture, currFrameX, currFrameY, entity->w, entity->h, entity->x - camera.x, entity->y, entity->w, entity->h, entity->flip, 0, 255);
}

void renderSilhouette(Entity* entity) {

  /*Sint32 leftTile = (Sint32)floor(entity->x / 64);
  if (leftTile < 0) { leftTile = 0; }
  else if (leftTile > MAX_WALLS) { leftTile = MAX_WALLS; }

  Sint32 rightTile = (Sint32)floor((entity->x + 64) / 64);
  if (rightTile < 0) { rightTile = 0; }
  else if (rightTile > MAX_WALLS) { rightTile = MAX_WALLS; }

  Wall* leftWall = &walls[leftTile][0];
  Wall* rightWall = &walls[rightTile][0];

  Sint32 currFrameX = (entity->frameX / ENTITY_ANIM_SPEED) * entity->w; 
  Sint32 currFrameY = entity->anim * entity->h; 

  // Draw only if entity is behind a wall
  if (leftWall->alive) {
    Sint32 p = entity->x - leftTile * TILE_SIZE;

    SDL_SetTextureColorMod(entity->texture, 0, 0, 0);

    if (entity->flip == SDL_FLIP_HORIZONTAL) {
      renderSprite(entity->texture, currFrameX + entity->w - p, currFrameY, p, entity->h, entity->x - camera.x, entity->y, p, entity->h, entity->flip, 1, 255);
    } else {
      renderSprite(entity->texture, currFrameX, currFrameY, entity->w - p, entity->h, entity->x - camera.x, entity->y, entity->w - p, entity->h, entity->flip, 0, 255);
    }

    SDL_SetTextureColorMod(entity->texture, 255, 255, 255);
  }
  
  if (rightWall->alive) {
    Sint32 p = entity->x + entity->w - rightTile * TILE_SIZE;

    SDL_SetTextureColorMod(entity->texture, 0, 0, 0);

    if (entity->flip == SDL_FLIP_HORIZONTAL) {
      renderSprite(entity->texture, currFrameX + p, currFrameY, entity->w - p, entity->h, entity->x + p - camera.x, entity->y, entity->w - p, entity->h, entity->flip, 1, 255);
    } else {
      renderSprite(entity->texture, currFrameX + entity->w - p, currFrameY, p, entity->h, entity->x + entity->w - p - camera.x, entity->y, p, entity->h, entity->flip, 1, 255);
    }

    SDL_SetTextureColorMod(entity->texture, 255, 255, 255);
  }*/

  
}

/* Arrows */
Arrow* getDeadArrow() {

  for (Sint32 i = 0; i < MAX_ARROWS; i++) {
    Arrow* arrow = &arrows[i];
    if (!arrow->alive) { return arrow; }
  }

  printf("[getAliveArrow] no more arrows in pool!\n");
  return NULL;

}

void updateArrows(float dt) {
  for (Sint32 i = 0; i < MAX_ARROWS; i++) {
    Arrow* arrow = &arrows[i];
    if (arrow->alive) {

      arrow->time += dt;
      if (arrow->time > ARROW_FLY_TIME) {

        // Check entity collision
        Entity* target = arrow->target;
        SDL_Rect arrowRect = { (Sint32)arrow->x, (Sint32)arrow->y, arrow->w, arrow->h };
        SDL_Rect targetRect = { (Sint32)target->x, (Sint32)target->y, target->w, target->h };
        if (target->state != STATE_DIE && rectCollision(arrowRect, targetRect)) {
          target->state = STATE_DIE;
          changeAnim(target, ANIM_DIE);
          arrow->alive = false;
        }

        // Check ground collision
        if (arrow->y >= SCREEN_HEIGHT - 64) {
          continue;
        }

      }

      float vx = arrow->vx * arrow->time;
      float vy = arrow->vy * arrow->time + 0.5 * world.gravity * arrow->time * arrow->time;
      arrow->x = arrow->startx + vx;
      arrow->y = arrow->starty + vy;

      arrow->angle = atan2(arrow->vy + world.gravity * arrow->time, arrow->vx);
    }
  }
}

void renderArrows() {
  for (Sint32 i = 0; i < MAX_ARROWS; i++) {
    Arrow* arrow = &arrows[i];
    if (arrow->alive) {
      renderTexture(arrowTexture, arrow->x, arrow->y, 12, 2, arrow->angle * 180 / PI, 255);
    }
  }
}

void fireArrow(Entity* entity, Entity* target) {
  Arrow* arrow = getDeadArrow();
  if (arrow != NULL) {
    arrow->x = entity->x + entity->w/2;
    arrow->y = entity->y + entity->h/2;
    arrow->startx = arrow->x;
    arrow->starty = arrow->y;
    float dx = (target->x + target->w/2) - (entity->x + entity->w/2);
    float dy = (target->y + target->h/2) - (entity->y + entity->h/2);
    arrow->vx = (float)dx / ARROW_FLY_TIME;
    arrow->vy = -(dy + 0.5 * world.gravity * ARROW_FLY_TIME * ARROW_FLY_TIME) / ARROW_FLY_TIME;
    arrow->time = 0;
    arrow->target = target;
    arrow->alive = true;
    changeAnim(entity, ANIM_ATTACK);
    entity->state = STATE_ATTACK;
  }
}

/* Environment */
void renderSun() {
  renderTexture(sunTexture, (SCREEN_WIDTH / 2) - 240, 100, 480, 480, 0, 255);
}

void scrollGround(Ground* ground) {
  if (ground->x + 1024 < camera.x) {
    ground->x = floor((camera.x + SCREEN_WIDTH) / 1024) * 1024;
  } else if (ground->x > camera.x + SCREEN_WIDTH) {
    ground->x = floor(((camera.x - 1024) / 1024) + 1) * 1024;
  }
}

void renderGround() {
  scrollGround(&ground1);
  scrollGround(&ground2);
  renderTexture(groundTexture, ground1.x - camera.x, SCREEN_HEIGHT - 64, 1024, 64, 0, 255);
  renderTexture(groundTexture, ground2.x - camera.x, SCREEN_HEIGHT - 64, 1024, 64, 0, 255);
}

void renderBackground() {
  renderTexture(bg0Texture, 0, GROUND_POS - 256, 1024, 256, 0, 255);
  renderTexture(bg1Texture, 0, GROUND_POS - 256, 1024, 256, 0, 255);
}

/* Buildings */
void renderWalls() {

  setRenderTarget(silhouetteTexture);

  // Set transparent background
  SDL_SetTextureBlendMode(silhouetteTexture, SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  SDL_RenderClear(renderer);

  // Draw background
  float offY = GROUND_POS - 64;
  for (Sint32 row = 0; row < MAX_WALLS; row++) {
    for (Sint32 col = 0; col < 4; col++) {
      Wall* wall = &walls[row][col];
      if (wall->alive) {
        renderSprite(wallsTexture, 0, 0, 64, 64, row * wall->w - camera.x, offY - col * wall->h, wall->w, wall->h, SDL_FLIP_NONE, 0, 255);
        renderSprite(wallsTexture, 64 * 3, 0, 64, 64, row * wall->w - camera.x, offY - wall->h, wall->w, wall->h, SDL_FLIP_NONE, 0, 255);
      }
    }
  }

  // Render entities silhouettes
  Sint32 currFrameX = (player.frameX / ENTITY_ANIM_SPEED) * player.w; 
  Sint32 currFrameY = player.anim * player.h; 

  SDL_SetTextureColorMod(player.texture, 0, 0, 0);
  SDL_SetTextureBlendMode(player.texture, SDL_BLENDMODE_ADD);
  renderSprite(player.texture, currFrameX, currFrameY, player.w, player.h, player.x, player.y, player.w, player.h, player.flip, 0, 255);
  SDL_SetTextureColorMod(player.texture, 255, 255, 255);

  setRenderTarget(NULL);
  renderTexture(silhouetteTexture, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 255);
  //SDL_SetTextureBlendMode(player.texture, SDL_BLENDMODE_NONE);

}
