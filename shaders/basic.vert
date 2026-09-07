#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

uniform float time;

out vec3 worldPosition;
out vec3 worldNormal;
out vec2 uv;

void addWave(
    vec2 position,
    vec2 direction,
    float amplitude,
    float frequency,
    float speed,
    float phaseOffset,
    inout float height,
    inout vec2 gradient)
{
    vec2 waveDirection = normalize(direction);

    float phase =
        dot(position, waveDirection) * frequency +
        time * speed +
        phaseOffset;

    height +=
        amplitude *
        sin(phase);

    float slope =
        amplitude *
        frequency *
        cos(phase);

    gradient +=
        slope *
        waveDirection;
}

void main()
{
    float height = 0.0;
    vec2 gradient = vec2(0.0);

    addWave(
        aPosition.xz,
        vec2(1.0, 0.25),
        0.18,
        1.10,
        0.80,
        0.0,
        height,
        gradient);

    addWave(
        aPosition.xz,
        vec2(-0.35, 1.0),
        0.08,
        2.30,
        1.35,
        1.7,
        height,
        gradient);

    addWave(
        aPosition.xz,
        vec2(0.75, -1.0),
        0.035,
        4.80,
        2.20,
        3.1,
        height,
        gradient);

    vec3 displacedPosition =
        aPosition;

    displacedPosition.y +=
        height;

    vec3 localNormal =
        normalize(
            vec3(
                -gradient.x,
                aNormal.y,
                -gradient.y));

    vec4 world =
        model *
        vec4(
            displacedPosition,
            1.0);

    worldPosition =
        world.xyz;

    worldNormal =
        normalize(
            normalMatrix *
            localNormal);

    uv = aUV;

    gl_Position =
        projection *
        view *
        world;
}
