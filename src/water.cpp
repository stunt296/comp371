#include <GL/glew.h>
#include <glm/ext.hpp>                  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/glm.hpp>                  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/common.hpp>
#include <vector>
#include <queue>
#include <unordered_set>
#include <iostream>
#include <random>
#include <algorithm>

#include "cube.h"
#include "ground.h"
#include "textureLoader.h"
#include "tree.h"
#include "chunk.h"
#include "water.h"

// Function to check if a given position is within the heightmap bounds
bool isValidPosition(int x, int z, int SIZE)
{
    return x >= 0 && x < SIZE && z >= 0 && z < SIZE;
}

int randomInt(int min, int max) 
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(gen);
}

std::vector<Square> createWater(int SIZE, HeightMap& map)
{
    // Generate a random starting position
    int startX;
    int startZ;
    // select random size of water pool
    int waterSize = 0;
    // number of water pools
    int numOfPools = randomInt(3,8);
    int waterLevel = findSmallestValue(map.heightmap, SIZE, SIZE) + 4;

    // create vector that holds water
    std::vector<Square> waterSet;
    // create queue
    std::unordered_set<int> visitedIndices;
    std::queue<int> bfsQueue;

    for (int i = 0; i < numOfPools; i++)
    {
        // set the vars for the pool
        startX = randomInt(1, (SIZE - 1));
        startZ = randomInt(1, (SIZE - 1));
        waterSize += randomInt(100,300);
        // reset the queues and sets
        std::queue<int>().swap(bfsQueue);
        bfsQueue.push(startX * SIZE + startZ); // Convert 2D index to 1D index for performance reasons
        // loop through queue
        while (!bfsQueue.empty() && waterSet.size() < (waterSize)) // while queue not empty & havent reached desired pool size
        {
            int currentIndex = bfsQueue.front();
            bfsQueue.pop();

            int currentX = currentIndex / SIZE;
            int currentZ = currentIndex % SIZE;

            // if the current index has already been used, it skips the rest of this iteration of the while loop
            if (visitedIndices.find(currentIndex) != visitedIndices.end())
            {
                continue;
            }
            visitedIndices.insert(currentIndex);            // else, add index to visited indices
            waterSet.push_back(Square(currentX, currentZ)); // add index to water set

            // Check neighbors (up, down, left, right)
            int dx[] = {0, 0, -1, 1};
            int dz[] = {-1, 1, 0, 0};
            // and adds any neighbor that could be used in the water formation
            for (int i = 0; i < 4; ++i)
            {
                int newX = currentX + dx[i];
                int newZ = currentZ + dz[i];

                int newIndex = newX * SIZE + newZ;

                if (isValidPosition(newX, newZ, SIZE) && visitedIndices.find(newIndex) == visitedIndices.end())
                {
                    bfsQueue.push(newIndex);
                }
            }
        }
    }

    // edit heightmap to increase all heights by 4
    // Loop through the entire heightmap
    for (int x = 0; x < SIZE; ++x)
    {
        for (int z = 0; z < SIZE; ++z)
        {
            // Create a Square object to represent the current position in heightmap
            Square currentSquare(x, z);

            // Check if the current square is not in the waterSet
            if (std::find(waterSet.begin(), waterSet.end(), currentSquare) == waterSet.end())
            {
                // Update the height value by adding 4 units
                map.heightmap[x][z] += 5.0f;
            }
            // if it is in waterSet, 
            else if (static_cast<int>(map.heightmap[x][z]) > waterLevel)
                map.heightmap[x][z] = (waterLevel - 1); 
        }
    }
    

    return waterSet;

}

void renderWater(int shaderProgram, int texture, int vao, int SIZE, HeightMap map, std::vector<Square> waterPositions)
{
    glBindVertexArray(vao);
    GLint colLocation = glGetUniformLocation(shaderProgram, "col");
    glBindTexture(GL_TEXTURE_2D, texture);
    Faces currentFace = TOP;
    glUniform3f(colLocation, 0.11f, 0.58, 0.88f); // blue
    setTransparency(shaderProgram, 1.0f); // make transparent
    int waterLevel = findSmallestValue(map.heightmap, SIZE, SIZE) + 4;
    
    for (const Square &selectedSquare : waterPositions)
    {
        int selectedX = selectedSquare.x;
        int selectedZ = selectedSquare.z;

        drawFace(currentFace, true, selectedX, waterLevel, selectedZ, shaderProgram);
        
    }
    setTransparency(shaderProgram, 0.0f); // remove transparent

}