#version 330 core

out float density;  // This is the output to the texture

uniform float zSlice;  // Current slice in the 3D texture
uniform vec3 blockOrigin;  // World space position of the block
uniform float blockSize;   // Size of the block in world units

float calculateDensity(vec3 position) {
  return -position.y;
}

void main() {
  // viewport needs to be 32x32
  int u = int(gl_FragCoord.x);
  int v = int(gl_FragCoord.y);
  int w = int(zSlice);

  vec3 voxelCoord = vec3(u, v, w);

  vec3 worldPosition = blockOrigin + voxelCoord * blockSize;
  density = calculateDensity(worldPosition);
}
