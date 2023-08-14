//COMP371 PROJECT (WALKING AROUND FOREST)
#include <iostream>
#include <list>
#include <algorithm>
#include <vector>
#include <string>
#include <glm/gtc/type_ptr.hpp>
#include <random>
#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler
#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
                        // initializing OpenGL and binding inputs
#include <glm/ext.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/common.hpp>



#include "textureLoader.h"
#include "cube.h"
#include "skyBox.h"
#include "ground.h"
#include "tree.h"
#include "chunk.h"

using namespace glm;
using namespace std;



// Create methods to make it easier to call instructions
void SetUniformMat4(GLuint shader_id, const char* uniform_name,
	mat4 uniform_value) {
	glUseProgram(shader_id);
	glUniformMatrix4fv(glGetUniformLocation(shader_id, uniform_name), 1, GL_FALSE,
		&uniform_value[0][0]);
}
void SetUniformVec3(GLuint shader_id, const char* uniform_name,
	vec3 uniform_value) {
	glUseProgram(shader_id);
	glUniform3fv(glGetUniformLocation(shader_id, uniform_name), 1,
		value_ptr(uniform_value));
}
template <class T>
void SetUniform1Value(GLuint shader_id, const char* uniform_name,
	T uniform_value) {
	glUseProgram(shader_id);
	glUniform1i(glGetUniformLocation(shader_id, uniform_name), uniform_value);
	glUseProgram(0);
}


GLuint loadTexture(const char *filename);

const char* getTexturedVertexShaderSource();

const char* getTexturedFragmentShaderSource();

int compileAndLinkShaders(const char* vertexShaderSource, const char* fragmentShaderSource);

void setProjectionMatrix(int shaderProgram, mat4 projectionMatrix)
{
    glUseProgram(shaderProgram);
    GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
}

void setViewMatrix(int shaderProgram, mat4 viewMatrix)
{
    glUseProgram(shaderProgram);
    GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
}

void setWorldMatrix(int shaderProgram, mat4 worldMatrix)
{
    glUseProgram(shaderProgram);
    GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
}

void setWorldTransparency(int shaderProgram, float transparent)
{
    glUseProgram(shaderProgram);
    GLuint transparencyLocation = glGetUniformLocation(shaderProgram, "is_transparent");
    glUniform1f(transparencyLocation, transparent);
}


//////////
// initializing parameters

const unsigned int DEPTH_MAP_TEXTURE_SIZE = 1024;
const int chunkSize = 40;
const int chunkHeight = 32;

// texture handling
bool LightEnabled = true;
GLfloat is_tex = 0.0f;

// Camera parameters for view transform
vec3 cameraPosition(45.0f, 20.0f, 45.0f);
vec3 cameraLookAt(0.0f, -18.0f, 0.0f);
vec3 cameraUp(0.0f, 1.0f, 0.0f);

// set heightmap
HeightMap map = initHeightMap();

// tree position map
std::vector<Position3D>  treePositions = createTreePositions(chunkSize, map);

