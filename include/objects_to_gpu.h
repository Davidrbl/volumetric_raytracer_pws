#include <stdlib.h>
#include <stdio.h>
#include <standard_types.h>
#include <assert.h>

// #define CONSTRUCT_TEMPLATE_VEC(typename) \
//     typedef struct { \
//         ##typename * data \
//         u32 buf_len \
//         u32 len \
//     } Vec_##typename \
//     \
//     void ADD_ITEM_VEC_##typename (##typename item, Vec_##typename * vec){ \
//         if (vec->len == vec->buf_len){  \
//             vec->buf_len += 5*sizeof(##typename );  \
//             vec->data = realloc(vec->data, vec->buf_len);   \
//             assert(vec->data);  \
//             vec->len++; \
//             vec->data[vec->len] = item; \
//         } else { \
//             vec->len++; \
//             vec->data[vec->len] = item; \
//         }   \
//     }   \

typedef struct {
    float pos[3];
    float radius;
} Sphere;

typedef struct {
    float pos[3];
    float dim[3];
} Cube;

typedef struct {
    float pos[3];
    float dim[3];
    float density_textureID;
    /*
        ^ Ok this is kinda fucky, havind a float as an ID, but lemme explain
        we can only send over one datatype to the gpu with this, bc glsl is fucky like that
        glsl has no type ambiguity, as far as i know,
        so we send something that is typically a u32 as a float, and
        in the shader we just int() that mf

        well see how it goes
    */
} Vol_Cube;

// typedef struct {
//     Sphere* data;

// }

typedef struct {

} Scene;

void add_sphere_to_scene();
