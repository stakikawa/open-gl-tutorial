// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLEW. Always include it before gl.h and glfw3.h, since it's a bit magic.
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

// Include Shaders
#include <common/shader.hpp>

int main() {
    // Initialize GLFW
    glewExperimental = true; // Needed for core profile
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); 
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make mac happy (not needed ?)
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Don't want old OpenGL

    // Open a window and create OpenGL context
    GLFWwindow* window;
    int width = 1024;
    int height = 768;
    window = glfwCreateWindow(width, height, "Tutorial 1", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window); // Initialize GLFW
    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to inialize GLFW");
        return -1;
    }

    // Ensure we can capture the escape key being pressed
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");

    // Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    // Vertex Array Object
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

    // vertex info to represent triangle vertices
    static const GLfloat g_vertex_buffer_data[] = { 
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 0.0f,  1.0f, 0.0f,
	};

    // identifies vertex buffer
	GLuint vertexbuffer;
    // generate buffer and put identifier in vertexbuffer
	glGenBuffers(1, &vertexbuffer);
    // specifies "what buffer" we are talking about
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    // give buffer and data to OpenGL
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    // Setting up MVP matrices:
    // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    mat4 Projection = perspective(radians(45.0f), (float) width / (float) height, 0.1f, 100.0f);
  
    // Or, for an ortho camera :
    //glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates
  
    // Camera matrix
    mat4 View = lookAt(
        vec3(4,3,3), // Camera is at (4,3,3), in World Space
        vec3(0,0,0), // and looks at the origin
        vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
        );
  
    // Model matrix : an identity matrix (model will be at the origin)
    mat4 Model = mat4(1.0f);
    // Our ModelViewProjection : multiplication of our 3 matrices
    mat4 mvp = Projection * View * Model; // Remember, matrix multiplication is the other way around

    do {
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT);

        // Get a handle for our "MVP" uniform
        // Only during the initialisation
        GLuint MatrixID = glGetUniformLocation(programID, "MVP");

        // Send our transformation to the currently bound shader, in the "MVP" uniform
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);

        // Use Shader
        glUseProgram(programID);

        // Attribute Buffer: vertices
        glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

        // Draw
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glDisableVertexAttribArray(0);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    } 
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0);

}
