#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include <standard_types.h>
#include <shader.h>

#define GL_VERSION_MAJOR 4
#define GL_VERSION_MINOR 6
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 1024
#define MOUSE_SENSITIVITY 3.0
#define CAMERA_SPEED 0.1

#define PI 3.141592

static void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %d %s\n", error, description);
}

static void framebuffer_resize_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}

void GLAPIENTRY gl_error_callback(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    i32 length,
    const GLchar* message,
    const void* userParam
){
    // GL_DEBUG_SEVERITY_HIGH;
    // GL_DEBUG_SEVERITY_MEDIUM;
    // GL_DEBUG_SEVERITY_LOW;

    if (severity != GL_DEBUG_SEVERITY_NOTIFICATION)
        fprintf(stderr, "GL_ERROR \nsource: %x \ntype: %x \nid: %u \nseverity: %x \nlength: %d \nmessage: %s\nuserParam: %p\n",
            source, type, id, severity, length, message, userParam);
}

void calc_movement(GLFWwindow* window, float* orig, float* rotation, float prev_mouse_x, float prev_mouse_y){
    rotation[0] += (glfwGetKey(window, GLFW_KEY_I) - glfwGetKey(window, GLFW_KEY_K))
                     * MOUSE_SENSITIVITY * 0.01;
    rotation[1] += (glfwGetKey(window, GLFW_KEY_L) - glfwGetKey(window, GLFW_KEY_J))
                     * MOUSE_SENSITIVITY * 0.01;

    if (rotation[0] >  0.49*PI) rotation[0] =  0.49*PI;
    if (rotation[0] < -0.49*PI) rotation[0] = -0.49*PI;

    float mul;

    // W and S
    mul = glfwGetKey(window, GLFW_KEY_W) - glfwGetKey(window, GLFW_KEY_S);

    orig[0] += sin(-rotation[1]) * mul * CAMERA_SPEED;
    orig[2] += cos(-rotation[1]) * mul * CAMERA_SPEED;

    // A and D
    mul = glfwGetKey(window, GLFW_KEY_A) - glfwGetKey(window, GLFW_KEY_D);

    orig[0] += cos(rotation[1]) * mul * CAMERA_SPEED; // * dt
    orig[2] += sin(rotation[1]) * mul * CAMERA_SPEED; // * dt

    // Space and shift
    mul = glfwGetKey(window, GLFW_KEY_SPACE) - glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
    orig[1] += mul * CAMERA_SPEED;
}

int main() {
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        return 1;
    }

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH,
                                          WINDOW_HEIGHT,
                                          "Volumetric Raytracer",
                                          NULL,
                                          NULL);
    if (!window) {
        glfwTerminate();
        return 2;
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_resize_callback);

    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GL_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GL_VERSION_MINOR);

    if (!gladLoadGL(glfwGetProcAddress)) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return 3;
    }

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(gl_error_callback, NULL);

    glClearColor(1.0, 0.0, 1.0, 1.0);

    // Shader program setup
    GLint main_program;
    create_program3s(
        "src/shaders/main.vert",
        "src/shaders/main.geom",
        "src/shaders/main.frag",
        &main_program
    );

    float time_begin;
    u64 frame = 1;

    float cam_pos[3] = {0.0, 0.0, 0.0};
    float cam_rot[2] = {0.0, 0.0};
    float cam_for[3] = {0.0, 0.0, 1.0};

    float prev_mouse_x = 0.0, prev_mouse_y = 0.0;
    // glfwGetCursorPos(window, &prev_mouse_x, &prev_mouse_x);

    while(!glfwWindowShouldClose(window)){
        calc_movement(window, cam_pos, cam_rot, prev_mouse_x, prev_mouse_y);
        // glfwGetCursorPos(window, &prev_mouse_x, &prev_mouse_y);
        if (glfwGetKey(window, GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window, 1);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glUseProgram(main_program);

        float temp = cos(cam_rot[0]);

        cam_for[0] = sin(-cam_rot[1]) * temp;
        cam_for[1] = sin(cam_rot[0]);
        cam_for[2] = cos(cam_rot[1]) * temp;

        glUniform3fv(glGetUniformLocation(main_program, "cam_origin"), 1, cam_pos);
        glUniform2fv(glGetUniformLocation(main_program, "rotation"), 1, cam_rot);
        glUniform3fv(glGetUniformLocation(main_program, "cam_for"), 1, cam_for);

        glDrawArrays(GL_POINTS, 0, 1);

        glfwPollEvents();
        glfwSwapBuffers(window);

        if (frame % 100 == 0) {
            printf("cam_pos -> %f | %f | %f\n",
                cam_pos[0],
                cam_pos[1],
                cam_pos[2]
            );
            printf("cam_rot -> %f | %f\n", cam_rot[0], cam_rot[1]);
            printf("cam_for -> %f | %f | %f\n",
                cam_for[0],
                cam_for[1],
                cam_for[2]
            );
        }

        if (frame % 1000 == 0){
            printf("time for frame: %f\n", (glfwGetTime()-time_begin));
            time_begin = glfwGetTime();
        }
        frame++;
    }
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
