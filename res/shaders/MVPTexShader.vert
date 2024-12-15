#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

layout(std140) uniform Camera {
    mat4 view;
    mat4 projection;
};

out vec2 TexCoord;

uniform mat4 uModel;

void main() {
    TexCoord = aTexCoords;
    gl_Position = projection * view * uModel * vec4(aPos, 1.0f);
}