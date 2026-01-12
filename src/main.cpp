#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);


int main() {

    // GLM Test
    glm::vec3 position(1.0f, 2.0f, 3.0f);
    std::cout << "Position: " << position.x << ", "
        << position.y << ", " << position.z << std::endl;

    glm::mat4 matrix = glm::mat4(1.0f);
    std::cout << "Matrix created successfully!" << std::endl;

    glm::mat4 transform = glm::translate(matrix, glm::vec3(1.0f, 0.0f, 0.0f));
    std::cout << "GLM works!" << std::endl;


    // Test GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    std::cout << "GLFW initialized successfully!\n";
    std::cout << "GLFW version: " << glfwGetVersionString() << "\n";

    const int glfw_width = 800;
    const int glfw_height = 600;

    GLFWwindow* window = glfwCreateWindow(glfw_width, glfw_height, "Veggen", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    std::cout << "OpenGL: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    printf("Window resized to: %dx%d\n", width, height);
    glViewport(0, 0, width, height);
}