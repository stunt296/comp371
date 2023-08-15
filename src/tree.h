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
    float scalar;
    glm::vec3 brown;
    glm::vec3 green;
};

const glm::vec3 BrownValues[] = {
    glm::vec3(0.8039f, 0.5216f, 0.2471f), // Basic
    glm::vec3(0.25f, 0.01f, 0.0f),         // Dark
    glm::vec3(0.73f, 0.62f, 0.51f)        // Light
};

const glm::vec3 GreenValues[] = {
    glm::vec3(0.0f, 1.0f, 0.0f),        // Basic
    glm::vec3(0.42f, 0.56f, 0.14f),     // Dark
    glm::vec3(0.6f, 0.8f, 0.2f)         // Light
};



int getTreeCount();

void setTreeWorldMatrix(int shaderProgram, glm::mat4 worldMatrix);

void drawBasicTree(int shaderProgram, int x, int y, int z, GLint colLocation, Position3D pos);

void drawDarkTree(int shaderProgram, int x, int y, int z, GLint colLocation, Position3D pos);

void drawLightTree(int shaderProgram, int x, int y, int z, GLint colLocation, Position3D pos);

float randomFloat(float min, float max);

int randomInt(int min, int max);

bool isTreeCollision(int x, int z, std::vector<Position3D> treePosition);

std::vector<Position3D> createTreePositions(int SIZE, HeightMap map);

void renderTrees(int shaderProgram, int vao, int SIZE, HeightMap map, std::vector<Position3D> treePosition);

#endif // TREE_H