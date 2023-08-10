#include <GL/glew.h>
#include <glm/ext.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/common.hpp>
#include <vector>

#include "cube.h"
#include "ground.h"
#include "textureLoader.h"

using namespace glm;
using namespace std;

void setGroundWorldMatrix(int shaderProgram, glm::mat4 worldMatrix)
{
    glUseProgram(shaderProgram);
    GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
}


void renderGround(int shaderProgram, int vao)
{
    // Load Textures
    #if defined(__APPLE__)
        GLuint cementTextureID = loadTexture("assets/Textures/cement.jpg");
    #else
        GLuint cementTextureID = loadTexture("../Assets/Textures/cement.jpg");
    #endif
        
    GLint colLocation = glGetUniformLocation(shaderProgram, "col");

    glBindVertexArray(vao);
    glBindTexture(GL_TEXTURE_2D, cementTextureID);
	glUniform3f(colLocation, 1.0f, 1.0f, 1.0); //beige

    // draw
    glBindTexture(GL_TEXTURE_2D, cementTextureID);
    glm::mat4 groundWorldMatrix = translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.01f, 0.0f)) * scale(glm::mat4(1.0f), glm::vec3(1000.0f, 0.02f, 1000.0f));
    setGroundWorldMatrix(shaderProgram, groundWorldMatrix);
    glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
}