# Lecture 1 — First Shader Experiments

> **Repository checkpoint:** This exercise was written for the `lecture-01`
> baseline. If `main` has already moved ahead, create a catch-up branch:
>
> ```powershell
> git fetch --tags
> git switch -c catchup-lecture-01 lecture-01
> ```
>
> If your current code already matches the exercise and works, you do not need
> to switch.

As you work through this course, use learnopengl.com as a reference and structured tutorial.
The **Shaders** chapter is particularly relevant.
Read the theory there, and work through some of the examples to reinforce your understanding.
Then return to the course material/exercises.

## Goal

Build a first mental model of:

**CPU vertex data → vertex shader → primitive assembly → rasterization/interpolation → fragment shader → framebuffer**

The goal is not to memorize every OpenGL function yet.

Focus on understanding:

- where the data comes from;
- what the vertex shader does;
- what the fragment shader does;
- what the graphics pipeline interpolates for us;
- how changing shader code changes the final image.

---

## Before you start

Make sure the starter project builds and runs successfully.

From the repository root:

```powershell
cmake --build build --config Debug
.\build\bin\shader_course.exe
```

You should see the RGB triangle.

The main files for this exercise are:

```text
src/main.cpp
shaders/basic.vert
shaders/basic.frag
```

After changing a shader, rebuild and run the project again.

---

## Core exercise

### 1. Constant fragment colour

Open:

```text
shaders/basic.frag
```

Change the fragment shader so every fragment becomes one constant colour.

For example:

```glsl
FragColor = vec4(1.0, 0.2, 0.7, 1.0);
```

Before running, answer:

- Which shader stage are you changing?
- Does changing the fragment colour affect the triangle's geometry?
- What do you expect the result to look like?

Build and run.

---

### 2. Vertex colours and interpolation

Restore the original fragment shader.

Now change the RGB values stored for the three vertices in:

```text
src/main.cpp
```

Build and run again.

Observe the colours between the vertices.

Explain briefly:

- Where do the intermediate colours come from?
- Are those intermediate colours explicitly stored in `main.cpp`?
- Which part of the graphics pipeline produces them?

---

### 3. Modify interpolated data

In `basic.frag`, modify the interpolated colour mathematically.

Try:

```glsl
vec3 color = 1.0 - vertexColor;
FragColor = vec4(color, 1.0);
```

Then try:

```glsl
vec3 color = vertexColor * vertexColor;
FragColor = vec4(color, 1.0);
```

For each change:

1. predict the result;
2. build and run;
3. compare the result with your prediction.

Try one variation of your own.

---

### 4. Break something on purpose

Introduce a GLSL syntax error in one of the shader files.

Build/run the project and inspect the terminal output.

Find:

- which shader failed;
- the compiler error;
- the approximate location of the problem.

Fix the error and verify that the triangle renders again.

A black screen or shader compiler error is not unusual in graphics programming. Learning how to isolate and understand failures is part of the course.

---

## Stretch — first uniform

Add a time value to the fragment shader.

In `basic.frag`, add:

```glsl
uniform float time;
```

Then send a value from C++ using `glfwGetTime()`.

Use the value to animate the shader, for example:

```glsl
float pulse = 0.5 + 0.5 * sin(time);
```

Use `pulse` to affect one or more colour components.

The LearnOpenGL **Shaders** chapter contains a useful example of sending uniforms from C++ to GLSL.

Try to identify:

- where the uniform is declared;
- where its value comes from;
- whether the uniform changes per vertex, per fragment, or per draw/frame.

---

## Optional exploration

If you finish early, try one or more of these:

- make one colour channel pulse over time;
- make all colours fade toward black;
- swap colour channels;
- use `sin()` on the interpolated colour;
- create a grayscale result;
- make the effect depend on both colour and time.

Keep the changes small enough that you understand what each line is doing.

---

## Reflection

Be ready to explain:

1. What does the vertex shader execute once per?
2. What does the fragment shader execute once per?
3. What data did rasterization interpolate for us?
4. Which parts of this program happen on the CPU?
5. Which parts happen on the GPU?
6. What changed when you modified `main.cpp`?
7. What changed when you modified `basic.frag`?
8. What kind of visual effect would you like to explore further in this course?
