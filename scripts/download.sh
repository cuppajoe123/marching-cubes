#!/bin/bash
wget https://github.com/glfw/glfw/releases/download/3.3.8/glfw-3.3.8.zip
unzip glfw-3.3.8.zip
mv glfw-3.3.8 ../lib/

wget https://www.zlib.net/zlib-1.3.1.tar.gz
gunzip zlib-1.3.1.tar.gz
tar -xvf zlib-1.3.1.tar
mv zlib-1.3.1 ../lib/

wget https://github.com/assimp/assimp/archive/refs/tags/v5.4.0.zip
unzip v5.4.0.zip
mv assimp-5.4.0 ../lib/
# For GLFW, run cmake and make. For zlib, run ./configure and make. For assimp, run `cmake CMakeLists.txt -DBUILD_SHARED_LIBS=OFF` and make. Then find the static library for each and copy it into lib/.
