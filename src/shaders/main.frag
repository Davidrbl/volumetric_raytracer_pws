#version 450 core

layout (location = 0) out vec4 FragColor;

layout (std430, binding = 0) readonly buffer objects {
    float data[];
};

struct Light {
    vec3 pos;
    float power;
};

layout (std430, binding = 1) readonly buffer light_buffer
{
    float lights[];
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

#define NUM_SAMPLE_STEPS 40
#define NUM_IN_SCATTERING_SAMPLE_STEPS 8

#define TRANSMITTANCE_MUL 1.0
#define IN_SCAT_MUL 0.7

#define OBJECT_TYPE_NONE        0
#define OBJECT_TYPE_SPHERE      1
#define OBJECT_TYPE_CUBE        2
#define OBJECT_TYPE_VOL_CUBE    3

uniform vec3 cam_origin;
uniform vec3 cam_for;

uniform sampler3D cube_density_texture;
uniform samplerCube skybox_texture;

uniform float time;

vec3 clamp_color(vec3 c){
    return vec3(
        clamp(c.x, 0.0, 1.0),
        clamp(c.y, 0.0, 1.0),
        clamp(c.z, 0.0, 1.0)
    );
}

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

    uint vol_cube_count = floatBitsToUint(data[index]);
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

// vec4 col_through_vol_cube(
//     vec3 begin,
//     vec3 end,
//     // begin and end are NOT divided by the cube_dim
//     vec3 vol_cube_pos,
//     vec3 vol_cube_dim,
//     sampler3D density_texture
// ){
//     vec4 return_value = vec4(1.0, 1.0, 1.0, 1.0);
    // // Do the in-scattering
    // // return_value.a = 1.0;
    // // NOTE: (david) do we do this light major or sample point major?
    // // for (l in lights){
    // //     for (p in sample_points){
    // //         do stuff
    // //     }
    // // }
    // // or ...
    // // for (p in sample_points){
    // //    for (l in lights){
    // //         do stuff
    // //     }
    // // }
    // // gonna do this light-major for now, but i don't know,
    // // we could check performance differences between both
    // uint index = 0;
//     uint num_dir_lights = floatBitsToUint(lights[index++]);

    // vec3 uv_begin = begin - vol_cube_pos;
    // uv_begin /= vol_cube_dim;
    // uv_begin = uv_begin / 2.0 - 1.0;

    // vec3 uv_end = end - vol_cube_pos;
    // uv_end /= vol_cube_dim;
    // uv_end = uv_end / 2.0 - 1.0;

// 	float transmittance = 1.0;
//     for (uint i = 0; i < num_dir_lights; i++){
//         Light l = Light(
//             vec3(
//                 lights[index++],
//                 lights[index++],
//                 lights[index++]
//             ),
//             lights[index++]
//         );

//         vec3 sample_step = (uv_end - uv_begin) / NUM_SAMPLE_STEPS;
//         float sample_length = length(uv_end - uv_begin) / NUM_SAMPLE_STEPS;


//         vec3 sample_pos = uv_begin;
//         for (uint j = 0; j < NUM_SAMPLE_STEPS; j++){

//             vec3 sample_pos_world = sample_pos * 2.0 - 1.0;
//             sample_pos_world *= vol_cube_dim;
// 			return vec4(sample_pos_world, 1.0);
//             sample_pos_world += vol_cube_pos;
//             vec3 light_dir = normalize(l.pos - sample_pos_world);
//             HitResult hit = cube_intersect(sample_pos_world, light_dir, vol_cube_pos, vol_cube_dim);

//             if (!hit.valid) return vec4(1.0, 0.0, 1.0, 1.0); // Something's fucky wucky

//             vec3 in_scat_uv_begin = sample_pos;
//             vec3 in_scat_end = sample_pos_world + light_dir * hit.result.y;
//             vec3 in_scat_uv_end = in_scat_end - vol_cube_pos;
//             in_scat_uv_end /= vol_cube_dim;
//             in_scat_uv_end = in_scat_uv_end / 2.0 + 0.5;

//             vec3 in_scat_sample_pos = in_scat_uv_begin;
//             vec3 in_scat_sample_step = (in_scat_uv_end - in_scat_uv_begin) / NUM_IN_SCATTERING_SAMPLE_STEPS;
//             float in_scat_sample_length = length(in_scat_uv_end - in_scat_uv_begin) / NUM_IN_SCATTERING_SAMPLE_STEPS;

//             float in_scat_ray_transmittance = 1.0;

//             for (uint k = 0; k < NUM_IN_SCATTERING_SAMPLE_STEPS; k++){
//                 float in_scat_density = texture(density_texture, in_scat_sample_pos).r;

//                 float in_scat_ray_transmittance_part = pow(10, -in_scat_density * in_scat_sample_length);

//                 in_scat_ray_transmittance *= in_scat_ray_transmittance_part;

//                 in_scat_sample_pos += in_scat_sample_step;
//             }

// 		 float density = texture(density_texture, sample_pos).r;
// 		 float transmittance_part = pow(10, -density * sample_length);

// 		 transmittance *= transmittance_part;
// 		 return_value.rgb += in_scat_ray_transmittance * transmittance;
//         }
//     }

// 	return_value.a = 1.0 - transmittance;
// 	return return_value;
// }


float transmittance_through_texture(vec3 uv_begin, vec3 uv_end, sampler3D density_texture){
    float transmittance = 1.0;
    float sample_length = length(uv_begin - uv_end) / NUM_SAMPLE_STEPS;

    vec3 sample_step = (uv_end - uv_begin) / NUM_SAMPLE_STEPS;
    vec3 sample_pos = uv_begin;
    for (uint i = 0; i < NUM_SAMPLE_STEPS; i++){
        float density = texture(density_texture, sample_pos).x;

        float transmittance_part = pow(10, -density * sample_length);

        transmittance *= transmittance_part;
        sample_pos += sample_step;
    }
    return transmittance;
}


vec4 col_through_vol_cube(
    vec3 begin,
    vec3 end,
    // begin and end are NOT divided by the cube_dim
    vec3 vol_cube_pos,
    vec3 vol_cube_dim,
    sampler3D density_texture
){
	vec3 vol_cube_col = vec3(0.0);
    vec3 uv_begin = begin - vol_cube_pos;
    uv_begin /= vol_cube_dim;
    uv_begin = uv_begin / 2.0 + 0.5;

    vec3 uv_end = end - vol_cube_pos;
    uv_end /= vol_cube_dim;
    uv_end = uv_end / 2.0 + 0.5;

    // Do the in-scattering
    // return_value.a = 1.0;
    // NOTE: (david) do we do this light major or sample point major?
    // for (l in lights){
    //     for (p in sample_points){
    //         do stuff
    //     }
    // }
    // or ...
    // for (p in sample_points){
    //    for (l in lights){
    //         do stuff
    //     }
    // }
    // gonna do this light-major for now, but i don't know,
    // we could check performance differences between both
	float transmittance = 1.0;
    float sample_length = length(uv_begin - uv_end) / NUM_SAMPLE_STEPS;

    vec3 sample_step = (uv_end - uv_begin) / NUM_SAMPLE_STEPS;
    vec3 sample_pos = uv_begin;

    uint index = 0;
    uint num_dir_lights = floatBitsToUint(lights[index++]);

    for (uint i = 0; i < num_dir_lights; i++){
        Light l = Light(
            vec3(
                lights[index++],
                lights[index++],
                lights[index++]
            ),
            lights[index++]
        );
        for (uint j = 0; j <= NUM_SAMPLE_STEPS; j++){
            float density = texture(density_texture, sample_pos).x;

            float transmittance_part = pow(10, -density * sample_length * TRANSMITTANCE_MUL);

            transmittance *= transmittance_part;

            vec3 sample_pos_world = sample_pos * 2.0 - 1.0;
            sample_pos_world *= vol_cube_dim;
            sample_pos_world += vol_cube_pos;
            vec3 light_dir = l.pos - sample_pos_world;
            HitResult hit = cube_intersect(
                (sample_pos * 2.0 - 1.0) * vol_cube_dim * (1.0 - SMALL_NUM), light_dir,    // Ray origin and direction
                vec3(0.0), vol_cube_dim                                // Vol_cube data
            );

            if (!hit.valid) return vec4(1.0, 0.0, 1.0, 1.0);

            vec3 in_scat_uv_begin = sample_pos;
            vec3 in_scat_uv_end = sample_pos + light_dir * hit.result.y;

            float transmittance_to_sample_pos = 1.0;
            vec3 light_ray_origin = sample_pos_world + light_dir * hit.result.y;
            ObjectHit light_ray = intersect(sample_pos_world + light_dir * hit.result.y, light_dir);
            while (light_ray.object_type == OBJECT_TYPE_VOL_CUBE && light_ray.result.x > 0.0){
                switch (light_ray.object_type){
                    case OBJECT_TYPE_VOL_CUBE:
                        vec3 vol_cube_pos = vec3(
                            data[light_ray.object_index + 0],
                            data[light_ray.object_index + 1],
                            data[light_ray.object_index + 2]
                        );

                        vec3 vol_cube_dim = vec3(
                            data[light_ray.object_index + 3],
                            data[light_ray.object_index + 4],
                            data[light_ray.object_index + 5]
                        );

                        vec3 begin = light_ray_origin + max(light_ray.result.x, 0.0) * light_dir - vol_cube_pos;
                        vec3 end = light_ray_origin +light_ray.result.y * light_dir - vol_cube_pos;

                        begin /= vol_cube_dim;
                        end /= vol_cube_dim;

                        begin = begin / 2.0 + 0.5;
                        end = end / 2.0 + 0.5;

                        float alpha = 1.0 - transmittance_through_texture(begin, end, cube_density_texture);
                        alpha = clamp(alpha, 0.0, 1.0);

                        float next_depth = light_ray.result.y + SMALL_NUM;
                        light_ray_origin += light_dir * next_depth;
                        transmittance_to_sample_pos *= 1.0 - alpha;
                        break;

                    default:
                        break;
                }
                light_ray= intersect(light_ray_origin, light_dir);
            }
            vec3 in_scat_sample_pos = in_scat_uv_begin;
            vec3 in_scat_sample_step = (in_scat_uv_end - in_scat_uv_begin) / NUM_IN_SCATTERING_SAMPLE_STEPS;

            float optical_depth = 0.0;

            for (uint k = 0; k < NUM_IN_SCATTERING_SAMPLE_STEPS; k++){
                float in_scat_density = texture(density_texture, in_scat_sample_pos).x;
                optical_depth += in_scat_density;
                in_scat_sample_pos += in_scat_sample_step;
            }
            // float in_scat_ray_transmittance = pow(10, -optical_depth * in_scat_sample_length);
            // ^ this is the same as the average density multiplied by the total length,
            // because opt_depth is avg * n, sample_length is length_total / n
            // what is said above is what is said in the tutorial of scatchapixel.com
            optical_depth /= NUM_IN_SCATTERING_SAMPLE_STEPS; // So this is now the average density along ray

            float in_scat_ray_length = length(in_scat_uv_end - in_scat_uv_begin);

            float in_scat_ray_transmittance = pow(10, -optical_depth * in_scat_ray_length * IN_SCAT_MUL);

            // The henyey_greenstein phase function returns the factor of light
            // that will be reflected at the angle between the light and camera
            const float g = 0.04; // some constant that changes the henyey_greenstein constant
            float HG_angle = dot(light_dir, -normalize(sample_step));
            float henyey_greenstein = 1 / (4*PI) * (1.0 - g*g) / pow(1 + g*g - 2*g * HG_angle, 1.5);

            vol_cube_col += vec3(1.0) * // Light color, always 1.0 with us
                            l.power * // divide this by 4*pi*r for normal lights
                            transmittance_to_sample_pos *
                            in_scat_ray_transmittance *
                            henyey_greenstein * // Henyey greenstein phase function
                            transmittance * // The transmittance up to this point
                            sample_length *
                            density;

            sample_pos += sample_step;
        }
    }
    return vec4(vol_cube_col, 1.0 - transmittance);
}
vec3 shading_at_point(vec3 point, vec3 normal, vec3 base_col, vec3 albedo){
    vec3 total = vec3(0.0);

    uint dir_light_count = floatBitsToUint(lights[0]);

    for (uint i = 0; i < dir_light_count; i++){
        Light l = {
            vec3(
                lights[i * 4 + 1],
                lights[i * 4 + 2],
                lights[i * 4 + 3]
            ),
            lights[i * 4 + 4]
        };
        vec3 light_dir = normalize(l.pos - point);
        vec3 ray_origin = point + normal * SHADOW_BIAS;
        ObjectHit hit = intersect(ray_origin, light_dir);
        float light_intensity_mul = 1.0;
        while (hit.object_type == OBJECT_TYPE_VOL_CUBE && hit.result.x > 0.0){
            switch (hit.object_type){
                case OBJECT_TYPE_VOL_CUBE:
                    // float dist = hit.result.y - max(hit.result.x, 0.0);

                    // float half_thickness = data[hit.object_index + 6];

                    // float alpha = 1.0 - pow(0.5, dist / half_thickness);

                    vec3 vol_cube_pos = vec3(
                        data[hit.object_index + 0],
                        data[hit.object_index + 1],
                        data[hit.object_index + 2]
                    );

                    vec3 vol_cube_dim = vec3(
                        data[hit.object_index + 3],
                        data[hit.object_index + 4],
                        data[hit.object_index + 5]
                    );

                    vec3 begin = ray_origin + max(hit.result.x, 0.0) * light_dir - vol_cube_pos;
                    vec3 end = ray_origin + hit.result.y * light_dir - vol_cube_pos;

                    begin /= vol_cube_dim;
                    end /= vol_cube_dim;

                    begin = begin / 2.0 + 0.5;
                    end = end / 2.0 + 0.5;

                    float alpha = 1.0 - transmittance_through_texture(begin, end, cube_density_texture);
                    alpha = clamp(alpha, 0.0, 1.0);

                    float next_depth = hit.result.y + SMALL_NUM;
                    ray_origin += light_dir * next_depth;
                    light_intensity_mul *= 1.0 - alpha;
                    break;

                default:
                    break;
            }
            hit = intersect(ray_origin, light_dir);
        }
        float r = length(l.pos - point);
        if ((hit.valid && hit.result.x > 0.0) && hit.result.x < r) continue;
        float power = max(dot(light_dir, normal), 0.0) * l.power * light_intensity_mul;
        vec3 reflected = albedo / PI;
        total += base_col * reflected * power;
    }

    uint light_begin = 1+4*dir_light_count;
    uint light_count = floatBitsToUint(lights[light_begin]);

    for (uint i = 0; i < light_count; i++){
        Light l = {
            vec3(
                lights[light_begin + i * 4 + 1],
                lights[light_begin + i * 4 + 2],
                lights[light_begin + i * 4 + 3]
            ),
            lights[light_begin + i * 4 + 4]
        };
        vec3 light_dir = normalize(l.pos - point);
        vec3 ray_origin = point + normal * SHADOW_BIAS;
        ObjectHit hit = intersect(ray_origin, light_dir);
        float light_intensity_mul = 1.0;
        while (hit.object_type == OBJECT_TYPE_VOL_CUBE && hit.result.x > 0.0){
            switch (hit.object_type){
                case OBJECT_TYPE_VOL_CUBE:
                    // float dist = hit.result.y - max(hit.result.x, 0.0);

                    // float half_thickness = data[hit.object_index + 6];

                    // float alpha = 1.0 - pow(0.5, dist / half_thickness);

                    vec3 vol_cube_pos = vec3(
                        data[hit.object_index + 0],
                        data[hit.object_index + 1],
                        data[hit.object_index + 2]
                    );

                    vec3 vol_cube_dim = vec3(
                        data[hit.object_index + 3],
                        data[hit.object_index + 4],
                        data[hit.object_index + 5]
                    );

                    vec3 begin = ray_origin + max(hit.result.x, 0.0) * light_dir - vol_cube_pos;
                    vec3 end = ray_origin + hit.result.y * light_dir - vol_cube_pos;

                    begin /= vol_cube_dim;
                    end /= vol_cube_dim;

                    begin = begin / 2.0 + 0.5;
                    end = end / 2.0 + 0.5;

                    float alpha = 1.0 - transmittance_through_texture(begin, end, cube_density_texture);
                    alpha = clamp(alpha, 0.0, 1.0);

                    float next_depth = hit.result.y + SMALL_NUM;
                    ray_origin += light_dir * next_depth;
                    light_intensity_mul *= 1.0 - alpha;
                    break;

                default:
                    break;
            }
            hit = intersect(ray_origin, light_dir);
        }
        float r = length(l.pos - point);
        if ((hit.valid && hit.result.x > 0.0) && hit.result.x < r) continue;

        // The only change between dir_lights and lights is that dir lights do not have an intensity falloff
        // and lights do
        float power = max(dot(light_dir, normal), 0.0) * l.power * light_intensity_mul / (4*PI*r*r);
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

                shading = shading_at_point(hit_pos, normal, vec3(1.0, 0.0, 0.0), vec3(0.18));
                color += clamp_color(shading) * influence;
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
                color += clamp_color(shading) * influence;
                influence = 0.0;
                break;

            case OBJECT_TYPE_VOL_CUBE:
                // vec3 cube_col;
                float alpha = 0.0;
                // cube_col = vec3(1.0);
                // float dist = hit.result.y - max(hit.result.x, 0.0); // Test value, we need to calculate this

                // float half_thickness = data[hit.object_index + 6];

                // alpha = 1.0 - pow(0.5, dist / half_thickness);

				vec4 cube_col = vec4(1.0);
                vec3 vol_cube_pos = vec3(
                    data[hit.object_index + 0],
                    data[hit.object_index + 1],
                    data[hit.object_index + 2]
                );

                vec3 vol_cube_dim = vec3(
                    data[hit.object_index + 3],
                    data[hit.object_index + 4],
                    data[hit.object_index + 5]
                );

                vec3 begin = ray_origin + max(hit.result.x, 0.0) * ray_dir;
                vec3 end = ray_origin + hit.result.y * ray_dir;
#if 0
                begin /= vol_cube_dim;
                end /= vol_cube_dim;

                begin = begin / 2.0 + 0.5;
                end = end / 2.0 + 0.5;

                alpha = 1.0 - transmittance_through_texture(begin, end, cube_density_texture);
#else
				cube_col = col_through_vol_cube(begin, end, vol_cube_pos, vol_cube_dim, cube_density_texture);

				alpha = cube_col.a;
#endif
                alpha = clamp(alpha, 0.0, 1.0);

                float next_depth = hit.result.y + SMALL_NUM;
                ray_origin = ray_origin + ray_dir * next_depth;

                color += cube_col.rgb * alpha * influence;        // This is the volumetric cube part
                influence *= 1.0 - alpha;
                break;

            case OBJECT_TYPE_NONE:
                vec3 skybox_col = texture(skybox_texture, ray_dir).rgb;
                // vec3 skybox_col = vec3(1.0);
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