// collision
bool checkCollision(const glm::vec3& newPosition, const std::vector<Position3D>& treePosition, HeightMap map, int CHUNK_SIZE) {
    // Check collision with the different parts of the trees
    const float treeSize = 3.0f; // Adjust as needed

    for (const Position3D& treePos : treePosition) {
        // Check collision with the trunk of the tree
        glm::vec3 treeTrunkPosition(treePos.x, treePos.y + 2.5f, treePos.z);
        glm::vec3 treeTrunkScale(1.0f, 5.0f, 1.0f);
        if (newPosition.x > treeTrunkPosition.x - treeSize && newPosition.x < treeTrunkPosition.x + treeSize &&
            newPosition.y > treeTrunkPosition.y && newPosition.y < treeTrunkPosition.y + treeTrunkScale.y &&
            newPosition.z > treeTrunkPosition.z - treeSize && newPosition.z < treeTrunkPosition.z + treeSize) {
            std::cout << "Collision detected with trunk of tree" << std::endl;
            return true; // Collision detected with tree trunk
        }

        // Check collision with the leaf base of the tree
        glm::vec3 treeLeafBasePosition(treePos.x, treePos.y + 5.5f, treePos.z);
        glm::vec3 treeLeafBaseScale(3.0f, 2.0f, 3.0f);
        if (newPosition.x > treeLeafBasePosition.x - treeSize && newPosition.x < treeLeafBasePosition.x + treeSize &&
            newPosition.y > treeLeafBasePosition.y && newPosition.y < treeLeafBasePosition.y + treeLeafBaseScale.y &&
            newPosition.z > treeLeafBasePosition.z - treeSize && newPosition.z < treeLeafBasePosition.z + treeSize) {
            std::cout << "Collision detected with leaf base of tree" << std::endl;
            return true; // Collision detected with tree leaf base
        }

        // Check collision with the leaf top of the tree
        glm::vec3 treeLeafTopPosition(treePos.x, treePos.y + 7.0f, treePos.z);
        glm::vec3 treeLeafTopScale(1.5f, 1.0f, 1.5f);
        if (newPosition.x > treeLeafTopPosition.x - treeSize && newPosition.x < treeLeafTopPosition.x + treeSize &&
            newPosition.y > treeLeafTopPosition.y && newPosition.y < treeLeafTopPosition.y + treeLeafTopScale.y &&
            newPosition.z > treeLeafTopPosition.z - treeSize && newPosition.z < treeLeafTopPosition.z + treeSize) {
            std::cout << "Collision detected with leaf top of tree" << std::endl;
            return true; // Collision detected with tree leaf top
        }
    }

    // Check collision with ground
    int x = static_cast<int>(newPosition.x);
    int z = static_cast<int>(newPosition.z);
    if (x >= 0 && x < chunkSize && z >= 0 && z < chunkSize) {
        int columnHeight = static_cast<int>(map.heightmap[x][z]);
        if (newPosition.y < columnHeight) {
            std::cout << "Collision detected with ground" << std::endl;
            return true; // Collision with ground
        }
    }

    return false; // No collision
}

