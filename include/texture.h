#ifndef TEXTURE_H
#define TEXTURE_H

#include <standard_types.h>

void create_texture3D(
    i32 width, i32 height, i32 depth,
    float (*function)(float x, float y, float z),
    u32* texture
);

void create_cubemap(const char* adresses[6], u32* dest);
#endif // TEXTURE_H
