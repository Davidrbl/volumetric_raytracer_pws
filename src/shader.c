#include <stdio.h>
#include <stdlib.h>

#include <shader.h>

void create_shader(const char* path, GLenum type, u32* dest) {
    *dest = glCreateShader(type);

    FILE* file_pointer = fopen(path, "rb");

    if (!file_pointer) {
        exit(42);
    }

    fseek(file_pointer, 0, SEEK_END);

    size_t file_len = ftell(file_pointer);

    fseek(file_pointer, 0, SEEK_SET);

    GLchar* source = malloc((file_len + 1) * sizeof(GLchar));

    fread(source, sizeof(GLchar), file_len, file_pointer);
    source[file_len] = '\0';

    fclose(file_pointer);

    glShaderSource(*dest, 1, (const GLchar* const*)&source, NULL);

    glCompileShader(*dest);

    i32 succes = 0;
    glGetShaderiv(*dest, GL_COMPILE_STATUS, &succes);
    if (succes != GL_TRUE) {
        i32 error_len = 0;
        glGetShaderiv(*dest, GL_INFO_LOG_LENGTH, &error_len);
        GLchar error_string[error_len + 1];
        glGetShaderInfoLog(*dest, error_len, &error_len, error_string);

        glDeleteShader(*dest);

        fprintf(stderr, "SHADER COMPILE ERROR: %s\n%s\n", path, error_string);
        exit(1);
    }

    free(source);
}

void create_program2s(const char* vertex_shader_path, const char* fragment_shader_path, u32* dest) {
    *dest = glCreateProgram();

    u32 vertex_shader = 0;
    u32 fragment_shader = 0;
    create_shader(vertex_shader_path,   GL_VERTEX_SHADER,   &vertex_shader);
    create_shader(fragment_shader_path, GL_FRAGMENT_SHADER, &fragment_shader);

    glAttachShader(*dest, vertex_shader);
    glAttachShader(*dest, fragment_shader);

    glLinkProgram(*dest);

    i32 succes = 0;
    glGetProgramiv(*dest, GL_LINK_STATUS, &succes);
    if (succes != GL_TRUE) {
        fprintf(stderr, "SHADER LINK ERROR\n");
        exit(1);
    }
}

void create_program3s(const char* vertex_shader_path, const char* geometry_shader_path, const char* fragment_shader_path, u32* dest) {
    *dest = glCreateProgram();

    u32 vertex_shader = 0;
    u32 geometry_shader = 0;
    u32 fragment_shader = 0;
    create_shader(vertex_shader_path,   GL_VERTEX_SHADER,   &vertex_shader);
    create_shader(geometry_shader_path, GL_GEOMETRY_SHADER, &geometry_shader);
    create_shader(fragment_shader_path, GL_FRAGMENT_SHADER, &fragment_shader);

    glAttachShader(*dest, vertex_shader);
    glAttachShader(*dest, geometry_shader);
    glAttachShader(*dest, fragment_shader);

    glLinkProgram(*dest);

    i32 succes = 0;
    glGetProgramiv(*dest, GL_LINK_STATUS, &succes);
    if (succes != GL_TRUE) {
        fprintf(stderr, "SHADER LINK ERROR\n");
        exit(1);
    }
}

void create_program(
    const char* compute_shader_path,
    const char* vertex_shader_path,
    const char* tessellation_control_shader_path,
    const char* tessellation_evaluation_shader_path,
    const char* geometry_shader_path,
    const char* fragment_shader_path,
    u32* dest
) {
    *dest = glCreateProgram();

    u32 compute_shader = 0;
    u32 vertex_shader = 0;
    u32 tessellation_control_shader = 0;
    u32 tessellation_evaluation_shader = 0;
    u32 geometry_shader = 0;
    u32 fragment_shader = 0;

    if (compute_shader_path) {
        create_shader(compute_shader_path, GL_COMPUTE_SHADER, &compute_shader);
        glAttachShader(*dest, compute_shader);
    }
    if (vertex_shader_path) {
        create_shader(vertex_shader_path, GL_VERTEX_SHADER, &vertex_shader);
        glAttachShader(*dest, vertex_shader);
    }
    if (tessellation_control_shader_path) {
        create_shader(tessellation_control_shader_path, GL_TESS_CONTROL_SHADER, &tessellation_control_shader);
        glAttachShader(*dest, tessellation_control_shader);
    }
    if (tessellation_evaluation_shader_path) {
        create_shader(tessellation_evaluation_shader_path, GL_TESS_EVALUATION_SHADER, &tessellation_evaluation_shader);
        glAttachShader(*dest, tessellation_evaluation_shader);
    }
    if (geometry_shader_path) {
        create_shader(geometry_shader_path, GL_GEOMETRY_SHADER, &geometry_shader);
        glAttachShader(*dest, geometry_shader);
    }
    if (fragment_shader_path) {
        create_shader(fragment_shader_path, GL_FRAGMENT_SHADER, &fragment_shader);
        glAttachShader(*dest, fragment_shader);
    }

    glLinkProgram(*dest);

    i32 succes = 0;
    glGetProgramiv(*dest, GL_LINK_STATUS, &succes);
    if (succes != GL_TRUE) {
        fprintf(stderr, "SHADER LINK ERROR\n");
        exit(1);
    }

    if (vertex_shader_path) {
        glDeleteShader(vertex_shader);
    }
    if (tessellation_control_shader_path) {
        glDeleteShader(tessellation_control_shader);
    }
    if (tessellation_evaluation_shader_path) {
        glDeleteShader(tessellation_evaluation_shader);
    }
    if (geometry_shader_path) {
        glDeleteShader(geometry_shader);
    }
    if (fragment_shader_path) {
        glDeleteShader(fragment_shader);
    }
}
