#!/bin/bash

# Compilazione degli shader Phong
echo "Compiling Phong vertex shader..."
glslc phong/PhongShader.vert -o phong/PhongVert.spv
echo "Done."
echo "Compiling Phong fragment shader..."
glslc phong/PhongShader.frag -o phong/PhongFrag.spv
echo "Done."

# Compilazione degli shader Cook-Torrance
echo "Compiling Cook-Torrance vertex shader..."
glslc cook_torrance/CookTorranceShader.vert -o cook_torrance/CookTorranceVert.spv
echo "Done."
echo "Compiling Cook-Torrance fragment shader..."
glslc cook_torrance/CookTorranceShader.frag -o cook_torrance/CookTorranceFrag.spv
echo "Done."

# Compilazione degli shader Toon
echo "Compiling Toon vertex shader..."
glslc toon/ToonShader.vert -o toon/ToonVert.spv
echo "Done."
echo "Compiling Toon fragment shader..."
glslc toon/ToonShader.frag -o toon/ToonFrag.spv
echo "Done."

# Compilazione degli shader per il testo
echo "Compiling Text vertex shader..."
glslc text/TextShader.vert -o text/TextVert.spv
echo "Done."
echo "Compiling Text fragment shader..."
glslc text/TextShader.frag -o text/TextFrag.spv
echo "Done."

echo "Shaders compilation completed."
