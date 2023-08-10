#ifndef SKYBOX_H
#define SKYBOX_H

#include <GL/glew.h>// GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/glm.hpp>  
#include <vector>

void setWorldMatrix(int shaderProgram, glm::mat4 worldMatrix);

void renderSkyBox(GLuint SkyTextureID,  int shaderProgram, int vao);

#endif // SKYBOX_H