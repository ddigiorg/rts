# TODO

- add a logger class for standardized output
- cull all sprites not in view of camera
- implement uniform spatial partitioning
- implement unit formations

- build process failed on linux laptop... needs fix

## Alpha

- Fixed-size map (11x11 chunks?)
- Single biome: forest (grass, shallow water, water)
- Single character: can move, mine, melee attack; no skills
- Day only
- No fog of war
- All chunks active but only render within camera
- Resources: souls (enemies), berries (food), trees (wood), stone
- Buildable: banner, wall, bridge, unit
- Units: melee only, small groups (16 units per)
- Enemy Units: spawn off map and attack banner/player


SpriteRenderer

- renders all sprites (tiles, resources, units, etc.)

- [ ] make all sprites CENTERED
- [ ] break out Position3d into Position2d and ZOrder(?)

ChunkManager

- holds chunks
- each chunk 32x32 tiles
- each tile 32x32 pixels
- different tile types (grass, water)

  generate()
    for tile in tiles:
      generate tile type from perlin
      generate resource from perlin (add entity and set tile mask)
      generate tree from perlin (add entity and set tile mask)

- [ ] setup chunk object (each chunk 32x32 tiles)
- [ ] setup 11x11 chunks
- [ ] put grass, water_shallow, water on spritesheet
- [ ]

Entity

- resources, trees, buildings, character, units
- some have collision boxes, some dont

config data

- starting health?

runtime data

- uint16 id (2 bytes)
- uint8 type (1 byte)
- Position position (8 bytes)
- Velocity velocities (8 bytes)
- BoundingBox collisionBox (16 bytes)
- BoundingBox selectionBox (16 bytes)
- uint8 collisionMask (1 byte) // 0 means non-collidable?
- uint16 data1 (2 bytes) // health, mana, resource amount, etc.
- SpriteRender activeSprite (16 bytes)

Spatial Grid & Collision

```cpp
struct SpatialGridCell {
    std::array<SpatialGridCell, 8> neighbors; // 64 bytes (assume 64-bit system)
    std::array<Position2d, 4> positions; // 32 bytes
    std::array<Velocity2d, 4> velocities; // 32 bytes
    std::array<BoundingBox, 4> collisionBoxes; // 64 bytes
    uint8_t count; // 1 byte
};
```

```cpp
// Note: ContiguousVector is a custom swap-and-pop vector for cache locality

struct EntityRecord {
    // Note: NULL_INDEX is MAX_UINT16_VALUE
    uint16_t chunkX;
    uint16_t chunkY;
    uint16_t chunkIdx;

    bool dynamic;
    uint16_t gridX;
    uint16_t gridY;
    uint16_t gridIdx;
};

struct EntityData {
    EntityType type;
    uint16_t health;
    // ...etc
};

struct World {
    ChunkManager chunks;
    SpatialGrid grid;

    // TODO: should these be contiguous instead?
    // Note: EntityID is the index into these vectors
    // Note: NULL_ENTITY is maximum entity id value (probably uint32)
    std::vector<EntityRecord> entityRecords;
    std::vector<EntityData> entityDatas;

    std::vector<EntityID> recycledEntities;
};

// 32x32 tiles
struct Chunk {
    std::vector<Chunk*> neighbors;

    std::array<std::array<TileType, TILE_COUNT_X>, TILE_COUNT_Y> tileTypes;
    std::array<std::array<TileMask, TILE_COUNT_X>, TILE_COUNT_Y> tileMasks;

    ContiguousVector<SpriteRender> spritePositions; // x, y
    ContiguousVector<SpriteRender> spriteTexCoords; // u, v, w, h
};

class ChunkManager {
private:
    std::array<std::array<Chunk, CHUNK_COUNT_X>, CHUNK_COUNT_Y> chunks;
};


using Neighbors = std::array<std::array<uint16_t, 2>, 8>; // Note: NULL_INDEX is MAX_UINT16_VALUE

struct StaticCell {
    // static entities (e.g. trees, buildings, resources, decoration)
    ContiguousVector<EntityID> sIds;
    ContiguousVector<Position> sPositions;
    ContiguousVector<BoundingBox> sSelectionBoxes;
};

struct DynamicCell {
    // dynamic entities (e.g units)
    ContiguousVector<EntityID> dIds;
    ContiguousVector<Position> dPositions;
    ContiguousVector<Velocity> dVelocities;
    ContiguousVector<BoundingBox> dCollisionBoxes;
    ContiguousVector<BoundingBox> dSelectionBoxes;
    ContiguousVector<EntityMask> dEntityMasks; // collision masks
};

// 2x2 tiles (avg dynamic entity size is 1x1 tile)
class SpatialGrid {
public:
    uint16_t addStaticEntity();
    uint16_t addDynamicEntity();
    void delStaticEntity();
    void delDynamicEntity();

private:
    std::array<std::array<Neighbors, GRID_COUNT_X - 2>, GRID_COUNT_Y - 2> neighbors;
    std::array<std::array<StaticCell, GRID_COUNT_X>, GRID_COUNT_Y> staticCells;
    std::array<std::array<DynamicCell, GRID_COUNT_X>, GRID_COUNT_Y> dynamicCells;
};
```


