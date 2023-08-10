#include <GL/glew.h>
#include <glm/ext.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/common.hpp>
#include <vector>

#include "cube.h"
#include "skyBox.h"

using namespace glm;
using namespace std;

void setSkyWorldMatrix(int shaderProgram, glm::mat4 worldMatrix)
{
    glUseProgram(shaderProgram);
    GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
}


void renderSkyBox(GLuint SkyTextureID,  int shaderProgram, int vao)
{
        GLint colLocation = glGetUniformLocation(shaderProgram, "col");

        glBindVertexArray(vao);
        glBindTexture(GL_TEXTURE_2D, SkyTextureID);
        glUniform3f(colLocation, 0.43f, 0.79f, 0.95f); //blue

        // TOP cube
        glm::mat4 worldMatrix = translate(glm::mat4(1.0f), glm::vec3(0.0f, 999.0f, 0.0f)) * scale(glm::mat4(1.0f), glm::vec3(1000.0f, 0.02f, 1000.0f));       
        setSkyWorldMatrix(shaderProgram, worldMatrix);      
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
        
        // GROUND cube
        worldMatrix = translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f)) * scale(glm::mat4(1.0f), glm::vec3(1000.0f, 0.02f, 1000.0f));
        setSkyWorldMatrix(shaderProgram, worldMatrix);
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0

        // SIDES cube - Front
        worldMatrix = translate(glm::mat4(1.0f), glm::vec3(0.0f, 499.0f, 500.0f)) * scale(glm::mat4(1.0f), glm::vec3(1000.0f, 1000.0f, 0.02f));
        setSkyWorldMatrix(shaderProgram, worldMatrix);
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
        
        // SIDES cube - Back
        worldMatrix = translate(glm::mat4(1.0f), glm::vec3(0.0f, 499.0f, -500.0f)) * scale(glm::mat4(1.0f), glm::vec3(1000.0f, 1000.0f, 0.02f));
        setSkyWorldMatrix(shaderProgram, worldMatrix);
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0

        // SIDES cube - Left
        worldMatrix = translate(glm::mat4(1.0f), glm::vec3(-500.0f, 499.0f, 0.0f)) * scale(glm::mat4(1.0f), glm::vec3(0.02f, 1000.0f, 1000.0f));
        setSkyWorldMatrix(shaderProgram, worldMatrix);
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0

        // SIDES cube - Right
        worldMatrix= translate(glm::mat4(1.0f), glm::vec3(500.0f, 499.0f, 0.0f)) * scale(glm::mat4(1.0f), glm::vec3(0.02f, 1000.0f, 1000.0f));
        setSkyWorldMatrix(shaderProgram, worldMatrix);
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
}