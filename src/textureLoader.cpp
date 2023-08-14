#include <GL/glew.h>
#include <glm/ext.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/common.hpp>
#include <vector>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <glm/gtc/noise.hpp>


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "textureLoader.h"



// Generate a procedural heightmap using Perlin noise
float** generateHeightmap(int& width, int& height, int octaves, float persistence, float scale) {
    // Initialize random seed
    srand(static_cast<unsigned int>(time(nullptr)));

    // Set width and height
    width = 256;
    height = 256;

    // Create the 2D array to store height values
    float** heightmap = new float* [width];
    for (int i = 0; i < width; ++i) {
        heightmap[i] = new float[height];
    }

    // Generate procedural heightmap using Perlin noise
    for (int x = 0; x < width; ++x) {
        for (int z = 0; z < height; ++z) {
            float amplitude = 1.3f;
            float frequency = 1.2f;
            float noiseHeight = 0.0f;

            // Generate multiple octaves of Perlin noise
            for (int o = 0; o < octaves; ++o) {
                float sampleX = x / scale * frequency;
                float sampleZ = z / scale * frequency;
                float perlinValue = glm::perlin(glm::vec2(sampleX, sampleZ)) * 0.5f + 0.5f;

                noiseHeight += perlinValue * amplitude;
                amplitude *= persistence;
                frequency *= 2.0f;
            }

            // Store height value in heightmap
            heightmap[x][z] = noiseHeight;
        }
    }

    return heightmap;
}



GLuint loadTexture(const char *filename)
{
  // Step1 Create and bind textures
  GLuint textureId = 0;
  glGenTextures(1, &textureId);
  assert(textureId != 0);
 
 
  glBindTexture(GL_TEXTURE_2D, textureId);
 
  // Step2 Set filter parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 
  // Step3 Load Textures with dimension data
  int width, height, nrChannels;
  unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
  if (!data)
  {
    std::cerr << "Error::Texture could not load texture file:" << filename << std::endl;
    return 0;
  }
 
  // Step4 Upload the texture to the PU
  GLenum format = 0;
  if (nrChannels == 1)
      format = GL_RED;
  else if (nrChannels == 3)
      format = GL_RGB;
  else if (nrChannels == 4)
      format = GL_RGBA;
  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height,
               0, format, GL_UNSIGNED_BYTE, data);
 
  // Step5 Free resources
  stbi_image_free(data);
  glBindTexture(GL_TEXTURE_2D, 0);
  return textureId;
}
 