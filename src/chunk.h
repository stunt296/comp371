#ifndef CHUNK_H
#define CHUNK_H

#include <GL/glew.h>// GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/glm.hpp>  
#include <vector>

// associated number refers to the index where that face starts in the texturedCubeVertexArray[]
enum Faces{ 
    LEFT = 0, 
    BACK = 6, 
    BOTTOM = 12, 
    FRONT = 18,
    RIGHT = 24,
    TOP = 30
};


void setChunkWorldMatrix(int shaderProgram, glm::mat4 worldMatrix);

void setTransparency(int shaderProgram, float transparent);

void drawFace(Faces face, bool isWater, int x, int y, int z, int shaderProgram);

int findSmallestValue(float** heightmap, int width, int height);

void renderChunk(int shaderProgram, int vao, int SIZE, int HEIGHT);


#endif // CHUNK_H