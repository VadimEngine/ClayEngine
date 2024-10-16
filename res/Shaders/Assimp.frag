#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BloomColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform bool uWireframeMode = false;
uniform vec4 uWireframeColor = vec4(0,0,0,1.0);
uniform vec4 uColor = vec4(1.0,1.0,1.0,1.0);

void main() {
    if (uWireframeMode) {
        FragColor = uWireframeColor;
    } else {
        FragColor = uColor;
        //FragColor = texture(texture_diffuse1, TexCoords);
    }

    // check whether fragment output is higher than threshold, if so output as brightness color
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 1.0) {
        BloomColor = vec4(FragColor.rgb, 1.0);
    } else {
        BloomColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}