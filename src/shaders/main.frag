#version 460 core

layout (location = 0) out vec4 FragColor;

layout (std430, binding = 0) readonly buffer objects {
    float data[];
};

in FRAG_IN {
    vec2 uv;
    mat3 rot;
} frag_in;

struct HitResult {
    vec2 result;
    bool valid;
};

struct ObjectHit {
    vec2 result;
    bool valid;
    uint object_index;
};

#define PI 3.14159265

uniform vec3 cam_origin;
uniform vec3 cam_for;

uniform sampler3D cube_density_texture;

HitResult cube_intersect(vec3 ray_origin, vec3 ray_dir, vec3 cube_pos, vec3 cube_dim) {
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


    if (tmin > tymax || tymin > tmax) {
        result.valid = false;
        return result;
    }

    if (tymin > tmin) {
        tmin = tymin;
    }
    if (tymax < tmax) {
        tmax = tymax;
    }

    float tzmin = (cube_pos.z - cube_dim.z - ray_origin.z) / ray_dir.z;
    float tzmax = (cube_pos.z + cube_dim.z - ray_origin.z) / ray_dir.z;

    if (tzmin > tzmax) {
        float temp = tzmin;
        tzmin = tzmax;
        tzmax = temp;
    }

    if (tmin > tzmax || tzmin > tmax) {
        result.valid = false;
        return result;
    }

    if (tzmin > tmin) {
        tmin = tzmin;
    }
    if (tzmax < tmax) {
        tmax = tzmax;
    }

    result.valid = true;
    result.result.x = tmin;
    result.result.y = tmax;

    if (result.result.x > result.result.y) {
        result.result.xy = result.result.yx;
    }

    return result;
}

HitResult solve_quadratic_form(float a, float b, float c) {
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

HitResult sphere_intersect(vec3 ray_origin, vec3 ray_dir, vec4 sphere) {
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

ObjectHit intersect(vec3 ray_origin, vec3 ray_dir) {
    ObjectHit return_value = ObjectHit(
        vec2(0.0), // Result, doesn't have one
        false, // Valid, false as default
        1 // object_index, where the floats start
    );

    uint index = 0;

    // for (int i = 0; i < 2; i++){
    //     vec4 sphere_data = vec4(
    //         data[index + 0],
    //         data[index + 1],
    //         data[index + 2],
    //         data[index + 3]
    //     );
    //     HitResult cur_result = sphere_intersect(ray_origin, ray_dir, sphere_data);

    //     if (cur_result.valid && cur_result.result.y > 0.0 && // if the hit is valid
    //         (!return_value.valid || // if we've hit something already
    //         cur_result.result.x < return_value.result.x)) { // if this hit is actually closer than the other one
    //             return_value.result = cur_result.result;
    //             return_value.valid = true;
    //             return_value.object_index = index;
    //     }
    //     index += 4;
    // }

    // /*
    // ======================== FUCKY SHIT ================================== //

    // Spheres
    // while (data[index] != OBJECT_SPLIT){
    // while (false){

    uint sphere_count = int(data[index]);
    index++;

    for (int i = 0; i < sphere_count; i++) {
        vec4 sphere_data = vec4(
            data[index + 0],
            data[index + 1],
            data[index + 2],
            data[index + 3]
        );
        HitResult cur_result = sphere_intersect(ray_origin, ray_dir, sphere_data);

        if (cur_result.valid && cur_result.result.y > 0.0 && // if the hit is valid
            (!return_value.valid || // if we've hit something already
            cur_result.result.x < return_value.result.x)) {
                return_value.result = cur_result.result;
                return_value.valid = true;
                return_value.object_index = index;
        }

        index += 4;
    }

    uint cube_count = int(data[index]);
    index++;
    // A split was detected on the spheres, now onto cubes
    // Cubes
    // while (data[index] != OBJECT_SPLIT){
    // while (false){
    for (int i = 0; i < cube_count; i++) {
        vec3 cube_pos_data = vec3(
            data[index + 0],
            data[index + 1],
            data[index + 2]
        );
        vec3 cube_dim_data = vec3(
            data[index + 3],
            data[index + 4],
            data[index + 5]
        );

        HitResult cur_result = cube_intersect(ray_origin, ray_dir, cube_pos_data, cube_dim_data);

        if (cur_result.valid && cur_result.result.y > 0.0 &&
            (!return_value.valid ||
            cur_result.result.x < return_value.result.x)) {
                return_value.result = cur_result.result;
                return_value.valid = true;
                return_value.object_index = index;
        }

        index += 6;
    }

    // index++;

    // ======================== END OF FUCKY SHIT ================================== //

    if (return_value.result.x > return_value.result.y) {
        return_value.result.xy = return_value.result.yx;
    }

    return return_value;
}

void main(){
    vec3 ray_dir = vec3(frag_in.uv*2.0-1.0, 1.0);
    ray_dir = normalize(ray_dir);
    ray_dir *= frag_in.rot;
    ObjectHit hit = ObjectHit(vec2(0.0), true, 0);
    float dist = 0.0;
    float ys = 0.0;
    while (hit.valid && hit.result.y >= 0.0) {
        dist += hit.result.y - hit.result.x;
        ys += hit.result.y + 0.01;
        hit = intersect(cam_origin + ray_dir * ys, ray_dir);
    }
    /*
    ObjectHit hit = ObjectHit(
        vec2(0.0),
        true,
        0
    );
    */
    vec3 col = vec3(dist / 2);

    // col = vec3(0.0, 1.0, 0.0);

    FragColor = vec4(col, 1.0);
}
