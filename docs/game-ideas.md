# Game Ideas

## Names

## Gameplay

- Fantasy
- Real time strategy
- Open world like Minecraft
- Exploration, resource mining, base building, killing enemies
- Each player gets a single customizable hero unit
- Each player can establish a main base and outposts like Colony Survival
- Occasional attacks from enemy mobs/bases

## World

- Grid/tile based initially (WC1, WC2)
- Eventually overhaul to isometric for better looks (SC, AoE)
- Cliffs and height maps (like WC3)
- Water
- Fog of war: black for unloaded chunks, grey for far chunks, visible for nearby chunks
- Day/night cycles eventually
- Different biomes eventually
- Weather eventually

## Entities

- 2D sprite-based (like wc2 & aoe2)

## Army Control

- Semi automated AI with standard commands
- Default to attack move instead of just move like Dune 2 The Golden Path Mod?
- Units can get formations (like aoe) that provide benefits to the group:
  - Default: defense/offense bonuses
  - Column: faster for marching?
- Multiple units selected become a temporary grouping like AoE (get a flag overhead like twwh)

## Resources

- Each resource is a sprite on top of terrain (like aoe)
- dirt for modifying terrain like minecraft (hard)

high prio ideas

- food
- wood
- stone
- coal
- iron
- gold

low prio ideas

- dirt
- water
- gems
- crystal
- oil
- obsidian
- tin
- copper
- bronze
- silver
- flint
- wool

## Path Finding

- global: hierarchical pathfinding across chunks (like factorio) then A* (see FFF #317)
- local: 
- pre calculate connectivity map?
- flow fields?
- need collision detection with other objects
- use flocking behavior: calculate pathfinding for one "ghost" unit and have real units try to stay within range of it

## Inspirations

- warcraft 1, 2, 3
- starcraft 1, 2
- age of empires
- total war warhammer
- heroes of might and magic
- minecraft
- shining force
- world box
