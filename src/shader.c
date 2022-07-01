#include <stdio.h>
#include <stdlib.h>

#include <shader.h>

void create_shader(const GLchar* const source, GLenum type, u32* dest) {
    *dest = glCreateShader(type);

    glShaderSource(*dest, 1, &source, NULL);

    glCompileShader(*dest);

    i32 succes = 0;
    glGetShaderiv(*dest, GL_COMPILE_STATUS, &succes);
    if (succes != GL_TRUE) {
        i32 error_len = 0;
        glGetShaderiv(*dest, GL_INFO_LOG_LENGTH, &error_len);
        GLchar error_string[error_len + 1];
        glGetShaderInfoLog(*dest, error_len, &error_len, error_string);

        glDeleteShader(*dest);

        fprintf(stderr, "SHADER COMPILE ERROR: %s\n", error_string);
        exit(1);
    }
}

void create_program_compute(const GLchar* compute_shader_source, u32* dest) {
    *dest = glCreateProgram();

    u32 compute_shader = 0;
    create_shader(compute_shader_source, GL_VERTEX_SHADER, &compute_shader);

    glAttachShader(*dest, compute_shader);

    glLinkProgram(*dest);

    i32 succes = 0;
    glGetProgramiv(*dest, GL_LINK_STATUS, &succes);
    if (succes != GL_TRUE) {
        fprintf(stderr, "SHADER LINK ERROR\n");
        exit(1);
    }

    glDeleteShader(compute_shader);
}

void create_program(
    const GLchar* vertex_shader_source,
    const GLchar* tessellation_control_shader_source,
    const GLchar* tessellation_evaluation_shader_source,
    const GLchar* geometry_shader_source,
    const GLchar* fragment_shader_source,
    u32* dest
) {
    *dest = glCreateProgram();

    u32 vertex_shader = 0;
    u32 tessellation_control_shader = 0;
    u32 tessellation_evaluation_shader = 0;
    u32 geometry_shader = 0;
    u32 fragment_shader = 0;

    if (vertex_shader_source) {
        create_shader(vertex_shader_source, GL_VERTEX_SHADER, &vertex_shader);
        glAttachShader(*dest, vertex_shader);
    }
    if (tessellation_control_shader_source) {
        create_shader(tessellation_control_shader_source, GL_TESS_CONTROL_SHADER, &tessellation_control_shader);
        glAttachShader(*dest, tessellation_control_shader);
    }
    if (tessellation_evaluation_shader_source) {
        create_shader(tessellation_evaluation_shader_source, GL_TESS_EVALUATION_SHADER, &tessellation_evaluation_shader);
        glAttachShader(*dest, tessellation_evaluation_shader);
    }
    if (geometry_shader_source) {
        create_shader(geometry_shader_source, GL_GEOMETRY_SHADER, &geometry_shader);
        glAttachShader(*dest, geometry_shader);
    }
    if (fragment_shader_source) {
        create_shader(fragment_shader_source, GL_FRAGMENT_SHADER, &fragment_shader);
        glAttachShader(*dest, fragment_shader);
    }

    glLinkProgram(*dest);

    i32 succes = 0;
    glGetProgramiv(*dest, GL_LINK_STATUS, &succes);
    if (succes != GL_TRUE) {
        fprintf(stderr, "SHADER LINK ERROR\n");
        exit(1);
    }

    if (vertex_shader_source) {
        glDeleteShader(vertex_shader);
    }
    if (tessellation_control_shader_source) {
        glDeleteShader(tessellation_control_shader);
    }
    if (tessellation_evaluation_shader_source) {
        glDeleteShader(tessellation_evaluation_shader);
    }
    if (geometry_shader_source) {
        glDeleteShader(geometry_shader);
    }
    if (fragment_shader_source) {
        glDeleteShader(fragment_shader);
    }
}
