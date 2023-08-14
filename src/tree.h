#ifndef TREE_H
#define TREE_H

#include <GL/glew.h>// GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/glm.hpp>  
#include <vector>

enum TreeType { BASIC, DARK, LIGHT};

struct Position3D {
    int x;
    int y;
    int z;
    TreeType tree;
};

int getTreeCount();

void setTreeWorldMatrix(int shaderProgram, glm::mat4 worldMatrix);

void drawBasicTree(int shaderProgram, int x, int y, int z, GLint colLocation);

void drawDarkTree(int shaderProgram, int x, int y, int z, GLint colLocation);

void drawLightTree(int shaderProgram, int x, int y, int z, GLint colLocation);

float randomFloat(float min, float max);

int randomInt(int min, int max);

bool isTreeCollision(int x, int z, std::vector<Position3D> treePosition);

std::vector<Position3D> createTreePositions(int SIZE, HeightMap map);

void renderTrees(int shaderProgram, int vao, int SIZE, HeightMap map, std::vector<Position3D> treePosition);

#endif // TREE_H