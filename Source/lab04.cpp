//COMP371 PROJECT (WALKING AROUND FOREST)
#include <iostream>
#include <list>
#include <algorithm>
#include <vector>
#include <glm/gtc/type_ptr.hpp>


#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
                        // initializing OpenGL and binding inputs

#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/common.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace glm;
using namespace std;

class Projectile
{
public:
    Projectile(vec3 position, vec3 velocity, int shaderProgram) : mPosition(position), mVelocity(velocity)
    {
        mWorldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
    }
    
    void Update(float dt)
    {
        mPosition += mVelocity * dt;
    }
    
    void Draw() {
        // this is a bit of a shortcut, since we have a single vbo, it is already bound
        // let's just set the world matrix in the vertex shader
        
        mat4 worldMatrix = translate(mat4(1.0f), mPosition) * rotate(mat4(1.0f), radians(180.0f), vec3(0.0f, 1.0f, 0.0f)) * scale(mat4(1.0f), vec3(0.2f, 0.2f, 0.2f));
        glUniformMatrix4fv(mWorldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    
private:
    GLuint mWorldMatrixLocation;
    vec3 mPosition;
    vec3 mVelocity;
};

//lowerarm rotation function variables ============================================================
const float translationAmount = 0.1f;
const float rotateAngle = glm::radians(0.1f);

// Camera parameters for view transform
vec3 cameraPosition(0.6f, 1.0f, 10.0f);
vec3 cameraLookAt(0.0f, -1.0f, 0.0f);
vec3 cameraUp(0.0f, 1.0f, 0.0f);

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

const unsigned int DEPTH_MAP_TEXTURE_SIZE = 1024;

// texture handling
bool LightEnabled = true;
GLfloat is_tex = 0.0f;

GLuint loadTexture(const char *filename);

const char* getVertexShaderSource();

const char* getFragmentShaderSource();

const char* getTexturedVertexShaderSource();

const char* getTexturedFragmentShaderSource();

int compileAndLinkShaders(const char* vertexShaderSource, const char* fragmentShaderSource);

struct TexturedColoredVertex
{
    TexturedColoredVertex(vec3 _position, vec3 _color, vec2 _uv, vec3 _normal, float _alpha)
        : position(_position), color(_color), uv(_uv), normal(_normal), alpha(_alpha) {}

    vec3 position;
    vec3 color;
    vec2 uv;
    vec3 normal;
    float alpha;
};
// Textured Cube model
const TexturedColoredVertex texturedCubeVertexArray[] = {
    // position           color          uv           normal
    // Left - red
    
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f), 1.0f),
    TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 1.0f), vec3(-1.0f, 0.0f, 0.0f), 1.0f),
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 1.0f), vec3(-1.0f, 0.0f, 0.0f), 1.0f),
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f), 1.0f),
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 1.0f), vec3(-1.0f, 0.0f, 0.0f), 1.0f),
    TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f), 1.0f),

    // Far - blue
    TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f), 1.0f),
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f), 1.0f),
    TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f), 1.0f),
    TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f), 1.0f),
    TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f), 1.0f),
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f), 1.0f),

    // Bottom - turquoise
    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f), vec2(1.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f), 1.0f),
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f), 1.0f),
    TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f), vec2(1.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f), 1.0f),
    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f), vec2(1.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f), 1.0f),
    TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f), vec2(0.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f), 1.0f),
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f), 1.0f),

    // Near - green
     TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f), 1.0f),
     TexturedColoredVertex(vec3(-0.5f, -0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f), 1.0f),
     TexturedColoredVertex(vec3(0.5f, -0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f), 1.0f),
     TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f), 1.0f),
     TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f), 1.0f),
     TexturedColoredVertex(vec3(0.5f, -0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f), 1.0f),


     // Right - purple
     TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f), vec2(1.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f), 1.0f),
     TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f), 1.0f),
     TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f), vec2(1.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f), 1.0f),
     TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f), 1.0f),
     TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f), vec2(1.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f), 1.0f),
     TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f), vec2(0.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f), 1.0f),

     // Top - yellow
     TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f), vec2(1.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f), 1.0f),
     TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f), vec2(1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), 1.0f),
     TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), 1.0f),
     TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f), vec2(1.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f), 1.0f),
     TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), 1.0f),
     TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f), vec2(0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f), 1.0f),
};

