# Lecture 2 — 3D Mathematics & Coordinate Spaces

> **Repository checkpoint:** This exercise was written for the `lecture-02`
> baseline. If `main` has already moved ahead, create a catch-up branch:
>
> ```powershell
> git fetch --tags
> git switch -c catchup-lecture-02 lecture-02
> ```
>
> If your current code already matches the exercise and works, you do not need
> to switch.

## Purpose and learning goals

The central question today is:

**How does this vertex get from the model into the image?**

You will work with vectors and matrices as practical graphics tools. The goal is not to derive matrix formulas. The goal is to build a useful mental model and connect the mathematics to code you can inspect, change and debug.

By the end, you should be able to:

- treat vectors as positions, directions and offsets;
- use translation, scale and rotation;
- explain why transformation order matters;
- trace a vertex through local/object, world, view and clip space;
- identify the later automatic steps from clip space to the screen;
- find where matrices are created on the CPU and uploaded to the GPU;
- explain what `w = 1.0` means at a practical level;
- predict basic effects of perspective, field of view and distance.

---

## Suggested reading / reference

These readings support the exercises, but you do not need to understand everything beforehand. Focus on concepts that map to this repository; LearnOpenGL uses different geometry and textures, so do not copy its examples mechanically. Matrix mathematics may become more intuitive after the practical experiments.

### Primary

