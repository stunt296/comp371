#include <GL/glew.h>
#include <glm/ext.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/common.hpp>
#include <vector>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace std;

#include "textureLoader.h"



float** generateHeightmap(std::string filename, int& width, int& height) {
    
        // Load image
    #if defined(__APPLE__)
        std::string folderPath = "assets/Textures/";
    #else
      std::string folderPath = "../Assets/Textures/";
    #endif

    std::string imagePath = folderPath + filename;

    // Load the image using stb_image
    int channels;
    unsigned char* imageData = stbi_load(imagePath.c_str(), &width, &height, &channels, 1);

    // Create the 2D array to store height values
    float** heightmap = new float*[width];
    for (int i = 0; i < width; ++i) {
        heightmap[i] = new float[height];
    }

    // Define the desired height range
    const float minHeight = 0.0f;
    const float maxHeight = 20.0f;

    // Process imageData and populate the heightmap array
    for (int x = 0; x < width; ++x) {
        for (int z = 0; z < height; ++z) {
            // Calculate grayscale value and map to height range
            unsigned char grayscaleValue = imageData[x * height + z];
            float normalizedHeight = static_cast<float>(grayscaleValue) / 255.0f;
            float height = minHeight + normalizedHeight * (maxHeight - minHeight); // Map to desired range

            // Store height value in heightmap
            heightmap[x][z] = height;
        }
    }

    // Clean up the loaded image data
    stbi_image_free(imageData);

    // Return the generated heightmap array
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
 