int createTexturedCubeVertexArrayObject();

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

int main(int argc, char*argv[])
{
    // Initialize GLFW and OpenGL version
    glfwInit();

#if defined(PLATFORM_OSX)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    // On windows, we set OpenGL version to 2.1, to support more hardware
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#endif

    // Create Window and rendering context using GLFW, resolution is 800x600
    GLFWwindow* window = glfwCreateWindow(1024, 768, "assignment 2", NULL, NULL);
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

    //racket selection
    int selection = 0;

    //translation function variables
    const float translationAmount = 0.1f;

    float translation_x_one = 0.0f;
    float translation_y_one = 0.0f;
    float translation_z_one = 0.0f;

    float translation_x_two = 0.0f;
    float translation_y_two = 0.0f;
    float translation_z_two = 0.0f;
   
    // Load Textures
#if defined(PLATFORM_OSX)
    GLuint cementTextureID = loadTexture("Textures/cement.jpg");
    GLuint SkyTextureID = loadTexture("Textures/sky.jpg");
#else
    GLuint cementTextureID = loadTexture("../Assets/Textures/cement.jpg");
    GLuint SkyTextureID = loadTexture("../Assets/Textures/sky.jpg");
#endif

    // Black background
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    // Compile and link shaders here ...
    int colorShaderProgram = compileAndLinkShaders(getVertexShaderSource(), getFragmentShaderSource());
    int texturedShaderProgram = compileAndLinkShaders(getTexturedVertexShaderSource(), getTexturedFragmentShaderSource());
      
    // Other camera parameters
    float cameraSpeed = 10.0f;
    float cameraFastSpeed = 2 * cameraSpeed;
    float cameraHorizontalAngle = 90.0f;
    float cameraVerticalAngle = 0.0f;
    bool  cameraFirstPerson = true; // press 1 or 2 to toggle this variable

    // Spinning cube at camera position
    float spinningCubeAngle = 0.0f;
    
    // Set projection matrix for shader, this won't change
    mat4 projectionMatrix = glm::perspective(70.0f,            // field of view in degrees
                                             800.0f / 600.0f,  // aspect ratio
                                             0.01f, 10000.0f);   // near and far (near > 0)
    
    // Set initial view matrix
    mat4 viewMatrix = lookAt(cameraPosition,  // eye
                             cameraPosition + cameraLookAt,  // center
                             cameraUp ); // up
    
    // Set View and Projection matrices on both shaders
    setViewMatrix(colorShaderProgram, viewMatrix);
    setViewMatrix(texturedShaderProgram, viewMatrix);

    setProjectionMatrix(colorShaderProgram, projectionMatrix);
    setProjectionMatrix(texturedShaderProgram, projectionMatrix);

    // Define and upload geometry to the GPU here ...
    int texturedCubeVAO = createTexturedCubeVertexArrayObject();

	// Set projection matrix on both shaders
	SetUniformMat4(texturedShaderProgram, "projection_matrix", projectionMatrix);

	// Set view matrix on both shaders
	SetUniformMat4(texturedShaderProgram, "view_matrix", viewMatrix);

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
    
    // Container for projectiles to be implemented in tutorial
    list<Projectile> projectileList;

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
        
        // Draw textured geometry
        glUseProgram(texturedShaderProgram);

        glActiveTexture(GL_TEXTURE0);
        GLuint textureLocation = glGetUniformLocation(texturedShaderProgram, "textureSampler");
        glUniform1i(textureLocation, 0);                // Set our Texture sampler to user Texture Unit 0


        SetUniformMat4(texturedShaderProgram, "transform_in_light_space", lightProjMatrix * lightViewMatrix);

		// turning texture on and off(needs to be before drawing section)
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

        //============================================= SKY BOX =============================================================
        // TOP cube
        glBindTexture(GL_TEXTURE_2D, SkyTextureID);
        mat4 groundWorldMatrixtop = translate(mat4(1.0f), vec3(0.0f, 999.0f, 0.0f)) * scale(mat4(1.0f), vec3(1000.0f, 0.02f, 1000.0f));       
        setWorldMatrix(texturedShaderProgram, groundWorldMatrixtop);      
        glUniform3f(colLocation, 0.43f, 0.79f, 0.95f); //blue
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
        
        // GROUND cube
        mat4 groundWorldMatrixbottom = translate(mat4(1.0f), vec3(0.0f, -1.0f, 0.0f)) * scale(mat4(1.0f), vec3(1000.0f, 0.02f, 1000.0f));
        setWorldMatrix(texturedShaderProgram, groundWorldMatrixbottom);
        glUniform3f(colLocation, 0.43f, 0.79f, 0.95f); //blue
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0

        // SIDES cube - Front
        mat4 groundWorldMatrixFront = translate(mat4(1.0f), vec3(0.0f, 499.0f, 500.0f)) * scale(mat4(1.0f), vec3(1000.0f, 1000.0f, 0.02f));
        setWorldMatrix(texturedShaderProgram, groundWorldMatrixFront);
        glUniform3f(colLocation, 0.43f, 0.79f, 0.95f); //blue
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
        
        // SIDES cube - Back
        mat4 groundWorldMatrixBack = translate(mat4(1.0f), vec3(0.0f, 499.0f, -500.0f)) * scale(mat4(1.0f), vec3(1000.0f, 1000.0f, 0.02f));
        setWorldMatrix(texturedShaderProgram, groundWorldMatrixBack);
        glUniform3f(colLocation, 0.43f, 0.79f, 0.95f); //blue
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0

        // SIDES cube - Left
        mat4 groundWorldMatrixLeft = translate(mat4(1.0f), vec3(-500.0f, 499.0f, 0.0f)) * scale(mat4(1.0f), vec3(0.02f, 1000.0f, 1000.0f));
        setWorldMatrix(texturedShaderProgram, groundWorldMatrixLeft);
        glUniform3f(colLocation, 0.43f, 0.79f, 0.95f); //blue
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0

        // SIDES cube - Right
        mat4 groundWorldMatrixRight = translate(mat4(1.0f), vec3(500.0f, 499.0f, 0.0f)) * scale(mat4(1.0f), vec3(0.02f, 1000.0f, 1000.0f));
        setWorldMatrix(texturedShaderProgram, groundWorldMatrixRight);
        glUniform3f(colLocation, 0.43f, 0.79f, 0.95f); //blue
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
        
        //============================================= SKY BOX END =============================================================

        // Draw ground
        glBindTexture(GL_TEXTURE_2D, cementTextureID);
        mat4 groundWorldMatrix = translate(mat4(1.0f), vec3(0.0f, -0.01f, 0.0f)) * scale(mat4(1.0f), vec3(1000.0f, 0.02f, 1000.0f));
        setWorldMatrix(texturedShaderProgram, groundWorldMatrix);
		glUniform3f(colLocation, 1.0f, 1.0f, 1.0); //beige
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0

        mat4 translation_matrix_one = glm::translate(mat4(1.0f), vec3(translation_x_one, translation_y_one, translation_z_one));
        //========================================== RACKET NUMBER ONE =============================================================================
				
		//Draws TREE TRUNK
		glBindTexture(GL_TEXTURE_2D, cementTextureID);
        for (int i = 0; i < 25; ++i) {
            for (int j = 0; j < 25; ++j) {
                mat4 pillarWorldMatrix_trunk = translate(mat4(1.0f), vec3(-350.0f + j * 35.0f, 7.5f, -350.0f + i * 35.0f)) * scale(mat4(0.5f), vec3(1.5f, 15.0f, 1.5f));
                setWorldMatrix(texturedShaderProgram, pillarWorldMatrix_trunk);
                glUniform3f(colLocation, 0.8039f, 0.5216f, 0.2471f); //brown 205,133,63
                glDrawArrays(GL_TRIANGLES, 0, 36);               
            }
        }

        glBindTexture(GL_TEXTURE_2D, cementTextureID);
        for (int i = 0; i < 25; ++i) {
            for (int j = 0; j < 25; ++j) {
                mat4 pillarWorldMatrix_layer1 = translate(mat4(1.0f), vec3(-350.0f + j * 35.0f, 15.50f, -350.0f + i * 35.0f)) * scale(mat4(0.5f), vec3(12.0f, 1.0f, 12.0f));
                setWorldMatrix(texturedShaderProgram, pillarWorldMatrix_layer1);
                glUniform3f(colLocation, 0.0f, 1.0f, 0.0f); //green
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }

        glBindTexture(GL_TEXTURE_2D, cementTextureID);
        for (int i = 0; i < 25; ++i) {
            for (int j = 0; j < 25; ++j) {
                mat4 pillarWorldMatrix_layer2 = translate(mat4(1.0f), vec3(-350.0f + j * 35.0f, 17.0f, -350.0f + i * 35.0f)) * scale(mat4(0.5f), vec3(6.0f, 2.0f, 6.0f));
                setWorldMatrix(texturedShaderProgram, pillarWorldMatrix_layer2);
                glUniform3f(colLocation, 0.0f, 1.0f, 0.0f); //green
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }


        // Draw colored geometry
        glUseProgram(colorShaderProgram);

        // End Frame
        glfwSwapBuffers(window);
        glfwPollEvents();
        
        // Handle inputs
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
        if (glfwGetKey(window, GLFW_KEY_W ) == GLFW_PRESS)
        {
            cameraPosition += cameraLookAt * dt * currentCameraSpeed;
        }
        
        if (glfwGetKey(window, GLFW_KEY_S ) == GLFW_PRESS)
        {
            cameraPosition -= cameraLookAt * dt * currentCameraSpeed;
        }
        
        if (glfwGetKey(window, GLFW_KEY_D ) == GLFW_PRESS)
        {
            cameraPosition += cameraSideVector * dt * currentCameraSpeed;
        }
        
        if (glfwGetKey(window, GLFW_KEY_A ) == GLFW_PRESS)
        {
            cameraPosition -= cameraSideVector * dt * currentCameraSpeed;
        }


        // Set the view matrix for first and third person cameras
        //mat4 viewMatrix = mat4(1.0);
        // - In first person, camera lookat is set like below
        // - In third person, camera position is on a sphere looking towards center
        mat4 viewMatrix(1.0f);
        
        if (cameraFirstPerson) {
            viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);
        }
       
        setViewMatrix(colorShaderProgram, viewMatrix);
        setViewMatrix(texturedShaderProgram, viewMatrix);
     
        // Shoot projectiles on mouse left click
        // To detect onPress events, we need to check the last state and the current state to detect the state change
        // Otherwise, you would shoot many projectiles on each mouse press
        if (lastMouseLeftState == GLFW_RELEASE && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
            const float projectileSpeed = 25.0f;
            projectileList.push_back(Projectile(cameraPosition, projectileSpeed * cameraLookAt, colorShaderProgram));
        }
        lastMouseLeftState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    }

    glfwTerminate();
    
	return 0;
}

