# Lecture 5 — Texturing & Applied Surface Effects

> **Repository checkpoint:** This exercise was written for the `lecture-05`
> baseline. Once published, that tag will identify the known-good Lecture 5
> starting state. If `main` has already moved ahead, create a catch-up branch:
>
> ```powershell
> git fetch --tags
> git switch -c catchup-lecture-05 lecture-05
> ```
>
> If your current code already matches the exercise and works, you do not need
> to switch.

## Purpose

The central question today is:

**How can a shader use coordinates to retrieve surface data and combine that
data with lighting and shader mathematics?**

Use the course workflow throughout:

**Predict -> change one thing -> run -> observe -> compare -> explain**

The main files remain `src/main.cpp`, `shaders/basic.vert`, and
`shaders/basic.frag`. Rebuild after shader edits so CMake copies them beside the
executable.

## 1. Inspect the known-good textured cube

Build and run. You should see the same static lit cube as Lecture 4, now with
one crisp red/cyan 4x4 checker across every face.

Trace these two paths:

```text
CPU vertex UV -> VBO/attribute 2 -> vertex shader -> interpolation -> fragment uv

CPU RGBA bytes -> texture object -> texture unit 0 -> sampler2D -> sampled texel
```

Find the eight-float stride, UV attribute offset, texture creation and upload,
texture-unit binding, sampler uniform, `texture(...)` call, and the point where
sampled RGB becomes the lit material colour.

## 2. Visualize UV directly

Temporarily replace the final output with:

```glsl
FragColor = vec4(uv, 0.0, 1.0);
```

Predict where red (`u`) and green (`v`) increase on one visible face. Why does
every face receive a complete gradient even though its world orientation is
different?

Restore the known-good output before continuing.

## 3. Predict and sample known texels

The generated texture is four texels wide and high. With nearest filtering,
each texel covers one quarter of each UV axis.

Temporarily sample a fixed coordinate:

```glsl
vec4 texel = texture(surfaceTexture, vec2(0.125, 0.125));
FragColor = texel;
```

Find that texel in the CPU byte array and predict whether the cube becomes red
or cyan before running. Repeat with one coordinate in a neighboring texel.
Restore sampling with `uv` afterward.

## 4. Compare nearest and linear filtering

In `main.cpp`, change only the two filtering parameters from `GL_NEAREST` to
`GL_LINEAR`. Predict what happens at checker boundaries.

Linear filtering can produce colours not explicitly stored in the texture by
combining neighboring texels. Restore nearest filtering after comparing.

## 5. Compare repeat and clamp-to-edge

Wrapping matters when coordinates leave the usual `[0,1]` range. First change
only the sampling coordinate:

```glsl
vec2 sampleUV = uv * 2.5;
vec4 texel = texture(surfaceTexture, sampleUV);
```

Predict how many repetitions should appear. Then change `GL_REPEAT` to
`GL_CLAMP_TO_EDGE` for both S and T. Which texels extend beyond the edge?

Restore `GL_REPEAT` and direct `uv` sampling.

## 6. Compare unlit texels with a textured material

First bypass lighting:

```glsl
FragColor = vec4(texel.rgb, 1.0);
```

Then restore the known-good ambient, diffuse, and specular result. Identify
which version shows only stored texture colour and which also communicates the
cube's orientation relative to the light and viewer.

## 7. Treat alpha as mask data

The checker stores alternating alpha bytes `255` and `64`, but the known-good
output deliberately uses alpha `1.0`.

Visualize the sampled channel as grayscale:

```glsl
FragColor = vec4(vec3(texel.a), 1.0);
```

Now use it as a mask between two colours:

```glsl
vec3 maskedColor = mix(
    vec3(0.08, 0.15, 0.65),
    vec3(1.0, 0.75, 0.10),
    texel.a);
FragColor = vec4(maskedColor, 1.0);
```

Explain why a texture channel is data and is not automatically transparency.

## 8. Combine sampled and procedural data

Create one simple UV-based scalar and combine it with the sampled mask:

```glsl
float wave = 0.5 + 0.5 * sin(uv.y * 20.0);
float combinedMask = texel.a * wave;
```

First output `wave`, then `combinedMask`, as grayscale. Only after inspecting
them should you use `combinedMask` with `mix`. Explain what spatial variation
comes from the texture and what comes from shader mathematics.

## 9. Temporary alpha and blending experiment

First output sampled alpha while blending remains disabled:

```glsl
FragColor = vec4(color, texel.a);
```

Alpha is currently just another fragment output value. It does not by itself
make conventional transparency happen.

Temporarily enable blending once after OpenGL initialization:

```cpp
glEnable(GL_BLEND);
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
```

Run again and compare. Blending is a later framebuffer/output operation that
uses source alpha to combine the fragment colour with the existing destination
colour.

This is a compact demonstration, not a complete solution for transparent 3D
objects. The cube still writes depth, and its faces are not sorted for
transparency. Do not add sorting or a transparency system here.

Finish by disabling/removing blending and restoring:

```glsl
FragColor = vec4(color, 1.0);
```

## Optional debugging: break the UV attribute layout

Save the known-good values first. Deliberately use either the old six-float
stride or an incorrect UV offset. Visualize `uv` directly rather than debugging
the complete lighting expression.

Explain why position and normal data can also become corrupted when the stride
is wrong, while a wrong UV offset may leave geometry intact but sample
unexpected coordinates. Restore the eight-float stride and six-float UV offset.

## Stretch and context

- **Mipmapping:** useful for minification and reducing distant aliasing, but the
  default baseline intentionally has no mipmap generation.
- **Image loading:** useful for projects, but adds decoding, paths, orientation,
  and asset-deployment concerns beyond today's texture-object lesson.
- **UV animation/distortion:** use the existing CPU time input with a shader
  offset or wave; visualize the modified coordinates before sampling.
- **Multiple textures:** a later extension of texture units and sampler
  uniforms, not a reason to build a material system now.
- **Normal mapping:** stores direction data in a texture but requires tangent
  space and a TBN basis. It is not a core Lecture 5 requirement.

## Project bridge

For one project idea, identify whether a texture would contain colour, a mask,
or another kind of data. State where its coordinates would come from, how you
would visualize a sample while debugging, and the smallest experiment that
would test the idea.

## Finish

Restore the static opaque checker cube with repeat wrapping, nearest filtering,
direct UV sampling, full lighting, blending disabled, and final alpha `1.0`.
Be ready to explain the distinction between a texture object, texture unit,
sampler uniform, UV coordinate, sampled texel, and framebuffer blending.
