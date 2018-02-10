/* Manage entities state changes */
void toStateIdle(Entity* entity) {
  if (entity->state == STATE_IDLE) { return; }
  changeAnim(entity, ANIM_IDLE);
  entity->state = STATE_IDLE;
}

void toStateMove(Entity* entity) {
  if (entity->state == STATE_MOVE) { return; }
  changeAnim(entity, ANIM_MOVE);
  entity->state = STATE_MOVE;
}

/* Create entities */
void getDeadEnemy() {
  for (Sint32 i = 0; i < MAX_ENEMIES; i++) {
    Entity* enemy = enemies[i];
    if (!enemy->alive) {

    }
  }
}

void createSpearman() {

}
