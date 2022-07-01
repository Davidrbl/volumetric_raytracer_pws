#version 460 core

#define PI 3.14159265

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

out FRAG_IN {
    vec2 uv;
    mat3 rot;
} frag_in;

uniform vec3 cam_for;

void create_vertex(float x, float y) {
    frag_in.uv = vec2(x, y)/2.0 + 0.5;

    gl_Position = vec4(x, y, 0.0, 1.0);
    EmitVertex(); // Built-in GLSL function, pushes the currently constructed vertex to the primitive being constructed
}

void main() {
    frag_in.rot;

    vec3 forward = cam_for;
    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = cross(forward, up);
    right = normalize(right);

    up = cross(right, forward);

    // First row
    frag_in.rot[0][0] = right.x;
    frag_in.rot[1][0] = right.y;
    frag_in.rot[2][0] = right.z;

    // Second row
    frag_in.rot[0][1] = up.x;
    frag_in.rot[1][1] = up.y;
    frag_in.rot[2][1] = up.z;

    // Third row
    frag_in.rot[0][2] = forward.x;
    frag_in.rot[1][2] = forward.y;
    frag_in.rot[2][2] = forward.z;

    // Vertex Creation
    create_vertex(-1.0,  1.0);
    create_vertex(-1.0, -1.0);
    create_vertex( 1.0,  1.0);
    create_vertex( 1.0, -1.0);
    EndPrimitive();
}
