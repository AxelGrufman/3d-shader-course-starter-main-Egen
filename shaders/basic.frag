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
    vec3 normal =
        normalize(worldNormal);

    vec3 light =
        normalize(lightDirection);

    float diffuseFactor =
        max(
            dot(normal, light),
            0.0);

    vec3 viewDirection =
        normalize(
            viewPosition -
            worldPosition);

    vec3 halfwayDirection =
        normalize(
            light +
            viewDirection);

    float specularFactor = 0.0;

    if (diffuseFactor > 0.0)
    {
        specularFactor =
            pow(
                max(
                    dot(
                        normal,
                        halfwayDirection),
                    0.0),
                shininess);
    }

    vec3 ambient =
        ambientStrength *
        baseColor;

    vec3 diffuse =
        diffuseFactor *
        baseColor *
        lightColor;

    vec3 specular =
        specularStrength *
        specularFactor *
        lightColor;

    vec3 finalColor =
        ambient +
        diffuse +
        specular;

    FragColor =
        vec4(
            finalColor,
            1.0);
}