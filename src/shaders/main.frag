#version 460 core

layout (location = 0) out vec4 FragColor;

in FRAG_IN {
    vec2 uv;
} frag_in;

vec3 solve_quadratic_form(float a, float b, float c){
    float disc = b*b - 4*a*c;
    if (disc < 0) return vec3(1.0);
    float first  = (-b + sqrt(disc)) / (2 * a);
    float second = (-b - sqrt(disc)) / (2 * a);

    return vec3(first, second, 0.0); // The Z component will be 1.0 if it failed, 0.0 if it was succesfull
}

vec3 intersect(vec3 ray_origin, vec3 ray_dir){
    vec3 return_value;

    vec4 circle = vec4(0.0, 0.0, 2.0, 1.0); // x, y, z, position, w (4th) is radius
    // ^ just want to test if this works def will not hardcode it like this in the future, figure something out idk

    vec3 L = ray_origin - circle.xyz;
    float a = dot(ray_dir, ray_dir);
    float b = 2 * dot(ray_dir, L);
    float dist = length(ray_origin - circle.xyz); 
    float c = dist*dist - circle.w * circle.w;

    return_value = solve_quadratic_form(a, b, c);

    if (return_value.x > return_value.y) return_value.xy = return_value.yx;

    return vec3(return_value.xyz);

}

void main(){
    vec3 ray_dir = vec3(frag_in.uv*2.0-1.0, 1.0);
    ray_dir = normalize(ray_dir);
    // Take camera rotation into account, by rotation matrix or another way

    vec3 hit = intersect(vec3(0.0), ray_dir);

    vec3 col = vec3(0.0);

    if (hit.z == 0.0) col = vec3(1.0);

    // FragColor = vec4(frag_in.uv, 0.0, 1.0);
    // FragColor = vec4(ray_dir, 1.0);
    FragColor = vec4(col, 1.0);
}
