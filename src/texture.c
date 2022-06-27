#include <stdlib.h>
#include <standard_types.h>
#include <glad/gl.h>

void create_texture3D(
    u32 width, u32 height, u32 depth,
    u8 (*function)(float x, float y, float z),
    u32* texture
){
    glCreateTextures(GL_TEXTURE_3D, 1, texture);

    glTextureStorage3D(*texture, 1, GL_R8, width, height, depth);

    u8* tex_buf = malloc(width * height * depth); // * sizeof(u8) -> 1

    for (u32 z = 0; z < depth; z++){
        for (u32 y = 0; y < height; y++){
            for (u32 x = 0; x < width; x++){
                u64 index = z * width * height + y * width + x;
                // maybe u64 is too big, idk multiplying three times gets big fast
                float x_coord = x / (float)width;
                float y_coord = y / (float)height;
                float z_coord = z / (float)depth;
                tex_buf[index] = function(x_coord, y_coord, z_coord);
            }
        }
    }

    glTextureSubImage3D(
        *texture,                   // Specify the texture
        0,                          // The texture level
        0, 0, 0,                    // x, y, z offsets
        width, height, depth,       // width, height and depth information
        GL_RED, GL_UNSIGNED_BYTE,   // specify the types of the data, (these are fucky sometimes)
        tex_buf                     // the actual data
    );

    glTextureParameteri(*texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(*texture, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(*texture, GL_TEXTURE_WRAP_R, GL_REPEAT);

    glTextureParameteri(*texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(*texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    free(tex_buf);
}
