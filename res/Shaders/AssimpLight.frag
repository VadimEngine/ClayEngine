#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BloomColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

#define MAX_LIGHTS 16

// vec4 for padding
layout(std140) uniform LightBuffer {
    vec4 numLights;
    vec4 lightPositions[MAX_LIGHTS];
    vec4 lightColors[MAX_LIGHTS];
};

uniform sampler2D texture_diffuse1;
uniform bool uWireframeMode = false;
uniform vec4 uWireframeColor = vec4(0,0,0,1.0);
uniform vec4 uColor = vec4(1.0,1.0,1.0,1.0);
uniform vec3 viewPos; // Camera/view position

void main() {
    if (uWireframeMode) {
        FragColor = uWireframeColor;
    } else {
        // Initialize lighting accumulators
        vec3 ambient = vec3(0.0);
        vec3 diffuse = vec3(0.0);
        vec3 specular = vec3(0.0);

        for (int i = 0; i < numLights[0]; ++i) {
            vec3 lightPos = lightPositions[i].xyz; // Use .xyz to get the actual data
            vec3 lightColor = lightColors[i].xyz;  // Use .xyz to get the actual data

            ambient += 0.1 * lightColor;

            vec3 norm = normalize(Normal);
            vec3 lightDir = normalize(lightPos - FragPos);
            float diff = max(dot(norm, lightDir), 0.0);
            diffuse += diff * lightColor;

            vec3 viewDir = normalize(viewPos - FragPos);
            vec3 reflectDir = reflect(-lightDir, norm);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
            specular += spec * lightColor;
        }

        vec3 result = (ambient + diffuse + specular) * vec3(uColor);
        FragColor = vec4(result, 1.0);

        float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
        if (brightness > 1.0) {
            BloomColor = vec4(FragColor.rgb, 1.0);
        } else {
            BloomColor = vec4(0.0, 0.0, 0.0, 1.0);
        }

        // FragColor = vec4(numLights/16.0, lightColors[0].x, 1.0, 1.0);
        // FragColor = vec4(numLights/16.0, lightColors[0].x, numLights/16.0, 1.0);


        // Optionally use the texture
        // FragColor = texture(texture_diffuse1, TexCoords) * vec4(result, 1.0);
    }
}
