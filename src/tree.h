#ifndef TREE_H
#define TREE_H

#include <GL/glew.h>// GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/glm.hpp>  
#include <vector>

void setTreeWorldMatrix(int shaderProgram, glm::mat4 worldMatrix);

void renderTree(int shaderProgram, int vao);

#endif // TREE_H