int main(int argc, char*argv[])
{
    // Initialize GLFW and OpenGL version
    glfwInit();

    #if defined(__APPLE__)
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #else
        // On windows, we set OpenGL version to 2.1, to support more hardware
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    #endif

    // Create Window and rendering context using GLFW, resolution is 800x600
    GLFWwindow* window = glfwCreateWindow(1024, 768, "Walk Through a Procedurelly Created World", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwMakeContextCurrent(window);
   
    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to create GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }

   
    // Load Textures
    #if defined(__APPLE__)
        GLuint cementTextureID = loadTexture("assets/Textures/cement.jpg");
        GLuint SkyTextureID = loadTexture("assets/Textures/sky.jpg");
    #else
        GLuint cementTextureID = loadTexture("../Assets/Textures/cement.jpg");
        GLuint SkyTextureID = loadTexture("../Assets/Textures/sky.jpg");
    #endif

    // Black background
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    
    // Compile and link shaders here ...
    int texturedShaderProgram = compileAndLinkShaders(getTexturedVertexShaderSource(), getTexturedFragmentShaderSource());
      
    // Other camera parameters
    float cameraSpeed = 10.0f;
    float cameraFastSpeed = 2 * cameraSpeed;
    float cameraHorizontalAngle = 135.0f;
    float cameraVerticalAngle = 0.0f;

    
    // Set projection matrix for shader, this won't change
    mat4 projectionMatrix = glm::perspective(70.0f,            // field of view in degrees
                                             800.0f / 600.0f,  // aspect ratio
                                             0.01f, 10000.0f);   // near and far (near > 0)
    
    // Set initial view matrix
    mat4 viewMatrix = lookAt(cameraPosition,  // eye
                             cameraPosition + cameraLookAt,  // center
                             cameraUp ); // up
    
    // Set View and Projection matrices 
    setViewMatrix(texturedShaderProgram, viewMatrix);
    setProjectionMatrix(texturedShaderProgram, projectionMatrix);

    // Define and upload geometry to the GPU here ...
    int texturedCubeVAO = createTexturedCubeVertexArrayObject();

	// Set projection matrix on both shaders
	SetUniformMat4(texturedShaderProgram, "projection_matrix", projectionMatrix);

	// Set view matrix on both shaders
	SetUniformMat4(texturedShaderProgram, "view_matrix", viewMatrix);

    // set transparency
    setWorldTransparency(texturedShaderProgram, 0.0f); // remove transparent 

	float lightAngleOuter = radians(30.0f);
	float lightAngleInner = radians(20.0f);
	// Set light cutoff angles on scene shader
	SetUniform1Value(texturedShaderProgram, "light_cutoff_inner", cos(lightAngleInner));
	SetUniform1Value(texturedShaderProgram, "light_cutoff_outer", cos(lightAngleOuter));

	// Set light color on scene shader
	SetUniformVec3(texturedShaderProgram, "light_color", vec3(1));

	// Set object color on scene shader
	SetUniformVec3(texturedShaderProgram, "object_color", vec3(1));

    // For frame time
    float lastFrameTime = glfwGetTime();
    int lastMouseLeftState = GLFW_RELEASE;
    double lastMousePosX, lastMousePosY;
    glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);
    
    // Other OpenGL states to set once
    // Enable Backface culling
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glBindVertexArray(texturedCubeVAO);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

    bool is_mouse_moved = true; 
    // Entering Main Loop

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    while(!glfwWindowShouldClose(window))
    {
        // Frame time calculation
        float dt = glfwGetTime() - lastFrameTime;
        lastFrameTime += dt;
        	
		// light parameters
		vec3 lightPosition = vec3(0.0f, 25.0f, 0.0f); 

		vec3 lightFocus(0, 0, -1);  // the point in 3D space the light "looks" at
		vec3 lightDirection = normalize(lightFocus - lightPosition);

		float lightNearPlane = 0.01f;
		float lightFarPlane = 400.0f;

		mat4 lightProjMatrix = //frustum(-1.0f, 1.0f, -1.0f, 1.0f, lightNearPlane, lightFarPlane);
			perspective(50.0f, (float)DEPTH_MAP_TEXTURE_SIZE / (float)DEPTH_MAP_TEXTURE_SIZE, lightNearPlane, lightFarPlane);
		mat4 lightViewMatrix = lookAt(lightPosition, lightFocus, vec3(0, 1, 0));
		
		SetUniformMat4(texturedShaderProgram, "light_proj_view_matrix", lightProjMatrix * lightViewMatrix);
		SetUniform1Value(texturedShaderProgram, "light_near_plane", lightNearPlane);
		SetUniform1Value(texturedShaderProgram, "light_far_plane", lightFarPlane);
		SetUniformVec3(texturedShaderProgram, "light_position", lightPosition);
		SetUniformVec3(texturedShaderProgram, "light_direction", lightDirection);
		
        // Each frame, reset color of each pixel to glClearColor
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // textured geometry
        glUseProgram(texturedShaderProgram);

        glActiveTexture(GL_TEXTURE0);
        GLuint textureLocation = glGetUniformLocation(texturedShaderProgram, "textureSampler");
        glUniform1i(textureLocation, 0);                // Set our Texture sampler to user Texture Unit 0


        SetUniformMat4(texturedShaderProgram, "transform_in_light_space", lightProjMatrix * lightViewMatrix);

		// turning texture on and off(needs to be before rendering section)
		if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		{
			LightEnabled = !LightEnabled;
			if (LightEnabled == true)
			{
				is_tex = 0.0f;
			}
			else
			{
				is_tex = 1.0f;
			}
			glUniform1f(glGetUniformLocation(texturedShaderProgram, "is_tex"), is_tex);
		}
        //links color to shader
		GLint colLocation = glGetUniformLocation(texturedShaderProgram, "col");
		glUseProgram(texturedShaderProgram);

        //============================================= DRAW OBJECTS =============================================================

        renderSkyBox(SkyTextureID, texturedShaderProgram, texturedCubeVAO );
        //renderGround(texturedShaderProgram, texturedCubeVAO);
        renderChunk(texturedShaderProgram, texturedCubeVAO, chunkSize, chunkHeight, map);
        renderTrees(texturedShaderProgram, texturedCubeVAO, chunkSize, map, treePositions);

        
        // End Frame
        glfwSwapBuffers(window);
        glfwPollEvents();
        
        //========================================== HANDLE INPUTS =============================================================================
				
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
	
        // This was solution for Lab02 - Moving camera exercise
        // We'll change this to be a first or third person camera
        bool fastCam = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
        float currentCameraSpeed = (fastCam) ? cameraFastSpeed : cameraSpeed;
        
        
        // - Calculate mouse motion dx and dy
        // - Update camera horizontal and vertical angle
        vec3 cameraSideVector; 
        if (is_mouse_moved) {
            double mousePosX, mousePosY;
            glfwGetCursorPos(window, &mousePosX, &mousePosY);

            double dx = mousePosX - lastMousePosX;
            double dy = mousePosY - lastMousePosY;
            lastMousePosX = mousePosX;
            lastMousePosY = mousePosY;
            // Convert to spherical coordinates
            const float cameraAngularSpeed = 60.0f;
            cameraHorizontalAngle -= dx * cameraAngularSpeed * dt;
            cameraVerticalAngle -= dy * cameraAngularSpeed * dt;

            // Clamp vertical angle to [-85, 85] degrees
            cameraVerticalAngle = std::max(-85.0f, std::min(85.0f, cameraVerticalAngle));

            float theta = radians(cameraHorizontalAngle);
            float phi = radians(cameraVerticalAngle);

            //std::cout << theta << " " << phi << std::endl;

            cameraLookAt = vec3(cosf(phi) * cosf(theta), sinf(phi), -cosf(phi) * sinf(theta));
            cameraSideVector = glm::cross(cameraLookAt, vec3(0.0f, 1.0f, 0.0f));
            glm::normalize(cameraSideVector);
        }
        else {
            cameraLookAt = vec3(-1.0f, 0.0f, 0.0f);
            cameraSideVector = glm::cross(cameraLookAt, vec3(0.0f, 1.0f, 0.0f));
            glm::normalize(cameraSideVector);
        }
        
        
        // Use camera lookat and side vectors to update positions with ASDW
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // move camera to the left
        {
            glm::vec3 newCameraPosition = cameraPosition - cameraSideVector * currentCameraSpeed * dt;

            if (!checkCollision(newCameraPosition, treePositions, map, chunkSize))  // Implement this function to check for collisions
                cameraPosition = newCameraPosition;
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // move camera to the right
        {
            glm::vec3 newCameraPosition = cameraPosition + cameraSideVector * currentCameraSpeed * dt;

            if (!checkCollision(newCameraPosition, treePositions, map, chunkSize))  // Implement this function to check for collisions
                cameraPosition = newCameraPosition;
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) // move camera backward
        {
            glm::vec3 newCameraPosition = cameraPosition - cameraLookAt * currentCameraSpeed * dt;

            if (!checkCollision(newCameraPosition, treePositions, map, chunkSize))  // Implement this function to check for collisions
                cameraPosition = newCameraPosition;
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) // move camera forward
        {
            glm::vec3 newCameraPosition = cameraPosition + cameraLookAt * currentCameraSpeed * dt;

            if (!checkCollision(newCameraPosition, treePositions, map, chunkSize))  // Implement this function to check for collisions
                cameraPosition = newCameraPosition;
        }
        mat4 viewMatrix =  lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);
        setViewMatrix(texturedShaderProgram, viewMatrix);
     
        
    }

    glfwTerminate();
    
	return 0;
}



