# Code Ideas

## Sprite Rendering

1. texture atlas
  - use a single large texture atlas is ok for now
  - will run into VRAM issues at some point so look into virtual texturing
  - https://factorio.com/blog/post/fff-227

2. mipmaps
  - https://learnopengl.com/Getting-started/Textures

3. rendering optimizations
  - https://factorio.com/blog/post/fff-251
  - instancing for objects with low polygon count is slow on OLD GPUs
  - New GPUs are optimized for instancing

4. blending
    - https://factorio.com/blog/post/fff-172

5. z-buffer


## Path Finding

- global: hierarchical pathfinding across chunks (like factorio) then A* (see FFF #317)
- local: 
- pre calculate connectivity map?
- flow fields?
- need collision detection with other objects
- use flocking behavior: calculate pathfinding for one "ghost" unit and have real units try to stay within range of it

