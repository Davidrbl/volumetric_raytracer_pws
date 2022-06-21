#version 460 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

out FRAG_IN{
    vec2 uv;
} frag_in;

void create_vertex(float x, float y){
    frag_in.uv = vec2(x, y)/2.0 + 0.5;
    gl_Position = vec4(x, y, 0.0, 1.0);
    EmitVertex(); // Built-in GLSL function, pushes the currently constructed vertex to the primitive being constructed
}

void main(){
    create_vertex(-1.0,  1.0);
    create_vertex(-1.0, -1.0);
    create_vertex( 1.0,  1.0);
    create_vertex( 1.0, -1.0);
}
