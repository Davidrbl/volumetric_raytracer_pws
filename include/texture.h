#include <standard_types.h>

void create_texture3D(
    u32 width, u32 height, u32 depth,
    u8 (*function)(float x, float y, float z),
    u32* texture
);
