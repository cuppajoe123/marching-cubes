# A simple Makefile for compiling small SDL projects

# set the compiler
CC := g++

# set the compiler flags
CXXFLAGS := -Wall -Llib -lassimp -lz -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl -Imarching_cubes

# add header files here
HDRS := marching_cubes/Shader.h marching_cubes/Mesh.h marching_cubes/Camera.h marching_cubes/FileSystem.h marching_cubes/glad/glad.h marching_cubes/GLFW/glfw3.h marching_cubes/stb_image.h

# add source files here
SRCS := marching_cubes/glad.cpp marching_cubes/Shader.cpp marching_cubes/Mesh.cpp marching_cubes/Camera.cpp marching_cubes/$(IN).cpp

# generate names of object files
OBJS := $(SRCS:.cpp=.o)

# name of executable
EXEC := $(IN)

# default recipe
all: $(EXEC)

# recipe for building the final executable
$(EXEC): $(OBJS) $(HDRS) Makefile
	$(CC) -o $@ $(OBJS) $(CXXFLAGS)

# recipe for building object files
$(OBJS): $(@:.o=.cpp) $(HDRS) Makefile
	$(CC) -o $@ $(@:.o=.cpp) -c $(CXXFLAGS)

# recipe to clean the workspace
clean:
	rm -f $(EXEC) marching_cubes/*.o

.PHONY: all clean
