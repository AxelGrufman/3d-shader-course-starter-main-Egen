#version 330 core

in vec3 worldPosition;
in vec3 worldNormal;
in vec2 uv;

uniform vec3 lightDirection;
uniform vec3 lightColor;
uniform vec3 viewPosition;
uniform vec3 baseColor;

uniform float ambientStrength;
uniform float specularStrength;
uniform float shininess;

out vec4 FragColor;

void main()
{
    vec3 N = normalize(worldNormal);
    vec3 L = normalize(lightDirection);
    vec3 V = normalize(viewPosition - worldPosition);

    float cosTheta = clamp(dot(N, V), 0.0, 1.0);
    const float F0 = 0.02;
    float fresnel = F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);

    float diffuseFactor = max(dot(N, L), 0.0);

    vec3 ambient = ambientStrength * baseColor;
    vec3 diffuse = diffuseFactor * baseColor * lightColor;
    vec3 waterColor = ambient + diffuse;

    const vec3 skyColor = vec3(0.30, 0.60, 0.90);
    vec3 reflectedWater = mix(waterColor, skyColor, fresnel);
    vec3 H = normalize(L + V);
    float specularFactor = 0.0;

    if (diffuseFactor > 0.0)
    {
        specularFactor = pow(max(dot(N, H), 0.0), shininess);
    }

    vec3 specular = specularStrength * specularFactor * fresnel * lightColor;

    vec3 finalColor = reflectedWater + specular;
    FragColor = vec4(finalColor, 1.0);
}

