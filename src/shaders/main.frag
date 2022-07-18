#version 450 core

layout (location = 0) out vec4 FragColor;

layout (std430, binding = 0) readonly buffer objects {
    float data[];
};

struct Light {
    vec3 pos;
    float intensity;
};

layout (std430, binding = 1) readonly buffer light_buffer
{
    uint num_lights;
    Light lights[];
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
    uint object_type;
};

#define PI 3.141592

//#define VERY_SMALL_NUM 0.000000357635468273628 // unnecessarily small
#define SMALL_NUM 0.000015
#define SHADOW_BIAS 0.0001

#define MAX_PASS_THROUGH 10

#define OBJECT_TYPE_NONE        0
#define OBJECT_TYPE_SPHERE      1
#define OBJECT_TYPE_CUBE        2
#define OBJECT_TYPE_VOL_CUBE    3

uniform vec3 cam_origin;
uniform vec3 cam_for;

// uniform sampler3D cube_density_texture;
uniform samplerCube skybox_texture;

uniform float time;

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
    result.result.x = (-b - sqrt(disc)) / (2 * a);
    result.result.y = (-b + sqrt(disc)) / (2 * a);
    result.valid = true;

    return result;
}

HitResult sphere_intersect(vec3 ray_origin, vec3 ray_dir, vec4 sphere) {
    HitResult result;

    vec3 L = ray_origin - sphere.xyz;
    float a = dot(ray_dir, ray_dir);
    float b = 2 * dot(ray_dir, L);
    float dist = length(L);
    float c = dist*dist - sphere.w * sphere.w;

    result = solve_quadratic_form(a, b, c);
    return result;
}

ObjectHit intersect(vec3 ray_origin, vec3 ray_dir) {
    ObjectHit return_value = ObjectHit(
        vec2(0.0), // Result, doesn't have one
        false, // Valid, false as default
        1, // object_index, where the floats start
        OBJECT_TYPE_NONE
    );

    uint index = 0;

    uint sphere_count = floatBitsToUint(data[index]);
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
            return_value.object_type = OBJECT_TYPE_SPHERE;
        }

        index += 4;
    }

    uint cube_count = floatBitsToUint(data[index]);
    index++;
    // A split was detected on the spheres, now onto cubes
    // Cubes
    for (int i = 0; i < cube_count; i++){
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
            return_value.object_type = OBJECT_TYPE_CUBE;
        }

        index += 6;
    }

    uint vol_cube_count = uint(data[index]);
    index++;

    for (int i = 0 ; i < vol_cube_count; i++){
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
            return_value.object_type = OBJECT_TYPE_VOL_CUBE;
        }

        index += 7; // One is for density of cube, not needed for collision
    }

    return return_value;
}

vec3 shading_at_point(vec3 point, vec3 normal, vec3 base_col, vec3 albedo){
    vec3 total = vec3(0.0);

    uint light_count = 1;

    Light la[1] = {
        Light(vec3(5.0, 0.0, 0.0), 10.0)
        // Light(vec3(sin(time * 5.0) * 5.0, 0.0, cos(time * 5.0) * 5.0), 10.0)
    };

    // for (uint i = 0; i < num_lights; i++){
    for (uint i = 0; i < light_count; i++){
        Light l = la[i];
        vec3 light_dir = normalize(l.pos - point);
        ObjectHit hit = intersect(point + normal * SHADOW_BIAS, light_dir);
        if (hit.valid && hit.result.y > 0.0) continue;

        float power = max(dot(light_dir, normal), 0.0) * l.intensity;
        vec3 reflected = albedo / PI;
        total += base_col * reflected * power;
    }
    return total;
}

vec3 ray_color(vec3 ray_origin, vec3 ray_dir){
    vec3 color = vec3(0.0);

    float influence = 1.0;

    while (influence > 0.0){
        ObjectHit hit = intersect(ray_origin, ray_dir);
        vec3 hit_pos = ray_origin + ray_dir * hit.result.x;
        vec3 normal = vec3(0.0);
        vec3 shading = vec3(0.0);

        switch (hit.object_type){
            case OBJECT_TYPE_SPHERE:
                vec3 sphere_pos = vec3(
                    data[hit.object_index + 0],
                    data[hit.object_index + 1],
                    data[hit.object_index + 2]
                );

                float r = data[hit.object_index + 3];

                normal = (hit_pos - sphere_pos) / r;

                // vec3 c = texture(skybox_texture, reflect(ray_dir, normal)).rgb;

                shading = shading_at_point(hit_pos, normal, vec3(1.0, 0.0, 0.0), vec3(0.18));
                color += shading * influence;
                influence = 0.0;
                break;

            case OBJECT_TYPE_CUBE:
                vec3 cube_pos = vec3(
                    data[hit.object_index + 0],
                    data[hit.object_index + 1],
                    data[hit.object_index + 2]
                );
                vec3 cube_dim = vec3(
                    data[hit.object_index + 3],
                    data[hit.object_index + 4],
                    data[hit.object_index + 5]
                );

                vec3 rel_hit_pos = hit_pos - cube_pos;
                rel_hit_pos /= cube_dim;

                float pos[3] = {rel_hit_pos.x, rel_hit_pos.y, rel_hit_pos.z};
                uint highest_index = 0;

                float highest = 0.0;

                for (int i = 0; i < 3; i++){
                    if (abs(pos[i]) > abs(highest)){
                        highest_index = i;
                        highest = pos[i];
                    }
                }

                normal = vec3(0.0);

                normal.x += pos[0] / abs(pos[0]) * int(highest_index == 0);
                normal.y += pos[1] / abs(pos[1]) * int(highest_index == 1);
                normal.z += pos[2] / abs(pos[2]) * int(highest_index == 2);

                // vec3 reflected_c = texture(skybox_texture, reflect(ray_dir, normal)).rgb;

                shading = shading_at_point(hit_pos, normal, vec3(0.0, 1.0, 0.0), vec3(0.18));
                color += shading * influence;
                influence = 0.0;
                break;

            case OBJECT_TYPE_VOL_CUBE:
                vec3 cube_col;
                float alpha = 0.0;
                cube_col = vec3(0.0, 0.0, 1.0);
                float dist = hit.result.y - max(hit.result.x, 0.0); // Test value, we need to calculate this

                float half_thickness = data[hit.object_index + 6];

                alpha = 1.0 - pow(0.5, dist / half_thickness);

                alpha = clamp(alpha, 0.0, 1.0);

                float next_depth = hit.result.y + SMALL_NUM;
                ray_origin = ray_origin + ray_dir * next_depth;

                color += cube_col * alpha * influence;        // This is the volumetric cube part
                influence *= 1.0 - alpha;

                break;

            case OBJECT_TYPE_NONE:
                vec3 skybox_col = texture(skybox_texture, ray_dir).rgb;
                color += skybox_col * influence;
                influence = 0.0;
                break;
        }
    }

    return color;
}

void main() {
    vec3 ray_dir = normalize(vec3(frag_in.uv*2.0-1.0, 1.0));
    ray_dir *= frag_in.rot;
    vec3 col = ray_color(cam_origin, ray_dir);

    FragColor = vec4(col, 1.0);
}
