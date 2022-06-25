#version 460 core

layout (location = 0) out vec4 FragColor;

in FRAG_IN {
    vec2 uv;
    vec3 dir;
    vec2 pny;
} frag_in;

struct HitResult {
    vec2 result;
    bool valid;
};

#define PI 3.141592

uniform vec3 cam_origin;
uniform vec2 rotation; // x pitch, y yaw

HitResult cube_intersect(vec3 ray_origin, vec3 ray_dir, vec3 cube_pos, vec3 cube_dim){
    HitResult result;
    // https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection
    float tmin = (cube_pos.x - cube_dim.x - ray_origin.x) / ray_dir.x;
    float tmax = (cube_pos.x + cube_dim.y - ray_origin.x) / ray_dir.x;

    // swap(tmin, tmax);
    if (tmin > tmax) {
        float temp = tmin;
        tmin = tmax;
        tmax = temp;
    }

    float tymin = (cube_pos.y - cube_dim.y - ray_origin.y) / ray_dir.y;
    float tymax = (cube_pos.y + cube_dim.y - ray_origin.y) / ray_dir.y;

    if (tymin > tymax) {
        float temp = tymin;
        tymin = tymax;
        tymax = temp;
    }


    if ((tmin > tymax) || (tymin > tmax)){
        result.valid = false;
        return result;
    }

    if (tymin > tmin)
        tmin = tymin;

    if (tymax < tmax)
        tmax = tymax;

    float tzmin = (cube_pos.z - cube_dim.z - ray_origin.z) / ray_dir.z;
    float tzmax = (cube_pos.z + cube_dim.z - ray_origin.z) / ray_dir.z;

    if (tzmin > tzmax) {
        float temp = tzmin;
        tzmin = tzmax;
        tzmax = temp;
    }



    if ((tmin > tzmax) || (tzmin > tmax)){
        result.valid = false;
        return result;
    }

    if (tzmin > tmin)
        tmin = tzmin;

    if (tzmax < tmax)
        tmax = tzmax;

    result.valid = true;
    result.result.x = tmin;
    result.result.y = tmax;

    return result;
}

HitResult solve_quadratic_form(float a, float b, float c){
    HitResult result;
    float disc = b*b - 4*a*c;
    if (disc < 0) {
        result.valid = false;
        return result;
    };
    result.result.x = (-b + sqrt(disc)) / (2 * a);
    result.result.y = (-b - sqrt(disc)) / (2 * a);
    result.valid = true;

    return result;
}

HitResult sphere_intersect(vec3 ray_origin, vec3 ray_dir, vec4 sphere){
    HitResult result;
    // vec4 circle = vec4(0.0, 0.0, 4.0, 1.0); // x, y, z, position, w (4th) is radius
    // ^ just want to test if this works def will not hardcode it like this in the future, figure something out idk

    vec3 L = ray_origin - sphere.xyz;
    float a = dot(ray_dir, ray_dir);
    float b = 2 * dot(ray_dir, L);
    float dist = length(ray_origin - sphere.xyz);
    float c = dist*dist - sphere.w * sphere.w;

    result = solve_quadratic_form(a, b, c);
    return result;
}

HitResult intersect(vec3 ray_origin, vec3 ray_dir){
    HitResult return_value;
// #if 0
//     vec4 circle = vec4(0.0, 0.0, 4.0, 1.0); // x, y, z, position, w (4th) is radius
//     // ^ just want to test if this works def will not hardcode it like this in the future, figure something out idk

//     vec3 L = ray_origin - circle.xyz;
//     float a = dot(ray_dir, ray_dir);
//     float b = 2 * dot(ray_dir, L);
//     float dist = length(ray_origin - circle.xyz);
//     float c = dist*dist - circle.w * circle.w;

//     return_value = solve_quadratic_form(a, b, c);
// #else
//     vec3 cube_pos = vec3(1.5, 0.0, 4.0);
//     vec3 cube_dim = vec3(1.0, 1.0, 1.0);

//     // https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection
//     float tmin = (cube_pos.x - cube_dim.x - ray_origin.x) / ray_dir.x;
//     float tmax = (cube_pos.x + cube_dim.y - ray_origin.x) / ray_dir.x;

//     // swap(tmin, tmax);
//     if (tmin > tmax) {
//         float temp = tmin;
//         tmin = tmax;
//         tmax = temp;
//     }

//     float tymin = (cube_pos.y - cube_dim.y - ray_origin.y) / ray_dir.y;
//     float tymax = (cube_pos.y + cube_dim.y - ray_origin.y) / ray_dir.y;

//     // if (tymin > tymax) swap(tymin, tymax);
//     if (tymin > tymax) {
//         float temp = tymin;
//         tymin = tymax;
//         tymax = temp;
//     }


//     if ((tmin > tymax) || (tymin > tmax)){
//         return_value.valid = false;
//         return return_value;
//     }

//     if (tymin > tmin)
//         tmin = tymin;

//     if (tymax < tmax)
//         tmax = tymax;

//     float tzmin = (cube_pos.z - cube_dim.z - ray_origin.z) / ray_dir.z;
//     float tzmax = (cube_pos.z + cube_dim.z - ray_origin.z) / ray_dir.z;

//     // if (tzmin > tzmax) swap(tzmin, tzmax);
//     if (tzmin > tzmax) {
//         float temp = tzmin;
//         tzmin = tzmax;
//         tzmax = temp;
//     }



//     if ((tmin > tzmax) || (tzmin > tmax)){
//         return_value.valid = false;
//         return return_value;
//     }

//     if (tzmin > tmin)
//         tmin = tzmin;

//     if (tzmax < tmax)
//         tmax = tzmax;

//     return_value.valid = true;
//     return_value.result.x = tmin;
//     return_value.result.y = tmax;

//     return return_value;
// #endif
    return_value = cube_intersect(ray_origin, ray_dir, vec3(0.0), vec3(1.0));

    if (return_value.result.x > return_value.result.y) return_value.result.xy = return_value.result.yx;

    return return_value;

}

void main(){
    // vec2 screen_pos = frag_in.uv * 2.0 - 1.0;
    // float pitch, yaw;
    // pitch = rotation.y + screen_pos.y*0.25*PI;
    // yaw = rotation.x + screen_pos.x*0.25*PI;

    // float d = cos(pitch);

    // vec3 actual_dir = vec3(
    //     sin(-yaw) * d,
    //     sin(pitch),
    //     cos(yaw) * d
    // );

    vec3 frag_dir = normalize(frag_in.dir);
    vec3 ray_dir = vec3(frag_in.uv*2.0-1.0, 1.0);
    ray_dir = normalize(ray_dir);
    // Take camera rotation into account, by rotation matrix or another way
    // HitResult hit = intersect(cam_origin, actual_dir);
    // HitResult hit = intersect(cam_origin, frag_dir);
    HitResult hit = intersect(cam_origin, ray_dir);

    vec3 col = vec3(0.0);

    if (hit.valid && hit.result.y > 0.0) col = vec3((hit.result.y - hit.result.x)/2) + 0.2;

    // FragColor = vec4(frag_in.uv, 0.0, 1.0);
    FragColor = vec4(vec3(length(frag_dir)), 1.0);
    FragColor = vec4(col, 1.0);
    // FragColor = vec4(frag_in.pny-vec2(pitch, yaw), 0.0, 1.0);
}
