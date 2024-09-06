#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <Shader.h>
#include <Camera.h>
#include <FileSystem.h>

#include <iostream>

#define HEIGHT 250
#define WIDTH 250

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void calculateDensityTexture();
void renderFullScreenQuad(int VAO);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
  // glfw: initialize and configure
  // ------------------------------
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  // glfw window creation
  // --------------------
  GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Mountain Scene", NULL, NULL);
  if (window == NULL)
    {
      std::cout << "Failed to create GLFW window" << std::endl;
      glfwTerminate();
      return -1;
    }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);

  // tell GLFW to capture our mouse
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
      std::cout << "Failed to initialize GLAD" << std::endl;
      return -1;
    }

  // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
  stbi_set_flip_vertically_on_load(true);

  // configure global opengl state
  // -----------------------------
  glDisable(GL_DEPTH_TEST);

  // FIRST RENDERING PASS HERE: 3D TEXTURE WITH FRAG SHADER
  calculateDensityTexture();

  glfwTerminate();
  return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.ProcessKeyboard(FORWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.ProcessKeyboard(BACKWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.ProcessKeyboard(LEFT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  // make sure the viewport matches the new window dimensions; note that width and 
  // height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
  float xpos = static_cast<float>(xposIn);
  float ypos = static_cast<float>(yposIn);

  if (firstMouse)
    {
      lastX = xpos;
      lastY = ypos;
      firstMouse = false;
    }

  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

  lastX = xpos;
  lastY = ypos;

  camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
  camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void calculateDensityTexture()
{
  GLuint textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_3D, textureID);
  glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, 33, 33, 33, 0, GL_RED, GL_FLOAT, NULL);
  
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  // Create and bind a framebuffer
  GLuint fbo;
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);

  /* Load and execute the fragment shader */
  Shader densityShader("marching_cubes/shaders/density.vs", "marching_cubes/shaders/density.fs");
  densityShader.use();
  glViewport(0, 0, 33, 33);  // Set viewport to match the texture size
  glClear(GL_COLOR_BUFFER_BIT);

  glm::vec3 blockOrigin = glm::vec3(0, 0, 0);
  float blockSize = 32;
  densityShader.setVec3("blockOrigin", blockOrigin);
  densityShader.setFloat("blockSize", blockSize);

  // Quad vertices (2 triangles covering the entire screen)
  // Positions in NDC (x, y) - z = 0 for 2D plane
  float quadVertices[] = {
    // First triangle
    -1.0f, -1.0f, 0.0f,  // Bottom-left
    1.0f, -1.0f, 0.0f,  // Bottom-right
    1.0f,  1.0f, 0.0f,  // Top-right

    // Second triangle
    -1.0f, -1.0f, 0.0f,  // Bottom-left
    1.0f,  1.0f, 0.0f,  // Top-right
    -1.0f,  1.0f, 0.0f   // Top-left
  };

  GLuint fullScreenQuadVAO, fullScreenQuadVBO;

  // Generate and bind the VAO
  glGenVertexArrays(1, &fullScreenQuadVAO);
  glBindVertexArray(fullScreenQuadVAO);

  // Generate and bind the VBO
  glGenBuffers(1, &fullScreenQuadVBO);
  glBindBuffer(GL_ARRAY_BUFFER, fullScreenQuadVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

  // Enable and set the vertex attribute pointer
  // Assuming the layout(location = 0) is for position in the vertex shader
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

  // Unbind the VAO
  glBindVertexArray(0);
  
  for (int z = 0; z < 33; ++z) { 
    glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_3D, textureID, 0, z);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      std::cout << "Error creating 3D texture framebuffer" << std::endl;
      exit(1);
    }

    // Set the current z slice
    densityShader.setFloat("zSlice", z);

    // Render a full-screen quad to evaluate the fragment shader for this slice
    renderFullScreenQuad(fullScreenQuadVAO);
  }

  // Unbind the framebuffer after rendering
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  /* VERIFY CONTENTS OF FRAMEBUFFER */
  // Assuming the texture is bound to GL_TEXTURE_3D
  glBindTexture(GL_TEXTURE_3D, textureID);

  // Allocate memory for the texture data
  float* densityData = new float[33 * 33 * 33];  // 33x33x33 because of the 33 slices

  // Read the texture data from the GPU
  glGetTexImage(GL_TEXTURE_3D, 0, GL_RED, GL_FLOAT, densityData);

  // Now `densityData` contains the density values, you can inspect them
  for (int z = 0; z < 33; ++z) {
    for (int y = 0; y < 33; ++y) {
      for (int x = 0; x < 33; ++x) {
        int index = x + y * 33 + z * 33 * 33;
        std::cout << "Density[" << x << "," << y << "," << z << "] = " << densityData[index] << std::endl;
      }
    }
  }

  // Don't forget to free the memory afterwards
  delete[] densityData;
}

void renderFullScreenQuad(int VAO)
{
  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);  // 6 vertices = 2 triangles
  glBindVertexArray(0);
}

