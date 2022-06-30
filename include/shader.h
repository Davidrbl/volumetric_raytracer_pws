#ifndef SHADER_H
#define SHADER_H

#include <glad/gl.h>

#include <standard_types.h>

void create_shader(const char* path, GLenum type, u32* dest);

void create_program2s(
    const char* vertex_shader_path,
    const char* fragment_shader_path,
    u32* dest
);

void create_program3s(
    const char* vertex_shader_path,
    const char* geometry_shader_path,
    const char* fragment_shader_path,
    u32* dest
);

void create_program(
    const char* compute_shader_path,
    const char* vertex_shader_path,
    const char* tessellation_control_shader_path,
    const char* tessellation_evaluation_shader_path,
    const char* geometry_shader_path,
    const char* fragment_shader_path,
    u32* dest
);

#endif // SHADER_H
