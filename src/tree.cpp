#include <GL/glew.h>
#include <glm/ext.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/common.hpp>
#include <vector>
#include <iostream>
#include <random>

#include "cube.h"
#include "ground.h"
#include "textureLoader.h"
#include "tree.h"
#include "chunk.h"
#include "water.h"

using namespace glm;
using namespace std;

int treeCounter = 0;

int getTreeCount() {
    return treeCounter;
}

void setTreeWorldMatrix(int shaderProgram, glm::mat4 worldMatrix)
{
    glUseProgram(shaderProgram);
    GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
}

void drawBasicTree(int shaderProgram, int x, int y, int z, GLint colLocation, Position3D pos)
{
    // trunk
    glm::mat4 treeWorldMatrix = translate(glm::mat4(1.0f), glm::vec3(x, y+2.5f, z)) * scale(glm::mat4(0.5f), glm::vec3(1.0f, 5.0f, 1.0f));
    setTreeWorldMatrix(shaderProgram, treeWorldMatrix);
    glUniform3f(colLocation, pos.brown.x,pos.brown.y,pos.brown.z);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // leaf base
    treeWorldMatrix = translate(glm::mat4(1.0f), glm::vec3(x, y+5.5f, z)) * scale(glm::mat4(0.5f), glm::vec3(pos.scalar, 2.0f, pos.scalar));
    setTreeWorldMatrix(shaderProgram, treeWorldMatrix);
    glUniform3f(colLocation, pos.green.x, pos.green.y, pos.green.z); 
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // leaf top
    treeWorldMatrix = translate(glm::mat4(1.0f), glm::vec3(x, y+7.0f, z)) * scale(glm::mat4(0.5f), glm::vec3(pos.scalar/2, 1.0f, pos.scalar/2));
    setTreeWorldMatrix(shaderProgram, treeWorldMatrix);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void drawDarkTree(int shaderProgram, int x, int y, int z, GLint colLocation, Position3D pos)
{
    // trunk
    glm::mat4 treeWorldMatrix = translate(glm::mat4(1.0f), glm::vec3(x, y+3.0f, z)) * scale(glm::mat4(0.5f), glm::vec3(1.0f, 6.0f, 1.0f));
    setTreeWorldMatrix(shaderProgram, treeWorldMatrix);
    glUniform3f(colLocation, pos.brown.x,pos.brown.y,pos.brown.z); 
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // leaf base
    treeWorldMatrix = translate(glm::mat4(1.0f), glm::vec3(x, y+6.0f, z)) * scale(glm::mat4(0.5f), glm::vec3(pos.scalar, 2.0f, pos.scalar));
    setTreeWorldMatrix(shaderProgram, treeWorldMatrix);
    glUniform3f(colLocation, pos.green.x, pos.green.y, pos.green.z); 
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // leaf top
    treeWorldMatrix = translate(glm::mat4(1.0f), glm::vec3(x, y+8.0f, z)) * scale(glm::mat4(0.5f), glm::vec3(pos.scalar/2, 2.0f, pos.scalar/2));
    setTreeWorldMatrix(shaderProgram, treeWorldMatrix);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void drawLightTree(int shaderProgram, int x, int y, int z, GLint colLocation, Position3D pos)
{
    // trunk
    glm::mat4 treeWorldMatrix = translate(glm::mat4(1.0f), glm::vec3(x, y+1.5f, z)) * scale(glm::mat4(0.5f), glm::vec3(1.0f, 3.0f, 1.0f));
    setTreeWorldMatrix(shaderProgram, treeWorldMatrix);
    glUniform3f(colLocation, pos.brown.x,pos.brown.y,pos.brown.z); 
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // leaf base
    treeWorldMatrix = translate(glm::mat4(1.0f), glm::vec3(x, y+4.5f, z)) * scale(glm::mat4(0.5f), glm::vec3(pos.scalar, 4.0f, pos.scalar));
    setTreeWorldMatrix(shaderProgram, treeWorldMatrix);
    glUniform3f(colLocation, pos.green.x, pos.green.y, pos.green.z); 
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

float randomFloat(float min, float max) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distribution(min, max);
    return distribution(gen);
}


bool isTreeCollision(int x, int z, std::vector<Position3D> treePosition) {
    int treeSize = 6;
    for (const Position3D& position : treePosition) {
        if (x >= position.x - treeSize && x <= position.x + treeSize &&
            z >= position.z - treeSize && z <= position.z + treeSize) {
            return true;
        }
    }
    return false;
}

std::vector<Position3D> createTreePositions(int SIZE, HeightMap map)
{
    // initialize heightmap
    float** heightmap = map.heightmap;
    const int waterLevel = findSmallestValue(heightmap, SIZE, SIZE) + 4;

    // inti treePos
    std::vector<Position3D> treePosition;

    for(int x = 0; x < SIZE; x++)
    {
        for(int z = 0; z < SIZE; z++)
        {
            int columnHeight = static_cast<int>(heightmap[x][z]);
            for(int y = 0; y < columnHeight; y++)
            {
                
                //  if above water, randomly place DARK trees with 1% chance, make sure it doesnt collide with other trees
                if (y >= waterLevel && randomFloat(0, 100) <= 2 && !isTreeCollision(x, z, treePosition)) 
                {
                    Position3D pos;
                    pos.x = x;
                    pos.y = y;
                    pos.z = z;
                    pos.tree = static_cast<TreeType>(randomInt(0,2));
                    pos.brown = BrownValues[randomInt(0,2)];
                    pos.green = GreenValues[randomInt(0,2)];
                    pos.scalar = randomFloat(1.5, 6);
                    treePosition.push_back(pos); // add tree to position vecto
                    treeCounter++; // add to number of trees
                }
                
            }
        }
    }
    return treePosition;
}

void renderTrees(int shaderProgram, int vao, int SIZE, HeightMap map, std::vector<Position3D> treePosition)
{
    //initialize vao and shaders
    glBindVertexArray(vao);
    GLint colLocation = glGetUniformLocation(shaderProgram, "col");

    // Load Textures
    #if defined(__APPLE__)
        GLuint cementTextureID = loadTexture("assets/Textures/cement.jpg");
    #else
        GLuint cementTextureID = loadTexture("../Assets/Textures/cement.jpg");
    #endif
    glBindTexture(GL_TEXTURE_2D, cementTextureID);


    // draw the trees
    for (const Position3D& position : treePosition) {
        if (position.tree == BASIC)
            drawBasicTree(shaderProgram, position.x, position.y, position.z,  colLocation, position);
        if (position.tree == DARK)
            drawDarkTree(shaderProgram, position.x, position.y, position.z,  colLocation, position);
        if (position.tree == LIGHT)
            drawLightTree(shaderProgram, position.x, position.y, position.z,  colLocation, position);
    }
    

}
