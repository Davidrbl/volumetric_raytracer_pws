#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include <standard_types.h>
#include <texture.h>
#include <shader.h>

#define GL_VERSION_MAJOR 4
#define GL_VERSION_MINOR 6
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 1024
#define MOUSE_SENSITIVITY 0.5
#define CAMERA_SPEED 1.0

#define PI 3.141592

static void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %d %s\n", error, description);
}

static void framebuffer_resize_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}

u8 texture_function(float x, float y, float z){
    x = x*2.0-1.0;
    y = y*2.0-1.0;
    z = z*2.0-1.0;

    float dist = sqrt(
        x*x +
        y*y +
        z*z
    );

    dist /= sqrt(3); // divide by maximum value, so dist is now from 0 to 1

    dist *= (u8)0xFF;

    return (u8)dist;
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

void calc_movement(GLFWwindow* window, float* cam_pos, float* cam_rot, float* cam_for, float dt, double* mouse_x, double* mouse_y){
    cam_rot[0] += (mouse_y[1] - mouse_y[0])
                   * MOUSE_SENSITIVITY * dt;
    cam_rot[1] += (mouse_x[0] - mouse_x[1])
                   * MOUSE_SENSITIVITY * dt;

    if (cam_rot[0] >  0.5*PI) cam_rot[0] =  0.5*PI;
    if (cam_rot[0] < -0.5*PI) cam_rot[0] = -0.5*PI;

    float mul;

    // W and S
    mul = glfwGetKey(window, GLFW_KEY_W) - glfwGetKey(window, GLFW_KEY_S);

    cam_pos[0] += sin(-cam_rot[1]) * mul * CAMERA_SPEED * dt;
    cam_pos[2] += cos(-cam_rot[1]) * mul * CAMERA_SPEED * dt;

    // A and D
    mul = glfwGetKey(window, GLFW_KEY_A) - glfwGetKey(window, GLFW_KEY_D);

    cam_pos[0] += cos(cam_rot[1]) * mul * CAMERA_SPEED * dt;
    cam_pos[2] += sin(cam_rot[1]) * mul * CAMERA_SPEED * dt;

    // Space and shift
    mul = glfwGetKey(window, GLFW_KEY_SPACE) - glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
    cam_pos[1] += mul * CAMERA_SPEED * dt;

    float temp = cos(cam_rot[0]);

    cam_for[0] = sin(-cam_rot[1]) * temp;
    cam_for[1] = sin(cam_rot[0]);
    cam_for[2] = cos(cam_rot[1]) * temp;
}

int main() {
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        return 1;
    }

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
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
    if (glfwRawMouseMotionSupported()) {
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }

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

    u32 objects_buffer;

    float* test_buffer = NULL;
    u32 test_buffer_size = 0;

    // 5 spheres, 10 cubes
    u32 num_spheres = 2;
    u32 num_cubes = 4;

    u32 index = 0;

    test_buffer_size =  num_spheres * 4 * sizeof(float) +
                        num_cubes * 6 * sizeof(float) +
                        2 * sizeof(float); // for the split floats

    // test_buffer_size = 1000;
    test_buffer = malloc(test_buffer_size);

    test_buffer[index] = num_spheres;
    index++;

    for (u32 i = 0; i < num_spheres; i++){
        test_buffer[index + 0] = (float)(i+1);
        test_buffer[index + 1] = (float)(i+1);
        test_buffer[index + 2] = (float)(i+1);
        test_buffer[index + 3] = 0.5f;

        index += 4;
    }

    test_buffer[index] = num_cubes;
    index++;

    for (u32 i = 0; i < num_cubes; i++){
        test_buffer[index + 0] = -(float)(i+1);
        test_buffer[index + 1] = (float)(i+1);
        test_buffer[index + 2] = (float)(i+1);
        test_buffer[index + 3] = 0.5f;
        test_buffer[index + 4] = 0.5f;
        test_buffer[index + 5] = 0.5f;

        index += 6;
    }


    // Print the test scene
    // index = 0;
    // while (1){
    //     if (test_buffer[index] == (float)0xFFFFFFFF) break;
    //     printf("Sphere -> %f %f %f %f\n",
    //         test_buffer[index + 0],
    //         test_buffer[index + 1],
    //         test_buffer[index + 2],
    //         test_buffer[index + 3]
    //     );

    //     index += 4;
    // }

    // printf("SPLIT DETECTED\n");

    // index++;

    // while (1){
    //     if (test_buffer[index] == (float)0xFFFFFFFF) break;
    //     printf("Cube -> %f %f %f | %f %f %f\n",
    //         test_buffer[index + 0],
    //         test_buffer[index + 1],
    //         test_buffer[index + 2],

    //         test_buffer[index + 3],
    //         test_buffer[index + 4],
    //         test_buffer[index + 5]
    //     );

    //     index += 6;
    // }

    // printf("SPLIT DETECTED\n");

    // index++;

    // for (u32 i = 0; i < test_buffer_size/ sizeof(float); i++){
    //     printf("%d -> %f\n", i, test_buffer[i]);
    // }

    glCreateBuffers(1, &objects_buffer);
    glNamedBufferData(objects_buffer, test_buffer_size, test_buffer, GL_DYNAMIC_READ);
    free(test_buffer);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, objects_buffer);

    float time_begin = 0.f;
    u64 frame = 1;

    float frame_begin_time = 0.f;
    float frame_end_time = 0.f;
    float dt; // delta-time

    u32 cube_density_texture;
    create_texture3D(32, 32, 32, texture_function, &cube_density_texture);

    float cam_pos[3] = {0.0};
    float cam_rot[2] = {0.0};
    float cam_for[3] = {0.0};

    double mouse_x[2], mouse_y[2];
    glfwGetCursorPos(window, mouse_x, mouse_x);

    while (!glfwWindowShouldClose(window)){
        mouse_x[1] = mouse_x[0];
        mouse_y[1] = mouse_y[0];
        glfwGetCursorPos(window, mouse_x, mouse_y);
        dt = frame_end_time - frame_begin_time;
        frame_begin_time = glfwGetTime();
        calc_movement(window, cam_pos, cam_rot, cam_for, dt, mouse_x, mouse_y);
        if (glfwGetKey(window, GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window, 1);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glUseProgram(main_program);

        glUniform3fv(glGetUniformLocation(main_program, "cam_origin"), 1, cam_pos);
        glUniform3fv(glGetUniformLocation(main_program, "cam_for"), 1, cam_for);

        glDrawArrays(GL_POINTS, 0, 1);

        glfwPollEvents();
        glfwSwapBuffers(window);

        if (frame % 100 == 0) {
            if (glfwGetKey(window, GLFW_KEY_C)) {
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
                printf("mouse_x: %f | %f\n", mouse_x[0], mouse_x[1]);
                printf("mouse_y: %f | %f\n", mouse_y[0], mouse_y[1]);
                printf("\n");
            }
            if (glfwGetKey(window, GLFW_KEY_T)) {
                printf("time for frame: %f\n", (glfwGetTime() - time_begin) * 10.f);
                printf("\n");
            }
            time_begin = glfwGetTime();
        }
        frame_end_time = glfwGetTime();
        frame++;
    }
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