const char* getVertexShaderSource() //this shader is kinda useless don't use it
{
    return
                "#version 330 core\n"
                "layout (location = 0) in vec3 aPos;"
                "layout (location = 1) in vec3 aColor;"
                ""
                "uniform mat4 worldMatrix;"
                "uniform mat4 viewMatrix = mat4(1.0);" 
                "uniform mat4 projectionMatrix = mat4(1.0);"
                ""
                "out vec3 vertexColor;"
                "void main()"
                "{"
                "   vertexColor = aColor;"
                "   mat4 modelViewProjection = projectionMatrix * viewMatrix * worldMatrix;"
                "   gl_Position = modelViewProjection * vec4(aPos.x, aPos.y, aPos.z, 1.0);"
                "}";
}

const char* getFragmentShaderSource() //this shader is kinda useless don't use it
{
    return
                "#version 330 core\n"
                "in vec3 vertexColor;"
                "out vec4 FragColor;"
				"uniform vec3 colorU;" 
                "void main()"
                "{"
				"   FragColor = vec4(colorU, 1.0f) ;" 
                "}";
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
    "uniform mat4 viewMatrix = mat4(1.0);"  // default value for view matrix (identity)
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
		"uniform float is_tex ; "
		"out vec4 FragColor;"
		"uniform vec3 col ; "

		//light and shadow shaders		
		"uniform vec3 light_position;"
		"uniform vec3 light_direction;"
		"uniform vec3 object_color;"

		"const float shading_ambient_strength = 0.1;"
		"const float shading_diffuse_strength = 0.6;"
		"const float shading_specular_strength = 0.3;"

		"float light_cutoff_outer;"
		"uniform float light_cutoff_inner;"
		"uniform float light_near_plane;"
		"uniform float light_far_plane;"
		"uniform float is_transparent = 0.0f ; "

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
		//ends here


		"void main()"
		"{"
		//light and shadow shaders 
		"	vec3 light_color = vec3(1.0f, 1.0f, 1.0f);"
		"   vec3 ambient = vec3(0.0f);"
		"   vec3 diffuse = vec3(0.0f);"
		"   vec3 specular = vec3(0.0f);"
		"   ambient = ambient_color(light_color);"
		"   diffuse = diffuse_color(light_color, light_position);"
		"   specular = specular_color(light_color, light_position);"
		//ends here

		"   vec3 final_color = col * (ambient + diffuse + specular); "
		"   vec3 final_color_no_light = col;   "
		"   vec4 textureColor = texture(textureSampler, vertexUV);"
		"   if (is_tex == 0.0) {"
		"       FragColor = textureColor * vec4(final_color.r, final_color.g, final_color.b, 1.0f);"
		"   } else if (is_tex == 1.0) {"
		"       FragColor = textureColor * vec4(final_color_no_light.r, final_color_no_light.g, final_color_no_light.b, 1.0f);"
		"   }"
		
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
 
int createTexturedCubeVertexArrayObject()
{
    // Create a vertex array
    GLuint vertexArrayObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);
 
    // Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
    GLuint vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texturedCubeVertexArray), texturedCubeVertexArray, GL_STATIC_DRAW);
 
    glVertexAttribPointer(0,                   // attribute 0 matches aPos in Vertex Shader
                          3,                   // size
                          GL_FLOAT,            // type
                          GL_FALSE,            // normalized?
                          sizeof(TexturedColoredVertex), // stride - each vertex contain 2 vec3 (position, color)
                          (void*)0             // array buffer offset
                          );
    glEnableVertexAttribArray(0);
 
 
    glVertexAttribPointer(1,                            // attribute 1 matches aColor in Vertex Shader
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(TexturedColoredVertex),
                          (void*)sizeof(vec3)      // color is offseted a vec3 (comes after position)
                          );
    glEnableVertexAttribArray(1);
 
    glVertexAttribPointer(2,                            // attribute 2 matches aUV in Vertex Shader
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(TexturedColoredVertex),
                          (void*)(2*sizeof(vec3))      // uv is offseted by 2 vec3 (comes after position and color)
                          );
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(TexturedColoredVertex),
                          (void*)(2 * sizeof(vec3) + sizeof(vec2))      // uv is offseted by 2 vec3 (comes after position and color)
                          );
    glEnableVertexAttribArray(3);
                          return vertexArrayObject;
}

