#include <GL/glew.h>
#include <glm/ext.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/common.hpp>
#include <vector>

#include <iostream>

#include "cube.h"
#include "ground.h"
#include "textureLoader.h"
#include "tree.h"
#include "chunk.h"


void setChunkWorldMatrix(int shaderProgram, glm::mat4 worldMatrix)
{
    glUseProgram(shaderProgram);
    GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
}

void setTransparency(int shaderProgram, float transparent)
{
    glUseProgram(shaderProgram);
    GLuint transparencyLocation = glGetUniformLocation(shaderProgram, "is_transparent");
    glUniform1f(transparencyLocation, transparent);
}

void drawFace(Faces face, bool isWater, int x, int y, int z, int shaderProgram) 
{
    // position the block
    float xFloat = static_cast<float>(x);
    float yFloat = static_cast<float>(y);
    float zFloat = static_cast<float>(z);
    if (isWater) yFloat -= 0.2;
    glm::mat4 blockWorldMatrix = translate(glm::mat4(1.0f), glm::vec3(xFloat, yFloat, zFloat));
    // update world matrix
    setChunkWorldMatrix(shaderProgram, blockWorldMatrix);
    // use the index defined in the enum in chunk.h
    int faceIndex = static_cast<int>(face); 
    // draw the face (6 vertices) starting at the face's index  in the texturedCubeVertexArray[]
    glDrawArrays(GL_TRIANGLES, faceIndex, 6); 
}

int findSmallestValue(float** heightmap, int width, int height) {
    float smallestValue = heightmap[0][0]; // Initialize with the first value

    for (int x = 0; x < width; ++x) {
        for (int z = 0; z < height; ++z) {
            smallestValue = std::min(smallestValue, heightmap[x][z]);
        }
    }

    int smallestVal = static_cast<int>(smallestValue);
    return smallestVal;
}


void renderChunk(int shaderProgram, int vao, int SIZE, int HEIGHT)
{
    glBindVertexArray(vao);

    // Load Textures
    #if defined(__APPLE__)
        GLuint cementTextureID = loadTexture("assets/Textures/cement.jpg");
        GLuint waterTextureID = loadTexture("assets/Textures/water.jpg");
    #else
        GLuint cementTextureID = loadTexture("../Assets/Textures/cement.jpg");
        GLuint waterTextureID = loadTexture("../Assets/Textures/water.jpg");
    #endif

    glBindTexture(GL_TEXTURE_2D, cementTextureID);

    // instantiate colorLocation
    GLint colLocation = glGetUniformLocation(shaderProgram, "col");
    glUniform3f(colLocation, 0.4f, 0.26f, 0.13f); //brown
    

    // get heightmap
    //"Heightmap2.jpg"
    //"Heightmap3.jpg"
    //"Heightmap4.jpg"
    std::string filename = "Heightmap4.jpg";
    int width, height; // this is x and z of heightmap image
    float** heightmap = generateHeightmap(filename, width, height);
    // initialize faces enum
    Faces currentFace;
    const int waterLevel = findSmallestValue(heightmap, width, height) + 1;
    bool isWater = false;


    for(int x = 0; x < SIZE; x++)
    {
        for(int z = 0; z < SIZE; z++)
        {
            
            int columnHeight = static_cast<int>(heightmap[x][z]);
            glUniform3f(colLocation, 0.76f, 0.70f, 0.5f); //beige
            // floor faces
            // qualifications: is highest in chunk
            if (columnHeight == 0)
            {
                currentFace = TOP;
                drawFace(currentFace, isWater, x, -1, z, shaderProgram);

                setTransparency(shaderProgram, 1.0f); // make transparent
                //glBindTexture(GL_TEXTURE_2D, waterTextureID);
                glUniform3f(colLocation, 0.11f, 0.58, 0.88f); //blue
                drawFace(currentFace, true, x, waterLevel, z, shaderProgram);
                setTransparency(shaderProgram, 0.0f); // remove transparent 
            }
            //glBindTexture(GL_TEXTURE_2D, cementTextureID);
            for(int y = 0; y < columnHeight; y++)
            {
                if ( y < waterLevel) glUniform3f(colLocation, 0.76f, 0.70f, 0.5f); //beige
                else glUniform3f(colLocation, 0.4f, 0.26f, 0.13f); //brown
                
                // left faces
                // qualifications: block to the left is empty, not furthest left in chunk
                if (x > 0)
                {
                    int previousColumnHeight = static_cast<int>(heightmap[x-1][z]);
                    if (y > (previousColumnHeight-1)) // aka this block is taller than block to the left
                    {
                        currentFace = LEFT;
                        drawFace(currentFace, isWater, x, y, z, shaderProgram);
                    }
                }
                // left faces
                // qualifications: block to the left is empty, not furthest left in chunk
                if (x < SIZE-1)
                {
                    int nextColumnHeight = static_cast<int>(heightmap[x+1][z]);
                    if (y > (nextColumnHeight-1)) // aka this block is taller than block to the right
                    {
                        currentFace = RIGHT;
                        drawFace(currentFace, isWater, x, y, z, shaderProgram);
                    }
                }
                 // back faces
                // qualifications: block to the back is empty, not furthest back in chunk
                if (z > 0)
                {
                    int previousColumnHeight = static_cast<int>(heightmap[x][z-1]);
                    if (y > (previousColumnHeight-1)) // aka this block is taller than block to the left
                    {
                        currentFace = BACK;
                        drawFace(currentFace, isWater, x, y, z, shaderProgram);
                    }
                }
                // front faces
                // qualifications: block to the front is empty, not furthest front in chunk
                if (z < SIZE-1)
                {
                    int nextColumnHeight = static_cast<int>(heightmap[x][z+1]);
                    if (y > (nextColumnHeight-1)) // aka this block is taller than block to the right
                    {
                        currentFace = FRONT;
                        drawFace(currentFace, isWater, x, y, z, shaderProgram);
                    }
                }
                
                if ( y < waterLevel) glUniform3f(colLocation, 0.76f, 0.70f, 0.5f); //beige
                else glUniform3f(colLocation, 0.13f, 0.55, 0.0f); //green
                // top faces
                // qualifications: is highest in chunk
                if (y == (columnHeight - 1))
                {
                    currentFace = TOP;
                    drawFace(currentFace, isWater, x, y, z, shaderProgram);
                    // basically, if the top of the stack is below water level, get water
                    if (y < waterLevel) 
                    {
                        setTransparency(shaderProgram, 1.0f); // make transparent
                        //glBindTexture(GL_TEXTURE_2D, waterTextureID);
                        glUniform3f(colLocation, 0.11f, 0.58, 0.88f); //blue
                        drawFace(currentFace, true, x, waterLevel, z, shaderProgram);
                        setTransparency(shaderProgram, 0.0f); // remove transparent 
                    }
                }
                
                
                
            }
        }
    }





   
//  clean up the heightmap array when done using it
    for (int i = 0; i < width; ++i) {
        delete[] heightmap[i];
    }
    delete[] heightmap;


}