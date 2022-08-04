#include <stdlib.h>

#include <glad/gl.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <texture.h>

void create_texture3D(
    i32 width, i32 height, i32 depth,
    float (*function)(float x, float y, float z),
    u32* texture
) {
    glCreateTextures(GL_TEXTURE_3D, 1, texture);

    glTextureStorage3D(*texture, 1, GL_R32F, width, height, depth);

    float* tex_buf = malloc(width * height * depth * sizeof(float));

    for (i32 z = 0; z < depth; z++) {
        for (i32 y = 0; y < height; y++) {
            for (i32 x = 0; x < width; x++) {
                i32 index = z * width * height + y * width + x;
                // maybe u64 is too big, idk multiplying three times gets big fast
                // (finlay) i32 fits easily; 1290x1290x1290 is the max
                float x_coord = (float)x / (float)width;
                float y_coord = (float)y / (float)height;
                float z_coord = (float)z / (float)depth;
                tex_buf[index] = function(x_coord, y_coord, z_coord);
            }
        }
    }

    glTextureSubImage3D(
        *texture,                   // Specify the texture
        0,                          // The texture level
        0, 0, 0,                    // x, y, z offsets
        width, height, depth,       // width, height and depth information
        GL_RED, GL_FLOAT,           // specify the types of the data, (these are fucky sometimes)
        tex_buf                     // the actual data
    );

    glTextureParameteri(*texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(*texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(*texture, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glTextureParameteri(*texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(*texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    free(tex_buf);
}

void create_cubemap(const char* adresses[6], u32* dest){
    /*
    Right,
    Left,
    Top,
    Bottom,
    Back,
    Front
    */
  glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, dest);

  i32 width, height, channels;
//   GLint internal_format = GL_RGB;

  for (int i = 0; i < 6; i++){
    unsigned char* image_data = stbi_load(adresses[i], &width, &height, &channels, STBI_rgb_alpha);

    if (i == 0) glTextureStorage2D(*dest, 1, GL_RGB8, width, height);

    // if (channels == 4) internal_format = GL_RGBA;
    // else if (channels == 3) internal_format = GL_RGB;

    if (image_data == NULL) {
      printf("Cubemap texture loading error\n");
      exit(1);
    }

    glTextureSubImage3D(*dest,
                        0,
                        0,
                        0,
                        i,
                        width,
                        height,
                        1,
                        GL_RGBA,
                        GL_UNSIGNED_BYTE,
                        image_data);

    stbi_image_free(image_data);
  }

  glTextureParameteri(*dest, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTextureParameteri(*dest, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTextureParameteri(*dest, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTextureParameteri(*dest, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTextureParameteri(*dest, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glGenerateTextureMipmap(*dest);
}
