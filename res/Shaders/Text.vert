#version 330 core
layout (location = 0) in vec4 vertex;

layout(std140) uniform Camera {
    mat4 view;
    mat4 projection;
};

out vec2 TexCoords;

uniform mat4 uModel;

void main() {
    gl_Position = projection * view * uModel * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
}