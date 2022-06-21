#pragma once
#include <stdint.h>

#include <glad/gl.h>

void create_shader(const char* path, GLenum type, GLint* dest);

void create_program2s(const char* vertex_shader_path, const char* fragment_shader_path, GLint* dest);

void create_program3s(const char* vertex_shader_path, const char* geometry_shader_path, const char* fragment_shader_path, GLint* dest);

void create_program(const char* vertex_shader_path,
                    const char* tessellation_control_shader_path,
                    const char* tessellation_evaluation_shader_path,
                    const char* geometry_shader_path,
                    const char* fragment_shader_path,
                    GLint* dest);
