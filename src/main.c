#include <stdio.h>
#include <stdlib.h>
#include <standard_types.h>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

int main(){
    if (!glfwInit()){
        printf("Failed to init glfw!\n");
        exit(1);
    }

    uint32_t window_width = 1024;
    uint32_t window_height = 1024;
    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "Volumetric Raytracer", NULL, NULL);

    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)){
        printf("Failed to init glad!\n");
        exit(1);
    }

    while(!glfwWindowShouldClose(window)){
        if (glfwGetKey(window, GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window, 1);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    glfwDestroyWindow(window);
    glfwTerminate();
}
