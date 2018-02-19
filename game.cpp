/* Camera */
void updateCamera(float dt) {

  if (camera.x < 0) { camera.x = 0; }

  camera.x += camera.vx * dt;

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

/* Silhouettes */
void renderSilhouette(Entity* entity) {

  if (!entity->alive) { return; }

  Sint32 currFrameX = (entity->frameX / ENTITY_ANIM_SPEED) * entity->w; 
  Sint32 currFrameY = entity->anim * entity->h; 

  SDL_SetTextureColorMod(entity->texture, 0, 0, 0);
  SDL_SetTextureBlendMode(entity->texture, silhouetteBlendMode);
  renderSprite(entity->texture, currFrameX, currFrameY, entity->w, entity->h, entity->x, entity->y, entity->w, entity->h, entity->flip, 0, 100);
  SDL_SetTextureColorMod(entity->texture, 255, 255, 255);

  //SDL_SetTextureBlendMode(player.texture, SDL_BLENDMODE_NONE);
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
  renderSilhouette(&player);
  for (Sint32 i = 0; i < MAX_ENTITIES; i++) {
    renderSilhouette(&blueEntities[i]);
    renderSilhouette(&redEntities[i]);
  }

  setRenderTarget(NULL);
  renderTexture(silhouetteTexture, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 255);
}

/* Missiles */
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
          applyDamage(target, 10);
          arrow->alive = false;
          continue;
        }

        // Check ground collision
        if (arrow->y >= SCREEN_HEIGHT - 64) {

          // Make it disappear
          if (arrow->alpha <= 0) {
            arrow->alive = false;
            arrow->alpha = 0;
          } else {
            arrow->alpha -= dt / FADE_RATE;
          }

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
      renderTexture(arrowTexture, arrow->x, arrow->y, 12, 2, arrow->angle * 180 / PI, arrow->alpha);
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
    float dx = (target->x + target->w/2 + target->vx * ARROW_FLY_TIME * 50) - (entity->x + entity->w/2);
    float dy = (target->y + target->h/2) - (entity->y + entity->h/2);
    arrow->vx = (float)dx / ARROW_FLY_TIME;
    arrow->vy = -(dy + 0.5 * world.gravity * ARROW_FLY_TIME * ARROW_FLY_TIME) / ARROW_FLY_TIME;
    arrow->time = 0;
    arrow->target = target;
    arrow->owner = entity;
    arrow->alpha = 255;
    arrow->alive = true;
    changeAnim(entity, ANIM_ATTACK);
    entity->state = STATE_ATTACK;
  }
}
