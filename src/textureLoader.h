#ifndef TEXTURELOADER_H
#define TEXTURELOADER_H

#include <GL/glew.h>// GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/glm.hpp>  
#include <vector>

GLuint loadTexture(const char *filename);

#endif // TEXTURELOADER_H