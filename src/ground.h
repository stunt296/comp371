#ifndef GROUND_H
#define GROUND_H

#include <GL/glew.h>// GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/glm.hpp>  
#include <vector>

void setGroundWorldMatrix(int shaderProgram, glm::mat4 worldMatrix);

void renderGround(int shaderProgram, int vao);

#endif // GROUND_H