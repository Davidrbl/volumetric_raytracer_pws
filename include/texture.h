#ifndef TEXTURE_H
#define TEXTURE_H

#include <standard_types.h>

void create_texture3D(
    i32 width, i32 height, i32 depth,
    u8 (*function)(float x, float y, float z),
    u32* texture
);

#endif // TEXTURE_H
