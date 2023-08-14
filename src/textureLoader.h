#ifndef TEXTURELOADER_H
#define TEXTURELOADER_H

#include <GL/glew.h>// GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/glm.hpp>  
#include <vector>
#include <string>

using namespace std;

struct HeightMap {
    float** heightmap;
    int width;
    int height;
};

float** generateHeightmap(int& width, int& height, int octaves, float persistence, float scale);

GLuint loadTexture(const char *filename);

#endif // TEXTURELOADER_H