```cpp
enum TileCollisionMask : uint8_t {
    NONE        = 0b0000, // Walkable
    BLOCKED     = 0b0001, // Fully blocked (water, walls)
    SLOW        = 0b0010, // Slows movement (trees, mud)
    DANGEROUS   = 0b0100, // Traps, spikes
};

if (tileCollisionMasks[tileIndex] & BLOCKED) {
    // Can't move here
}
```


quadtree info: https://stackoverflow.com/questions/41946007/efficient-and-well-explained-implementation-of-a-quadtree-for-2d-collision-det



- tile masks loaded from chunks
- non-collidable data: store entity Postion, SpriteRender data
- collidable data: store entity Position, BoundingBox, collisionMask, SpriteRender data

Global Entity Vector

- index = EntityID
- EntityInfo
  - SpatialGrid* grid
  - uint8? collidable?
  - uint16? index



Ranges? (closest to furthest)

- render (based on camera zoom)
  - render chunk tiles based on camera bounding box
  - render chunk static entities camera bounding box
  - render spatial grid dynamic entities based on camera bounding box
- active
  - predefined chunk radius
  - predefined spatial grid radius
- loaded
  - no spatial grid





```cpp
using EntityID = uint32_t;
using EntityType = uint8_t; // probably enum instead

struct EntityRecord {
    uint16_t largeIndex;
    uint8_t mediumIndex;
    uint8_t smallIndex;
    uint8_t index;
};

struct EntityData {
    EntityType type;
    // store entity data
    // ...
};

class World {
    std::unordered_map<uint16_t, CellLarge> activeChunks;

    // Note: EntityID is the index into these vectors
    // Note: NULL_ENTITY is maximum entity id value
    std::vector<EntityRecord> entityRecords;
    std::vector<EntityData> entityDatas;

    std::vector<EntityID> recycledEntities;
};

class CellLarge {
    constexpr SIZE = 32 * 32; // tiles
    std::array<CellLarge*, 8> neighbors = {nullptr};
    std::array<std::optional<CellMedium>, 16> cells; // only allocate/deallocate when needed

};

class CellMedium {
    constexpr SIZE = 8 * 8; // tiles
    std::array<CellMedium*, 8> neighbors = {nullptr};
    std::array<std::optional<CellMedium>, 16> cells; // only allocate/deallocate when needed

    // store tile and static entity data in contiguous vectors
    // ...
};

class CellSmall {
    constexpr SIZE = 2 * 2; // tiles
    std::array<CellSmall*, 8> neighbors = {nullptr};

    // store dynamic entity data in contiguous vectors
    // ...
};
```