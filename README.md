# Marching Cubes
https://developer.nvidia.com/gpugems/gpugems3/part-i-geometry/chapter-1-generating-complex-procedural-terrains-using-gpu

## Algorithm
1. Get a block (1x1x1 in world space)
2. Divide block into 32x32x32
3. Use fragment shader to evaluate density function at each cell corner and store in 3D texture (there is one 3D texture shared across all blocks)
4. Visit each voxel and generate polygons, if any

## Creating 3D texture
- use glTexImage3D
