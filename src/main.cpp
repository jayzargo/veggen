 #include <GLFW/glfw3.h>
#include <iostream>

int main() {
    // Test GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }
    
    std::cout << "GLFW initialized successfully!\n";
    std::cout << "GLFW version: " << glfwGetVersionString() << "\n";
    
    glfwTerminate();
    return 0;
}