#version 330 core

layout (location = 0) in vec3 aPos;

layout(std140) uniform Camera {
    mat4 view;
    mat4 projection;
};

uniform mat4 uModel;

void main() {
    gl_Position = projection * view * uModel * vec4(aPos, 1.0f);
}