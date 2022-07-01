#ifndef SHADER_H
#define SHADER_H

#include <glad/gl.h>

#include <standard_types.h>

void create_shader(const GLchar* source, GLenum type, u32* dest);

void create_program_compute(const GLchar* compute_shader_source, u32* dest);

void create_program(
    const GLchar* vertex_shader_source,
    const GLchar* tessellation_control_shader_source,
    const GLchar* tessellation_evaluation_shader_source,
    const GLchar* geometry_shader_source,
    const GLchar* fragment_shader_source,
    u32* dest
);

#endif // SHADER_H
