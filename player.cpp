/* Player */
Entity* getNextEnemy() {

  for (Sint32 i = 0; i < MAX_ENTITIES; i++) {
    Entity* entity = &redEntities[i];
    if (entity->alive) {
      return entity;
    }
  }

}

void createPlayer() {

  player.texture = archerTexture;
  player.x = 200;
  player.y = GROUND_POS - 64;
  player.w = 64;
  player.h = 64;
  player.frameX = 0;
  player.anim = ANIM_IDLE;
  player.state = STATE_IDLE;
  player.alpha = 255;
  player.alive = true;
  player.hp = 100;
  player.type = ARCHER;

}

void updatePlayer(float dt) {

  updateEntity(&player, dt);

  Uint32 now = SDL_GetTicks();
  Entity* enemy = getNextEnemy();

  if (enemy != NULL) {
    bool facingEnemy = !player.flip && player.x < enemy->x || player.flip && player.x > enemy->x;

    if (enemy != NULL && now - player.lastAttackTime > FIRE_RATE && facingEnemy && player.state == STATE_IDLE) {
      player.lastAttackTime = now;
      fireArrow(&player, enemy);
    }
  }

}

