#include <GL/glew.h>
#include <glm/ext.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/common.hpp>
#include <vector>

#include "cube.h"
#include "ground.h"
#include "textureLoader.h"
#include "tree.h"

void setTreeWorldMatrix(int shaderProgram, glm::mat4 worldMatrix)
{
    glUseProgram(shaderProgram);
    GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
}

void renderTree(int shaderProgram, int vao)
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

    // draw
    glBindTexture(GL_TEXTURE_2D, cementTextureID);

    	//Draws TREE TRUNK
		glBindTexture(GL_TEXTURE_2D, cementTextureID);
        for (int i = 0; i < 25; ++i) {
            for (int j = 0; j < 25; ++j) {
                glm::mat4 pillarWorldMatrix_trunk = translate(glm::mat4(1.0f), glm::vec3(-350.0f + j * 35.0f, 7.5f, -350.0f + i * 35.0f)) * scale(glm::mat4(0.5f), glm::vec3(1.5f, 15.0f, 1.5f));
                setTreeWorldMatrix(shaderProgram, pillarWorldMatrix_trunk);
                glUniform3f(colLocation, 0.8039f, 0.5216f, 0.2471f); //brown 205,133,63
                glDrawArrays(GL_TRIANGLES, 0, 36);               
            }
        }

        // Drams TREE LEAVES
        glBindTexture(GL_TEXTURE_2D, cementTextureID);
        for (int i = 0; i < 25; ++i) {
            for (int j = 0; j < 25; ++j) {
                glm::mat4 pillarWorldMatrix_layer1 = translate(glm::mat4(1.0f), glm::vec3(-350.0f + j * 35.0f, 15.50f, -350.0f + i * 35.0f)) * scale(glm::mat4(0.5f), glm::vec3(12.0f, 1.0f, 12.0f));
                setTreeWorldMatrix(shaderProgram, pillarWorldMatrix_layer1);
                glUniform3f(colLocation, 0.0f, 1.0f, 0.0f); //green
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }

        glBindTexture(GL_TEXTURE_2D, cementTextureID);
        for (int i = 0; i < 25; ++i) {
            for (int j = 0; j < 25; ++j) {
                glm::mat4 pillarWorldMatrix_layer2 = translate(glm::mat4(1.0f), glm::vec3(-350.0f + j * 35.0f, 17.0f, -350.0f + i * 35.0f)) * scale(glm::mat4(0.5f), glm::vec3(6.0f, 2.0f, 6.0f));
                setTreeWorldMatrix(shaderProgram, pillarWorldMatrix_layer2);
                glUniform3f(colLocation, 0.0f, 1.0f, 0.0f); //green
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }
}

using namespace glm;
using namespace std;