#include <stdio.h>
#include <stdlib.h>

#include <glad/gl.h>

#include <shader.h>
#include <standard_types.h>

void create_shader(const char* path, GLenum type, GLint* dest){
    *dest = glCreateShader(type);

    FILE* file_pointer = fopen(path, "rb");

    if (!file_pointer) exit(42);

    fseek(file_pointer, 0, SEEK_END);

    u32 file_len = ftell(file_pointer);

    fseek(file_pointer, 0, SEEK_SET);

    char* source = calloc(file_len+1, sizeof(char));

    fread(source, sizeof(char), file_len, file_pointer);

    fclose(file_pointer);

    glShaderSource(*dest, 1, (const GLchar* const *)&source, NULL);

    glCompileShader(*dest);

    i32 succes;
    glGetShaderiv(*dest, GL_COMPILE_STATUS, &succes);
    if (succes != GL_TRUE){
        int error_len;
        glGetShaderiv(*dest, GL_INFO_LOG_LENGTH, &error_len);
        char error_string[error_len + 1];
        glGetShaderInfoLog(*dest, error_len, &error_len, error_string);

        glDeleteShader(*dest);

        fprintf(stderr, "SHADER COMPILE ERROR: %s\n%s\n", path, error_string);
        exit(1);
    }

    free(source);
}

void create_program2s(const char* vertex_shader_path, const char* fragment_shader_path, GLint* dest){
    *dest = glCreateProgram();

    GLint vertex_shader, fragment_shader;
    create_shader(vertex_shader_path,   GL_VERTEX_SHADER,   &vertex_shader);
    create_shader(fragment_shader_path, GL_FRAGMENT_SHADER, &fragment_shader);


    glAttachShader(*dest, vertex_shader);
    glAttachShader(*dest, fragment_shader);

    glLinkProgram(*dest);

    i32 succes;
    glGetProgramiv(*dest, GL_LINK_STATUS, &succes);
    if (succes != GL_TRUE){
        fprintf(stderr, "SHADER LINK ERROR\n");
        exit(1);
    }
}

void create_program3s(const char* vertex_shader_path, const char* geometry_shader_path, const char* fragment_shader_path, GLint* dest){
    *dest = glCreateProgram();

    GLint vertex_shader, geometry_shader, fragment_shader;
    create_shader(vertex_shader_path,   GL_VERTEX_SHADER,   &vertex_shader);
    create_shader(geometry_shader_path, GL_GEOMETRY_SHADER, &geometry_shader);
    create_shader(fragment_shader_path, GL_FRAGMENT_SHADER, &fragment_shader);


    glAttachShader(*dest, vertex_shader);
    glAttachShader(*dest, geometry_shader);
    glAttachShader(*dest, fragment_shader);

    glLinkProgram(*dest);

    i32 succes;
    glGetProgramiv(*dest, GL_LINK_STATUS, &succes);
    if (succes != GL_TRUE){
        fprintf(stderr, "SHADER LINK ERROR\n");
        exit(1);
    }
}

void create_program(const char* vertex_shader_path,
                    const char* tessellation_control_shader_path,
                    const char* tessellation_evaluation_shader_path,
                    const char* geometry_shader_path,
                    const char* fragment_shader_path,
                    GLint* dest)
{
    *dest = glCreateProgram();

    i32 vertex_shader;
    i32 tessellation_control_shader;
    i32 tessellation_evaluation_shader;
    i32 geometry_shader;
    i32 fragment_shader;

    if (vertex_shader_path){
        create_shader(vertex_shader_path, GL_VERTEX_SHADER, &vertex_shader);
        glAttachShader(*dest, vertex_shader);
    }
    if (tessellation_control_shader_path){
        create_shader(tessellation_control_shader_path, GL_TESS_CONTROL_SHADER, &tessellation_control_shader);
        glAttachShader(*dest, tessellation_control_shader);
    }
    if (tessellation_evaluation_shader_path){
        create_shader(tessellation_evaluation_shader_path, GL_TESS_EVALUATION_SHADER, &tessellation_evaluation_shader);
        glAttachShader(*dest, tessellation_evaluation_shader);
    }
    if (geometry_shader_path){
        create_shader(geometry_shader_path, GL_GEOMETRY_SHADER, &geometry_shader);
        glAttachShader(*dest, geometry_shader);
    }
    if (fragment_shader_path){
        create_shader(fragment_shader_path, GL_FRAGMENT_SHADER, &fragment_shader);
        glAttachShader(*dest, fragment_shader);
    }

    glLinkProgram(*dest);

    i32 succes;
    glGetProgramiv(*dest, GL_LINK_STATUS, &succes);
    if (succes != GL_TRUE){
        fprintf(stderr, "SHADER LINK ERROR\n");
        exit(1);
    }

    if (vertex_shader_path) glDeleteShader(vertex_shader);
    if (tessellation_control_shader_path) glDeleteShader(tessellation_control_shader);
    if (tessellation_evaluation_shader_path) glDeleteShader(tessellation_evaluation_shader);
    if (geometry_shader_path) glDeleteShader(geometry_shader);
    if (fragment_shader_path) glDeleteShader(fragment_shader);
}