const char* getTexturedVertexShaderSource() // use this shader as the vertex shader
{
    // For now, you use a string for your shader code, in the assignment, shaders will be stored in .glsl files
    return
                "#version 330 core\n"
                "layout (location = 0) in vec3 aPos;"
                "layout (location = 1) in vec3 aColor;"
                "layout (location = 2) in vec2 aUV;"
                "layout(location = 3) in vec3 normals;"
                ""
                "uniform mat4 worldMatrix;"
                "uniform mat4 viewMatrix = mat4(1.0);" 
                "uniform mat4 projectionMatrix = mat4(1.0);"

                "out vec3 fragment_normal;"
                "out vec3 fragment_position;"
                "out vec4 fragment_position_light_space;"
                ""
                "out vec3 vertexColor;"
                "out vec2 vertexUV;"
                ""
                "void main()"
                "{"
                    "fragment_normal = inverse(transpose(mat3(worldMatrix))) * normals;"
                "   fragment_position = vec3(worldMatrix * vec4(aPos, 1.0));"
                "   vertexColor = aColor;"
                "   mat4 modelViewProjection = projectionMatrix * viewMatrix * worldMatrix;"
                "   gl_Position = modelViewProjection * vec4(aPos.x, aPos.y, aPos.z, 1.0);"
                "   vertexUV = aUV;"
                "}";
}