- [LearnOpenGL — Transformations](https://learnopengl.com/Getting-started/Transformations) — useful for vectors and matrices, translation, scaling, rotation, homogeneous coordinates, GLM, transformation order, and matrix uniforms.
- [LearnOpenGL — Coordinate Systems](https://learnopengl.com/Getting-started/Coordinate-Systems) — useful for local/object, world, view, clip, NDC, and screen coordinates; model/view/projection; perspective; `gl_Position`; perspective divide; and viewport transformation.

### Refresher / reference

- [LearnOpenGL — Shaders / Uniforms](https://learnopengl.com/Getting-started/Shaders) — use this as a refresher when tracing values from the CPU application to shader uniforms.

The Camera chapter is not required for Lecture 2.

---

## How to work

For every experiment:

1. **Predict** what you expect to see.
2. **Change one thing.**
3. **Build and run.**
4. **Observe** the result carefully.
5. **Compare** it with your prediction.
6. **Explain** the result in your own words.

Keep a known-good version nearby. Restore it before moving to a new concept so that one experiment does not accidentally affect the next.

---

## Before you begin

The supervised studio/check-in from **09:00–10:00** is for setup, questions, completing Lecture 1 work and making sure your starter runs. Important new common material begins after 10:00.

From the repository root, build and run:

```powershell
cmake --build build --config Debug
.\build\bin\shader_course.exe
```

You should see one static triangle on a dark background, with smoothly interpolated red, green and blue vertex colours.

The main files today are:

```text
src/main.cpp
shaders/basic.vert
shaders/basic.frag
```

After changing a shader, build again so CMake copies it to `build/bin/shaders`.

Before continuing, save or commit the known-good state so you can return to it.

---

## Core exercises

### 1. Trace the current transform path

Do not change code yet. Find each item below.

In `src/main.cpp`, find:

- `vertices`, containing the original CPU-side positions and colours;
- `model`;
- `view`;
- `fieldOfView`, `aspectRatio`, `nearPlane` and `farPlane`;
- `projection` and the call to `glm::perspective`;
- `modelLocation`, `viewLocation` and `projectionLocation`;
- the three calls to `glGetUniformLocation`;
- the three calls to `glUniformMatrix4fv`;
- the draw call, `glDrawArrays`.

In `shaders/basic.vert`, find:

- the position attribute `aPosition`;
- the colour attribute `aColor`;
- the three `mat4` uniforms;
- the expression assigned to `gl_Position`.

In `shaders/basic.frag`, find where the interpolated `vertexColor` becomes `FragColor`.

Discuss or write short answers:

1. Which values begin on the CPU?
2. Which values are different for each vertex?
3. Which matrix values are shared by all vertices in this draw call?
4. Where do the matrices cross from CPU code to GPU shader code?
5. What coordinate space contains the original values in `aPosition`?
6. Which expression explicitly produces a clip-space position?
7. Does transforming position require changing the colour path?

Trace one vertex using this flow:

```text
vertices in C++
    -> VBO and aPosition
    -> model, view and projection uniforms
    -> gl_Position
    -> later graphics-pipeline operations
    -> image on screen
```

---

### 2. Directly manipulate a vertex position

Open `shaders/basic.vert`. Temporarily replace the current `gl_Position` expression with:

```glsl
vec3 offset = vec3(0.2, 0.0, 0.0);
gl_Position = vec4(aPosition + offset, 1.0);
```

Before running, predict:

- which direction the triangle will move;
- whether its size or shape will change;
- whether its colours will change.

Build, run and compare the result with your prediction.

Try these one at a time:

#### Scale directly

```glsl
gl_Position = vec4(aPosition * 0.5, 1.0);
```

Does the triangle become smaller around its centre, or move toward a corner?

#### Flip one axis

```glsl
gl_Position = vec4(aPosition.x, -aPosition.y, aPosition.z, 1.0);
```

Which axis changed? What happened to the top vertex?

#### Optional: move only one vertex

Use `aPosition.y > 0.0` to apply an offset only to the top vertex. Keep this experiment brief.

Why does changing one vertex alter the triangle's shape, while applying the same offset to every vertex moves it as one object?

The final `1.0` makes this a homogeneous **position**. At a practical level, this allows translation to affect it. No derivation is required here.

Restore the full expression before continuing:

```glsl
gl_Position = projection * view * model * vec4(aPosition, 1.0);
```

---

### 3. Transform the model with GLM

The model matrix transforms the complete object from local/object space into world space. The original values inside `vertices` do not need to change.

In `src/main.cpp`, replace the current identity model declaration with a mutable identity matrix:

```cpp
glm::mat4 model(1.0f);
```

`glm::mat4(1.0f)` creates an identity matrix. It is the matrix equivalent of “leave the value unchanged.”

Try each transformation separately. Predict first, then build and run.

#### Translation

```cpp
model = glm::translate(model, glm::vec3(0.25f, 0.0f, 0.0f));
```

Which coordinates change? Did the VBO data change?

#### Scale

```cpp
model = glm::scale(model, glm::vec3(0.6f, 0.6f, 1.0f));
```

Why does the whole triangle scale consistently, unlike moving only one vertex?

Try a non-uniform scale:

```cpp
model = glm::scale(model, glm::vec3(0.5f, 1.0f, 1.0f));
```

#### Rotation

```cpp
model = glm::rotate(
    model,
    glm::radians(30.0f),
    glm::vec3(0.0f, 0.0f, 1.0f));
```

`glm::rotate` expects an angle in radians. `glm::radians` makes the degree value convenient to read while supplying the expected unit.

Why is the Z axis a useful rotation axis for this triangle as currently viewed?

After each test, restore `model` to identity before starting the next one.

---

### 4. Compare transformation order

Matrices let us compose several transformations into one model matrix. Their order matters.

First try:

```cpp
glm::mat4 model(1.0f);
model = glm::translate(model, glm::vec3(0.3f, 0.0f, 0.0f));
model = glm::scale(model, glm::vec3(0.5f));
```

Predict the result, then build and run.

Now swap the two transformation calls:

```cpp
glm::mat4 model(1.0f);
model = glm::scale(model, glm::vec3(0.5f));
model = glm::translate(model, glm::vec3(0.3f, 0.0f, 0.0f));
```

Compare the triangle's size and position.

Explain:

- Why can the same translation and scale values produce different results?
- In the final shader expression, which operation is closest to the original vertex?
- What practical rule can you use when a composed transform behaves unexpectedly?

With the column-vector workflow used by GLM and OpenGL, transformations affect the vertex from right to left. You do not need to multiply the matrices by hand, but you do need to choose their order deliberately.

Restore the model matrix to identity after the comparison.

---

### 5. Explore the full model/view/projection chain

Confirm that `shaders/basic.vert` contains:

```glsl
gl_Position = projection * view * model * vec4(aPosition, 1.0);
```

Change only one parameter at a time. Restore it before testing the next parameter.

#### Model Z position

Start with an identity model matrix and add:

```cpp
model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f));
```

Predict whether the triangle will appear larger or smaller. Then try a modest positive Z value such as `0.5f` and compare.

Which matrix moved the object? Did the viewer or projection definition change?

#### View distance

The known-good view translation uses Z approximately equal to `-2.0f`. Compare it with:

```cpp
glm::vec3(0.0f, 0.0f, -4.0f)
```

Why does the apparent size change even though the local vertex positions and model matrix can remain unchanged?

#### Field of view

Compare the known-good `45.0f` with `25.0f` and `80.0f`, one at a time.

Before running, predict which field of view makes the triangle appear larger and which shows more of the scene.

Notice that `aspectRatio` is calculated from the current framebuffer dimensions. Resize the window. Does the triangle retain its proportions?

Restore the known-good values before continuing:

```cpp
const glm::mat4 model(1.0f);

const glm::mat4 view =
    glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0f));

const float fieldOfView = glm::radians(45.0f);
const float nearPlane = 0.1f;
const float farPlane = 100.0f;
```

---

## Debugging checkpoint — a successful draw with no visible geometry

Compilation success and a draw call do not guarantee that geometry is visible.

Temporarily change:

```cpp
const float nearPlane = 2.5f;
```

Predict what will happen. Build and run.

Do not make random edits. Gather evidence:

1. Did the C++ code compile?
2. Did the shaders compile and link?
3. Is `glDrawArrays` still present and reached?
4. Did the fragment shader or colour path change?
5. Approximately how far in front of the viewer is the triangle?
6. Is that geometry inside or in front of the new near plane?

The main lesson is simple: correctly submitted geometry can still lie outside the visible region.

Restore:

```cpp
const float nearPlane = 0.1f;
```

Build and verify that the triangle returns. Do not continue experimenting extensively with the clipping planes today.

---

## Coordinate-space checkpoint

Complete and explain this path in your own words:

```text
local/object space
    -- model matrix -->
world space
    -- view matrix -->
view space
    -- projection matrix -->
clip space
    -- automatic perspective divide -->
normalized device coordinates (NDC)
    -- automatic viewport transform -->
screen/window coordinates
```

Answer briefly:

1. Where are the original triangle positions stored?
2. Which matrix places the object in the world?
3. Which matrix describes the world relative to the viewer?
4. Which matrix introduces perspective and the clipping volume?
5. Which transformations are explicitly written in the vertex shader?
6. Which steps happen automatically after the vertex shader?
7. Why is the original vertex converted to `vec4(aPosition, 1.0)`?
8. At what point is `gl_Position` produced?

You do not need to derive the perspective divide or viewport transform. Be able to place them correctly in the path.

---

## Project connection

Choose one current project candidate. Write a few sentences answering:

- Which shader stage or stages are likely to matter?
- Which coordinate spaces may matter?
- Could translation, scale, rotation, view placement or perspective help?
- What is the smallest visual experiment you could build to test that idea?

This is a connection to your Week 1 proposal work, not a complete proposal assignment.

---

## Definition of done

You are done with the core work when you can:

- trace one vertex from `vertices` to `gl_Position`;
- distinguish vertex attributes from uniforms;
- identify where matrices are created and uploaded;
- directly translate, scale and flip vertex positions;
- apply model translation, scale and rotation with GLM;
- demonstrate that transformation order matters;
- restore and explain the complete model/view/projection expression;
- predict basic effects of model Z, view distance and field of view;
- diagnose and repair the near-plane visibility failure;
- explain local/object → world → view → clip → NDC → screen;
- restore the known-good final state.

---

## Optional stretch

Choose one if the core work is complete.

### Animated model rotation

Use `glfwGetTime()` when constructing the model matrix so its rotation angle changes over time. The model matrix is already uploaded each frame, so no new time uniform is required.

Can you explain why this is CPU-side animation even though the vertex shader applies the matrix?

### Rotate around another point

Compose translation, rotation and reverse translation so the triangle rotates around a point other than its own origin.

Predict the required order before testing it.

### Investigate transformation order

Combine non-uniform scale, rotation and translation. Change only the order and document which results surprised you.

### Perspective comparison

Choose two safe field-of-view or model-Z values. Record your prediction and result for each, then explain the difference without changing the geometry data.

---

## Restore the known-good baseline

Before finishing, confirm all of the following:

In `shaders/basic.vert`:

```glsl
gl_Position = projection * view * model * vec4(aPosition, 1.0);
```

In `src/main.cpp`:

```cpp
const glm::mat4 model(1.0f);

const glm::mat4 view =
    glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0f));

const float fieldOfView = glm::radians(45.0f);
const float nearPlane = 0.1f;
const float farPlane = 100.0f;
```

Build and run one final time. You should again see the static RGB triangle with the correct proportions.

Be ready to explain the complete answer to today's question:

**How does this vertex get from the model into the image?**
