void updateAI() {
  
  /* Process enemies */
  for (Sint32 i = 0; i < MAX_ENTITIES; i++) {
    Entity* entity = &redEntities[i];
    if (entity->alive && entity->state != STATE_DIE) {

      float dx = player.x - entity->x;

      // Check player death
      if (player.state == STATE_DIE) {
        toStateIdle(entity);
        continue;
      }

      // Check player off range
      bool targetOutOfRange = player.y != entity->y;
      if (targetOutOfRange) {
        toStateIdle(entity);
        continue;
      }

      // Attack
      if (abs(dx) < 32) {
        toStateAttack(entity);
      } else { // Chase player
        entity->vx = (dx/abs(dx)) * ENTITY_SPEED;
        toStateMove(entity);
      }
    }
  }

}
