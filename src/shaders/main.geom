#version 460 core

#define PI 3.141592

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

out FRAG_IN{
    vec2 uv;
    vec3 dir;
    vec2 pny;
} frag_in;

uniform vec2 rotation;

void create_vertex(float x, float y){
    frag_in.uv = vec2(x, y)/2.0 + 0.5;

    float pitch, yaw;

    pitch = rotation.y + y*0.25*PI;
    yaw = rotation.x + x*0.25*PI;

    float d = cos(pitch);

    frag_in.dir = vec3(
        sin(-yaw) * d,
        sin(pitch),
        cos(yaw) * d
    );

    frag_in.pny = vec2(pitch, yaw);

    gl_Position = vec4(x, y, 0.0, 1.0);
    EmitVertex(); // Built-in GLSL function, pushes the currently constructed vertex to the primitive being constructed
}

void main(){
    create_vertex(-1.0,  1.0);
    create_vertex(-1.0, -1.0);
    create_vertex( 1.0,  1.0);
    create_vertex( 1.0, -1.0);
    EndPrimitive();
}
