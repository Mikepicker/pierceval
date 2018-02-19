/* Animations */
void changeAnim(Entity* entity, Anim newAnim) {
  entity->frameX = 0;
  entity->anim = newAnim;
}

/* Manage entities state changes */
void toStateIdle(Entity* entity) {
  if (entity->state == STATE_IDLE || entity->state == STATE_DIE) { return; }
  changeAnim(entity, ANIM_IDLE);
  entity->state = STATE_IDLE;
  entity->vx = 0;
}

void toStateMove(Entity* entity) {
  if (entity->state == STATE_MOVE || entity->state == STATE_DIE) { return; }
  changeAnim(entity, ANIM_MOVE);
  entity->state = STATE_MOVE;
}

void toStateAttack(Entity* entity) {
  if (entity->state == STATE_ATTACK || entity->state == STATE_DIE) { return; }
  changeAnim(entity, ANIM_ATTACK);
  entity->state = STATE_ATTACK;
  entity->vx = 0;
}

void toStateDie(Entity* entity) {
  if (entity->state == STATE_DIE || entity->state == STATE_DIE) { return; }
  changeAnim(entity, ANIM_DIE);
  entity->state = STATE_DIE;
  entity->vx = 0;
}

/* Damage */
void applyDamage(Entity* entity, Sint32 damage) {
  entity->hp -= damage;
  if (entity->hp <= 0) {
    toStateDie(entity);
  }
}

/* Create entities */
Entity* getDeadEntity(Team team) {
  Entity* list;

  if (team == TEAM_BLUE) {
    list = blueEntities;
  } else {
    list = redEntities;
  }

  for (Sint32 i = 0; i < MAX_ENTITIES; i++) {
    Entity* entity = &list[i];
    if (!entity->alive) {
      return entity;
    }
  }

  printf("[getDeadEntity] warning! No more entities\n");
  return NULL;
}

Entity* createEntity(EntityType type, Sint32 x, Sint32 y, Team team) {
 
  Entity* entity = getDeadEntity(team);
  entity->x = x;
  entity->y = y;
  entity->alpha = 255;
  entity->alive = true;
  entity->onWall = false;
  entity->anim = ANIM_MOVE;
  entity->state = STATE_MOVE;
  entity->hp = 100;
  entity->type = type;

  switch(type) {
    case SPEARMAN:
      entity->texture = spearmanTexture;
      entity->w = 64;
      entity->h = 64;
      break;
    case ARCHER:
      entity->texture = archerTexture;
      entity->w = 64;
      entity->h = 64;
      break;
    case RAM:
      entity->texture = ramTexture;
      entity->w = 128;
      entity->h = 64;
      break;
  }

  return entity;

}

/* Update entities */
void updateEntity(Entity* entity, float dt) {

  if (!entity->alive) { return; }

  // Update position
  entity->x += entity->vx;

  // Avoid falling from walls
  if (entity->onWall) {
    Wall* wall = getWall(entity->x + entity->w/2, entity->y + entity->w/2);
    if (entity->x < wall->x - 16 && getWall(wall->x - TILE_SIZE, wall->y) == NULL) { // Left
      entity->x = wall->x - 16;
    } else if (entity->x + entity->w > wall->x + wall->w + 16 && getWall(wall->x + TILE_SIZE, wall->y) == NULL) { // Right
      entity->x = wall->x + wall->w - entity->w + 16;
    }
  }

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
        entity->attacked = false;
      }
      break;
    case STATE_DIE:
      entity->vx = 0;
      if (entity->animCompleted) {
        entity->frameX = 3 * ENTITY_ANIM_SPEED;
        
        if (entity->alpha <= 0) {
          entity->alpha = 0;
          entity->alive = false;
        } else {
          entity->alpha -= dt / FADE_RATE;
        }
      }
      break;
  }

  // Entity specific behaviors
  switch(entity->type) {
    case SPEARMAN:

      if ((Sint32)(entity->frameX / ENTITY_ANIM_SPEED) == 1 &&
          entity->state == STATE_ATTACK &&
          !entity->attacked) {
        applyDamage(&player, 10);
        entity->attacked = true;
      }
     
      break;
  }

}

void renderEntity(Entity* entity) {

  if (!entity->alive) { return; }

  Sint32 currFrameX = (entity->frameX / ENTITY_ANIM_SPEED) * entity->w; 
  Sint32 currFrameY = entity->anim * entity->h; 
  renderSprite(entity->texture, currFrameX, currFrameY, entity->w, entity->h, entity->x - camera.x, entity->y, entity->w, entity->h, entity->flip, 0, entity->alpha);

  // Render health bar
  SDL_Rect redBar = {(Sint32)entity->x + 16, (Sint32)entity->y - 4, 32, 4};
  setRenderColor(0xff, 0x00, 0x00, 0xff);
  renderRect(redBar);

  SDL_Rect healthBar = {(Sint32)entity->x + 16, (Sint32)entity->y - 4, entity->hp * 32 / 100, 4};
  setRenderColor(0x00, 0xff, 0x00, 0xff);
  renderRect(healthBar);

  setRenderColor(0xff, 0xf8, 0xec, 0xc9);
}

void updateTeam(Team team, float dt) {
  
  Entity* list;

  if (team == TEAM_BLUE) {
    list = blueEntities;
  } else {
    list = redEntities;
  }

  for (Sint32 i = 0; i < MAX_ENTITIES; i++) {
    updateEntity(&list[i], dt);
  }

}

void renderTeam(Team team) {

  Entity* list;

  if (team == TEAM_BLUE) {
    list = blueEntities;
  } else {
    list = redEntities;
  }

  for (Sint32 i = 0; i < MAX_ENTITIES; i++) {
    renderEntity(&list[i]);
  }

}

/* Misc */
void climbWallUp(Entity* entity) {

  Wall* wall = getWall(entity->x + entity->w/2, entity->y + entity->h/2);
  if (wall != NULL) {
    entity->y = GROUND_POS - 128;
    entity->onWall = true;
  }

}

void climbWallDown(Entity* entity) {
  
  if (entity->onWall) {
    entity->y = GROUND_POS - 64;
    entity->onWall = false;
  }

}
