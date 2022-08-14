#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include <logging.h>
#include <shader.h>
#include <standard_types.h>
#include <texture.h>

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#define CIMGUI_USE_GLFW
#define CIMGUI_USE_OPENGL3
#include <cimgui_impl.h>

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 1024
#define MOUSE_SENSITIVITY .001f
#define SCROLL_SPEED .5f

#ifndef LOGLEVEL
#define LOGLEVEL 0
#endif

#define PI 3.14159265f

static float cam_speed = 1.f;

union utof {
    u32 u;
    float f;
};

static void error_callback(int error, const char* description) {
    tlog(5, "GLFW error: %d %s\n", error, description);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
static void framebuffer_resize_callback(
    GLFWwindow* window, // NOLINT(misc-unused-parameters)
    int width,
    int height
) {
    glViewport(0, 0, width, height);
}

static void scroll_callback(
    GLFWwindow* window, // NOLINT(misc-unused-parameters)
    double xoffset, // NOLINT(misc-unused-parameters)
    double yoffset
) {
    cam_speed += (float)yoffset * SCROLL_SPEED;
}
#pragma GCC diagnostic pop

void GLAPIENTRY gl_error_callback(
    GLenum source,
    GLenum type,
    u32 id,
    GLenum severity,
    i32 length,
    const GLchar* message,
    const void* userParam
) {
    // GL_DEBUG_SEVERITY_HIGH;
    // GL_DEBUG_SEVERITY_MEDIUM;
    // GL_DEBUG_SEVERITY_LOW;

    if (severity != GL_DEBUG_SEVERITY_NOTIFICATION) {
        tlog(
            5,
            "GL_ERROR\nsource: 0x%x\ntype: 0x%x\nid: %u\nseverity: 0x%x\nlength: %d\nmessage: %s\nuserParam: 0x%p\n",
            source,
            type,
            id,
            severity,
            length,
            message,
            userParam
        );
    }
}

static float texture_function(float x, float y, float z) {
    x = x * 2.f - 1.f;
    y = y * 2.f - 1.f;
    z = z * 2.f - 1.f;

    float dist = sqrtf(
        x * x +
        y * y +
        z * z
    );

    dist /= sqrtf(3.f); // divide by maximum value, so dist is now from 0 to 1

    return 1.0;
    if (dist > 0.5) return 0.0;
    else return 1.0;
}

static inline void calc_movement(
    GLFWwindow* window,
    float* cam_pos,
    float* cam_rot,
    float* cam_for,
    float dt,
    const double* mouse_x,
    const double* mouse_y
) {
    cam_rot[0] += (float)(mouse_y[1] - mouse_y[0]) * MOUSE_SENSITIVITY;
    cam_rot[1] += (float)(mouse_x[0] - mouse_x[1]) * MOUSE_SENSITIVITY;

    if (cam_rot[0] > .49f * PI) {
        cam_rot[0] = .49f * PI;
    }
    if (cam_rot[0] < -.49f * PI) {
        cam_rot[0] = -.49f * PI;
    }

    // W and S
    float mul = (float)(glfwGetKey(window, GLFW_KEY_W) - glfwGetKey(window, GLFW_KEY_S));

    cam_pos[0] += sinf(-cam_rot[1]) * mul * cam_speed * dt;
    cam_pos[2] += cosf(-cam_rot[1]) * mul * cam_speed * dt;

    // A and D
    mul = (float)(glfwGetKey(window, GLFW_KEY_A) - glfwGetKey(window, GLFW_KEY_D));

    cam_pos[0] += cosf(cam_rot[1]) * mul * cam_speed * dt;
    cam_pos[2] += sinf(cam_rot[1]) * mul * cam_speed * dt;

    // Space and shift
    mul = (float)(glfwGetKey(window, GLFW_KEY_SPACE) - glfwGetKey(window, GLFW_KEY_LEFT_SHIFT));

    cam_pos[1] += mul * cam_speed * dt;

    float temp = cosf(cam_rot[0]);

    cam_for[0] = sinf(-cam_rot[1]) * temp;
    cam_for[1] = sinf(cam_rot[0]);
    cam_for[2] = cosf(cam_rot[1]) * temp;
}

int main() {
    tlog_init(LOGLEVEL, stderr);
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        "Volumetric Raytracer",
        NULL,
        NULL
    );
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
    glfwSetScrollCallback(window, scroll_callback);

    if (!gladLoadGL(glfwGetProcAddress)) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return 3;
    }

    glfwSwapInterval(0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ZERO);

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(gl_error_callback, NULL);

    glClearColor(1.f, 0.f, 1.f, 1.f);

    ImGuiContext* ctx = igCreateContext(NULL);
    ImGuiIO* io = igGetIO();
    
    igImplGlfw_InitForOpenGL(window, true);
    igImplOpenGL3_Init("#version 330 core");
    
    // Shader program setup
    u32 main_program = 0;
    create_program(
        "src/shaders/main.vert",
        NULL,
        NULL,
        "src/shaders/main.geom",
        "src/shaders/main.frag",
        &main_program
    );

    u32 texture_program = 0;
    create_program(
        "src/shaders/texture.vert",
        NULL,
        NULL,
        "src/shaders/texture.geom",
        "src/shaders/texture.frag",
        &texture_program
    );

    u32 framebuffer = 0;
    u32 framebuffer_texture = 0;

    i32 framebuffer_width = 1024;
    i32 framebuffer_height = 1024;

    {
        glCreateFramebuffers(1, &framebuffer);
        glCreateTextures(GL_TEXTURE_2D, 1, &framebuffer_texture);

        glTextureStorage2D(framebuffer_texture, 1, GL_RGB8, framebuffer_width, framebuffer_height);

        glTextureParameteri(framebuffer_texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(framebuffer_texture, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTextureParameteri(framebuffer_texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(framebuffer_texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        u32 renderbuffer = 0;
        // This is just a dummy thing, its sorta like a texture for depth and stencil,
        // but you cant read this and its faster.
        // we dont want to read it anyway so this is cool.

        glCreateRenderbuffers(1, &renderbuffer);

        glNamedRenderbufferStorage(renderbuffer, GL_DEPTH24_STENCIL8, framebuffer_width, framebuffer_height);

        glNamedFramebufferTexture(framebuffer, GL_COLOR_ATTACHMENT0, framebuffer_texture, 0);
        glNamedFramebufferRenderbuffer(framebuffer, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);

        // Check if we have added everything needed for the framebuffer

        // GLenum status = glCheckNamedFramebufferStatus(framebuffer, GL_FRAMEBUFFER);
        // tlog(0, "Framebufferstatus -> %X\n", status);
        // GL_FRAMEBUFFER_COMPLETE -> 8CD5
    }

    u32 num_spheres = 4;
    u32 num_cubes = 4;
    u32 num_vol_cubes = 3;

    i64 test_buffer_size = (num_spheres * 4 + num_cubes * 6 + num_vol_cubes * 7 + 3) * (i64)sizeof(float); // for the split floats

    union utof* test_buffer = malloc(test_buffer_size);

    i64 index = 0;
    test_buffer[index].u = num_spheres;
    index++;

    for (i64 i = 0; i < num_spheres; i++) {
        test_buffer[index++].f = (float)(i + 1);
        test_buffer[index++].f = (float)(i + 1);
        test_buffer[index++].f = (float)(i + 1);
        test_buffer[index++].f = .5f;
    }

    test_buffer[index].u = num_cubes;
    index++;

    for (i64 i = 0; i < num_cubes; i++) {
        test_buffer[index++].f = -(float)(i + 1);
        test_buffer[index++].f = (float)(i + 1);
        test_buffer[index++].f = (float)(i + 1);
        test_buffer[index++].f = .5f;
        test_buffer[index++].f = .5f;
        test_buffer[index++].f = .5f;
    }

    test_buffer[index].u = num_vol_cubes;
    index++;

    for (i64 i = 0; i < num_vol_cubes; i++){
        test_buffer[index++].f = 0.f;
        test_buffer[index++].f = (float)(i+1);
        test_buffer[index++].f = 0.f;

        test_buffer[index++].f = .5f;
        test_buffer[index++].f = .5f;
        test_buffer[index++].f = .5f;

        test_buffer[index++].f = 1.f / (float)(i + 1);
    }

    u32 objects_buffer = 0;

    glCreateBuffers(1, &objects_buffer);
    glNamedBufferData(objects_buffer, test_buffer_size, test_buffer, GL_DYNAMIC_READ);
    free(test_buffer);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, objects_buffer);

    u32 num_dir_lights = 1;
    u32 num_lights = 0;

    i64 lights_buffer_size = (2 + num_dir_lights * 4 + num_lights * 4) * (i64)sizeof(float);

    union utof* lights_buffer_data = malloc(lights_buffer_size);

    index = 0;
    lights_buffer_data[index++].u = num_dir_lights;

    for (u32 i = 0; i < num_dir_lights; i++){
        lights_buffer_data[index++].f = 3.0; // X pos
        lights_buffer_data[index++].f = -3.0;//(float)i * 2.0; // Y pos
        lights_buffer_data[index++].f = 0.0;//(float)i * 5.0; // Z pos
        lights_buffer_data[index++].f = 15.f; // Power
    }

    lights_buffer_data[index++].u = num_lights;

    for (u32 i = 0; i < num_lights; i++){
        lights_buffer_data[index++].f = 0.0;
        lights_buffer_data[index++].f = (float)(i+2);
        lights_buffer_data[index++].f = (float)(i+2);
        lights_buffer_data[index++].f = 150.0;
    }

    // for (u32 i = 0; i < lights_buffer_size/sizeof(float); i++) {
    //     tlog(0, "0x%X\t --- %f\t --- \t%u\n", lights_buffer_data[i].u, lights_buffer_data[i].f, lights_buffer_data[i].u);
    // }

    u32 lights_buffer = 0;

    glCreateBuffers(1, &lights_buffer);
    glNamedBufferData(lights_buffer, lights_buffer_size, lights_buffer_data, GL_DYNAMIC_READ);
    free(lights_buffer_data);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, lights_buffer);

    // Skybox setup
    const char* skybox_textures_paths[6] = {
        "textures/positive_x.jpg",
        "textures/negative_x.jpg",
        "textures/positive_y.jpg",
        "textures/negative_y.jpg",
        "textures/positive_z.jpg",
        "textures/negative_z.jpg"
    };

    u32 skybox_texture = 0;
    create_cubemap(
        skybox_textures_paths,
        &skybox_texture
    );

    glBindTextureUnit(1, skybox_texture);

    double time_begin = 0.;
    u64 frame = 1;

    double frame_begin_time = 0.;
    double frame_end_time = 0.;
    float dt = 0.f; // delta-time

    u32 cube_density_texture = 0;
    create_texture3D(128, 128, 128, texture_function, &cube_density_texture);
    glBindTextureUnit(0, cube_density_texture);

    float cam_pos[3] = {1.0, 0.5f, -1.0};
    float cam_rot[2] = {0.f};
    float cam_for[3] = {0.f};

    double mouse_x[2] = {0.};
    double mouse_y[2] = {0.};
    glfwGetCursorPos(window, mouse_x, mouse_x);

    // Dummy VAO
    u32 vao = 0;
    glCreateVertexArrays(1, &vao);
    glBindVertexArray(vao);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        dt = (float)(frame_end_time - frame_begin_time);
        frame_begin_time = glfwGetTime();

        mouse_x[1] = mouse_x[0];
        mouse_y[1] = mouse_y[0];
        glfwGetCursorPos(window, mouse_x, mouse_y);
        calc_movement(window, cam_pos, cam_rot, cam_for, dt, mouse_x, mouse_y);

        if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(window, 1);
        }

        // RENDERING TO FRAMEBUFFER

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        glViewport(0, 0, framebuffer_width, framebuffer_height);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glUseProgram(main_program);

        glUniform1i(glGetUniformLocation(main_program, "cube_density_texture"), 0);
        glUniform1i(glGetUniformLocation(main_program, "skybox_texture"), 1);
        glUniform3fv(glGetUniformLocation(main_program, "cam_origin"), 1, cam_pos);
        glUniform3fv(glGetUniformLocation(main_program, "cam_for"), 1, cam_for);
        glUniform1f(glGetUniformLocation(main_program, "time"), (float)frame_begin_time);

        glDrawArrays(GL_POINTS, 0, 1);

        // RENDERING TO SCREEN

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(texture_program);

        glBindTextureUnit(0, framebuffer_texture);
        glUniform1i(glGetUniformLocation(texture_program, "texture_ID"), 0);

        glDrawArrays(GL_POINTS, 0, 1);


        // imgui gui drawing
        igImplOpenGL3_NewFrame();
        igImplGlfw_NewFrame();
        igNewFrame();
        
        igShowDemoWindow(NULL);
        
        igRender();
        igImplOpenGL3_RenderDrawData(igGetDrawData());
        
        glfwSwapBuffers(window);

        if (frame % 100 == 0) {
            if (glfwGetKey(window, GLFW_KEY_C)) {
                tlog(
                    0,
                    "cam_pos -> %f | %f | %f\n",
                    cam_pos[0],
                    cam_pos[1],
                    cam_pos[2]
                );
                tlog(0, "cam_rot -> %f | %f\n", cam_rot[0], cam_rot[1]);
                tlog(
                    0,
                    "cam_for -> %f | %f | %f\n",
                    cam_for[0],
                    cam_for[1],
                    cam_for[2]
                );
                tlog(0, "mouse_x: %f | %f\n", mouse_x[0], mouse_x[1]);
                tlog(0, "mouse_y: %f | %f\n", mouse_y[0], mouse_y[1]);
                tlog(0, "\n");
            }
            if (glfwGetKey(window, GLFW_KEY_T)) {
                tlog(0, "time for frame: %f ms\n", (glfwGetTime() - time_begin) * 10.);
                tlog(0, "\n");
            }
            if (glfwGetKey(window, GLFW_KEY_I)) {
                tlog(0, "GL_VENDOR -> %s\n",    glGetString(GL_VENDOR));
                tlog(0, "GL_RENDERER -> %s\n",  glGetString(GL_RENDERER));
                tlog(0, "GL_VERSION -> %s\n",   glGetString(GL_VERSION));

                GLint a;
                glGetIntegerv(GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS, &a);
                tlog(0, "GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS -> %d\n", a);
                tlog(0, "\n"); // david: kweenie waarom jij dit doet maar ik doe het ook wel

            }
            if (glfwGetKey(window, GLFW_KEY_R)){
                create_program(
                    "src/shaders/main.vert",
                    NULL,
                    NULL,
                    "src/shaders/main.geom",
                    "src/shaders/main.frag",
                    &main_program
                );
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
