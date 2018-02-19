/* Get wall in x, y */
Wall* getWall(float x, float y) {
  if (x >= 0 && x < MAX_WALLS * TILE_SIZE) {
    if (walls[(Sint32)floor(x / TILE_SIZE)][0].alive) {
      return &walls[(Sint32)floor(x / TILE_SIZE)][0];
    }
  } else {
    printf("[utils/getWall] warning! Wall out of bounds. Index: %d\n", (Sint32)floor(x / TILE_SIZE));
    return NULL;
  }
}