const char* getTexturedFragmentShaderSource() //use this shader as the fragment shader
{
	return
		"#version 330 core\n"
		"in vec3 vertexColor;"
		"in vec2 vertexUV;"
		"uniform sampler2D textureSampler;"
		"uniform float is_tex;"
		"out vec4 FragColor;"
		"uniform vec3 col;"
		
		"uniform vec3 light_position;"
		"uniform vec3 light_direction;"
		"uniform vec3 object_color;"

		"const float shading_ambient_strength = 0.6;"
		"const float shading_diffuse_strength = 0.6;"
		"const float shading_specular_strength = 0.5;"

		"float light_cutoff_outer;"
		"uniform float light_cutoff_inner;"
		"uniform float light_near_plane;"
		"uniform float light_far_plane;"
		"uniform float is_transparent = 0.0f; "

		"uniform vec3 view_position;"
		"uniform sampler2D shadow_map;"
		"in vec3 fragment_position;"
		"in vec4 fragment_position_light_space;"
		"in vec3 fragment_normal;"
		"in vec4 gl_FragCoord;"
		"out vec4 result;"

		"vec3 ambient_color(vec3 light_color_arg) {"
		"return shading_ambient_strength * light_color_arg;"
		"}"
		"vec3 diffuse_color(vec3 light_color_arg, vec3 light_position_arg) {"
		"vec3 light_direction = normalize(light_position - fragment_position);"
		"return shading_diffuse_strength * light_color_arg * max(dot(normalize(fragment_normal), light_direction), 0.0f);"
		"}"
		"vec3 specular_color(vec3 light_color_arg, vec3 light_position_arg) {"
		"vec3 light_direction = normalize(light_position_arg - fragment_position);"
		"vec3 view_direction = normalize(view_position - fragment_position);"
		"vec3 reflect_light_direction = reflect(-light_direction, normalize(fragment_normal));"
		"return shading_specular_strength * light_color_arg * pow(max(dot(reflect_light_direction, view_direction), 0.0f), 32);"
		"}"


		"void main()"
		"{"
		"	vec3 light_color = vec3(1.0f, 1.0f, 1.0f);"
		"   vec3 ambient = vec3(0.0f);"
		"   vec3 diffuse = vec3(0.0f);"
		"   vec3 specular = vec3(0.0f);"
		"   ambient = ambient_color(light_color);"
		"   diffuse = diffuse_color(light_color, light_position);"
		"   specular = specular_color(light_color, light_position);"


		"   vec3 final_color = col * (ambient + diffuse + specular); "
		"   vec3 final_color_no_light = col;   "
		"   vec4 textureColor = texture(textureSampler, vertexUV);"
		"   if (is_tex == 0.0) {"
		"       FragColor = textureColor * vec4(final_color.r, final_color.g, final_color.b, 1.0f);"
		"   } else if (is_tex == 1.0) {"
		"       FragColor = textureColor * vec4(final_color_no_light.r, final_color_no_light.g, final_color_no_light.b, 1.0f);"
		"   }"
        "   if (is_transparent == 1.0f) FragColor.w =0.2f;"
        "       else FragColor.w = 1.0f;"
		
		"}";
}


int compileAndLinkShaders(const char* vertexShaderSource, const char* fragmentShaderSource)
{
    // compile and link shader program
    // return shader program id
    // ------------------------------------

    // vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    // link shaders
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return shaderProgram;
}

