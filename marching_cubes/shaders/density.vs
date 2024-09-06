#version 330 core

layout(location = 0) in vec3 aPos;  // Quad vertex position

void main() {
    // Pass the vertex position to the fragment shader (in NDC)
    gl_Position = vec4(aPos, 1.0);
}
