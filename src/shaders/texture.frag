#version 450 core

layout (location = 0) out vec4 FragColor;

in FRAG_IN {
    vec2 uv;
} frag_in;

uniform sampler2D texture_ID;

void main(){
    FragColor = vec4(texture(texture_ID, frag_in.uv).rgb, 1.0);
}
