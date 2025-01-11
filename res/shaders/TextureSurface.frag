#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D uTexture;

uniform bool uwireframeMode = false;
uniform vec4 uwireframeColor = vec4(0,0,0,1.0);
uniform vec4 uColor = vec4(1.0,1.0,1.0,1.0);
uniform bool uVerticalFlip = true;

vec3 magenta1 = vec3(1.0, 0.0, 1.0);
vec3 magenta2 = vec3(0.60, 0.0, 0.60); //GL_SRGB of (204,0,204)
float epsilon = 0.01;

// Top left corner of sub image
uniform vec2 uSubImageTopLeft = vec2(0.0, 0.0);
// Size of subImage to draw
uniform vec2 uSubImageSize = vec2(1.0, 1.0);

void main() {
    if (uwireframeMode) {
        FragColor = uwireframeColor;
    } else {
        // Adjust texture coordinates for vertical flip
        vec2 flippedTexCoords = TexCoords;
        if (uVerticalFlip) {
            flippedTexCoords.y = 1.0 - TexCoords.y;
        }

        // Calculate the normalized texture coordinates for the sub-image
        vec2 subImageCoords = flippedTexCoords * uSubImageSize + uSubImageTopLeft;
        vec4 sampledColor = texture(uTexture, subImageCoords);

        if (length(sampledColor.rgb - magenta1) < epsilon || length(sampledColor.rgb - magenta2) < epsilon) {
            discard; // Discard fragment if spritesheet background color
        }

        FragColor = sampledColor * uColor;
    }
}