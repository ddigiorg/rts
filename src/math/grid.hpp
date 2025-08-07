
// =============================================================================
// Orthoganal Coordinate Functions
// =============================================================================
// TODO: bitshift versions for faster conversions
// TODO: put in utils as orthogonal.hpp?

// inline Position3f chunkGridToWorld(const Position2i& grid){
//     float worldX = grid.x * CHUNK_SIZE_X;
//     float worldY = grid.y * CHUNK_SIZE_Y;
//     return Position3f(worldX, worldY, 0.0f);
// }

// inline Position2i chunkWorldToGrid(const Position3f& world){
//     int indexX = std::floor(world.x / CHUNK_SIZE_X);
//     int indexY = std::floor(world.y / CHUNK_SIZE_Y);
//     return Position2i(indexX, indexY);
// }

// inline Position3f sectorGridToWorld(const Position2i& index){
//     float worldX = index.x * SECTOR_SIZE_X;
//     float worldY = index.y * SECTOR_SIZE_Y;
//     return Position3f(worldX, worldY, 0.0f);
// }

// inline Position2i sectorWorldToGrid(const Position3f& world){
//     int indexX = std::floor(world.x / SECTOR_SIZE_X);
//     int indexY = std::floor(world.y / SECTOR_SIZE_Y);
//     return Position2i(indexX, indexY);
// }

// inline Position3f tileGridToWorld(const Position2i& index){
//     float worldX = index.x * TILE_SIZE_X;
//     float worldY = index.y * TILE_SIZE_Y;
//     return Position3f(worldX, worldY, 0.0f);
// }

// inline Position2i tileWorldToGrid(const Position3f& world){
//     int indexX = std::floor(world.x / TILE_SIZE_X);
//     int indexY = std::floor(world.y / TILE_SIZE_Y);
//     return Position2i(indexX, indexY);
// }

// =============================================================================
// Isometric Coordinate Functions
// =============================================================================
// TODO: bitshift versions for faster conversions

// inline Position tileGridToWorld(const Location& index){
//     float worldX = (index.x - index.y) * (TILE_SIZE_X * 0.5f);
//     float worldY = (index.y + index.x) * (TILE_SIZE_Y * 0.5f);
//     return Position{worldX, worldY};
// }

// inline Location tileWorldToGrid(const Position& world){
//     int indexX = std::floor((world.x / TILE_SIZE_X) + (world.y / TILE_SIZE_Y));
//     int indexY = std::floor((world.y / TILE_SIZE_Y) - (world.x / TILE_SIZE_X));
//     return Location{indexX, indexY};
// }
