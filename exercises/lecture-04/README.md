# Lecture 4 — Lighting, Normals & Materials

> **Repository checkpoint:** This exercise was written for the `lecture-04`
> baseline. Once published, that tag will identify the known-good Lecture 4
> starting state. If `main` has already moved ahead, create a catch-up branch:
>
> ```powershell
> git fetch --tags
> git switch -c catchup-lecture-04 lecture-04
> ```
>
> If your current code already matches the exercise and works, you do not need
> to switch.

## Purpose

The central question today is:

**How does a surface's direction relative to a light and viewer affect its colour?**

Use the course workflow throughout:

**Predict -> change one thing -> run -> observe -> compare -> explain**

The main files are `src/main.cpp`, `shaders/basic.vert`, and
`shaders/basic.frag`. Rebuild after shader edits so CMake copies them beside the
executable.

## 1. Inspect the known-good lit cube

Build and run the project. You should see a static blue cube with several
visible faces, a small ambient floor, diffuse lighting, and a restrained
specular highlight.

Trace these two paths before changing code:

```text
local position -> world position -> view -> clip
local normal   -> world normal
```

Find the position and normal attributes, the normal matrix, the world-space
shader outputs, the light/material uniforms, and the final draw call. Which
values vary per vertex, which are interpolated, and which stay constant for the
draw?

## 2. Visualize normals as colour

Temporarily replace the final fragment output with:

```glsl
FragColor = vec4(N * 0.5 + 0.5, 1.0);
```

Predict the colour of faces whose world normals point mostly along +X, +Y, and
+Z. Why is the remap needed to display negative components?

## 3. Visualize `N dot L`

Output the diffuse scalar as grayscale:

```glsl
FragColor = vec4(vec3(diffuse), 1.0);
```

Predict the brightest face first. Explain why `max(dot(N, L), 0.0)` prevents
negative lighting on surfaces facing away from the light.

## 4. Change the light direction

In `main.cpp`, change one component of `lightDirection`. Keep it normalized.
Before running, predict which faces brighten and darken. Remember that this
repository defines the value as pointing **from the surface toward the light**.

Restore the known-good direction before continuing.

## 5. Break coordinate-space consistency

In the vertex shader, deliberately replace the correct normal transformation:

```glsl
worldNormal = normalMatrix * aNormal;
```

with:

```glsl
worldNormal = aNormal;
```

The light remains in world space while the normal is now local-space data.
Predict the error, run, and use normal-colour or diffuse-grayscale output to
diagnose it. Then restore the normal-matrix expression and verify the repair.

## 6. Compare diffuse and ambient

First output only:

```glsl
vec3 color = diffuseColor;
```

Then restore:

```glsl
vec3 color = ambientColor + diffuseColor + specularColor;
```

Which surfaces become completely black without ambient light? Ambient here is
a small readability floor, not a simulation of all indirect light.

## 7. Inspect specular lighting

Output `vec3(specular)` as grayscale. Find `V`, `H`, and the gated specular
calculation. Explain why the view direction uses world-space fragment position
and why a face with `diffuse == 0.0` must not receive a highlight.

Change only `shininess`, then restore it and change only `specularStrength`:

- Which value changes highlight size/concentration?
- Which value changes highlight intensity?

## Optional stretch: non-uniform scale

The checked-in inverse-transpose normal matrix is the generally correct way to
transform normals. Naively using `glm::mat3(model)` can produce incorrect normal
directions under non-uniform scaling. With this flat-shaded cube and some
transform combinations, however, normalization may make the visual difference
subtle or absent.

Optionally apply a non-uniform model scale and compare the two transformations.
Visualize normals or diffuse first, and treat an unchanged image as a result to
explain rather than proof that the transformations are equivalent. Restore the
correct normal matrix and known-good model afterward. Understanding or deriving
the inverse-transpose mathematics is not a core Lecture 4 requirement.

## Project bridge

For one project idea, identify where surface normals, light direction, viewer
direction, or material parameters affect the result. Propose the smallest
visual test that would verify your reasoning. If the project does not use
lighting, state why these concepts do not apply rather than forcing them in.

## Finish

Restore the known-good static cube. Be ready to explain why every vector in a
dot product must use the same coordinate space and how visualizing `N`, `N dot
L`, and the specular term helps isolate lighting errors.
