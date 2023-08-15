#ifndef WATER_H
#define WATER_H

#include <GL/glew.h>// GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/glm.hpp>  
#include <vector>

#include "textureLoader.h"

struct Square {
    int x, z;
    Square(int x, int z) : x(x), z(z) {}

    bool operator==(const Square& other) const {
        return x == other.x && z == other.z;
    }
};

bool isValidPosition(int x, int z, int SIZE);

int randomInt(int min, int max);

std::vector<Square> createWater(int SIZE, HeightMap& heightmap);

void renderWater(int shaderProgram, int texture, int vao, int SIZE, HeightMap map, std::vector<Square> waterPositions);


#endif // WATER_H