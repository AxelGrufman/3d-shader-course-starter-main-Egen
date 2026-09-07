# Lecture 3 — Shader Mathematics & Experimentation

> **Repository checkpoint:** This exercise was written for the `lecture-03`
> baseline. If `main` has already moved ahead, create a catch-up branch:
>
> ```powershell
> git fetch --tags
> git switch -c catchup-lecture-03 lecture-03
> ```
>
> If your current code already matches the exercise and works, you do not need
> to switch.

Friday 28 August 2026, 09:00–12:00

## Purpose and learning goals

Today is a short shader workshop. The main progression is:

```text
input -> visualize it -> transform it -> make a pattern
      -> add time -> debug an intermediate value -> connect it to a project
```

The goal is not to memorize a catalogue of GLSL functions. The goal is to become comfortable asking what values a shader receives, changing those values with small mathematical operations, and using colour to inspect the result.

Keep using the course method:

**Predict → change one thing → run → observe → compare → explain**

Leave time during the session to discuss your individual project and proposal.

Friday is short: you are not expected to complete every example. Shader work may stop early to protect time for project and proposal feedback.

## Optional supporting reading

- [The Book of Shaders — Shaping Functions](https://thebookofshaders.com/05/) — especially useful for visualizing scalar functions and building patterns.
- [LearnOpenGL — Shaders](https://learnopengl.com/Getting-started/Shaders) — a refresher on GLSL inputs, outputs, and uniforms.

The examples use different coordinates and setup. Adapt the ideas to `localCoord` and `time`; do not copy them mechanically.

## Known-good starting point

Build and run from the repository root:

```powershell
cmake --build build --config Debug
.\build\bin\shader_course.exe
```

You should see the static RGB triangle on a dark background.

In `shaders/basic.frag`, confirm these inputs exist:

```glsl
in vec2 localCoord;
uniform float time;
```

The baseline still ends with:

```glsl
FragColor = vec4(vertexColor, 1.0);
```

`localCoord` is the interpolated local/object-space XY position from the triangle's existing vertices. It is **not** UV coordinates and not screen coordinates. Its values do not naturally cover exactly `[0, 1]` or `[-1, 1]`.

`time` is simply another input: one float that changes as the program runs.

After every shader edit, rebuild and restart the application.

## Part A — Inspect and visualize `localCoord`

First output the coordinate as colour:

```glsl
FragColor = vec4(localCoord, 0.0, 1.0);
```

Predict where red and green should increase. Run and observe.

Now isolate one component as grayscale:

```glsl
float value = localCoord.x;
FragColor = vec4(vec3(value), 1.0);
```

Repeat with `.y`.

- Where do values become dark or black?
- What happens to negative values when written directly as colour?
- Does the coordinate follow the object, the window, or the screen?

The displayed framebuffer has a limited colour range, so negative or out-of-range shader outputs may be clamped or converted on their way to the screen. This is output behavior, not a rule that GLSL values or functions are universally clamped. When useful, remap the coordinate for visualization:

```glsl
vec2 displayCoord = localCoord * 0.5 + 0.5;
FragColor = vec4(displayCoord, 0.0, 1.0);
```

This is a convenient shift and scale, not proof that `localCoord` is a UV or that it originally occupied `[-1, 1]`.

### What about UVs?

UVs are commonly used 2D surface coordinates, especially for texture mapping and procedural effects. `[0, 1]` is a common UV convention, not a requirement. Our triangle does not yet have a separate UV attribute. For Friday, `localCoord` gives us a simple 2D domain using the existing vertex positions. We will revisit UVs when textures are introduced.

## Part B — Shape one scalar

Start with one component:

```glsl
float value = localCoord.x;
```

Choose two or three small experiments below. Each listed line is an independent experiment starting from the original scalar unless you deliberately combine them. Before each run, predict the output range and where the image should change.

```glsl
value = abs(value);
value = fract(value * 5.0);
value = step(0.2, value);
value = smoothstep(0.0, 0.3, value);
value = 0.5 + 0.5 * sin(value * 15.0);
```

Visualize after every individual operation:

```glsl
FragColor = vec4(vec3(value), 1.0);
```

Do not stack everything immediately. Ask:

- What range enters the function?
- What range comes out?
- Where are transitions sharp, smooth, repeated, or mirrored?

Optional 2D check: visualize `length(localCoord)` as grayscale. Unlike using only `.x`, this measures distance from the local origin in both dimensions.

## Part C — Static procedural pattern sprint

Create one static pattern using `localCoord` and at least two operations or functions.

For example, start from a wave:

```glsl
float wave = sin(localCoord.x * 20.0);
float stripes = step(0.0, wave);
FragColor = vec4(vec3(stripes), 1.0);
```

Before showing the final pattern, debug one intermediate scalar as grayscale. For the example above, remap and display `wave`:

```glsl
float debugValue = 0.5 + 0.5 * wave;
FragColor = vec4(vec3(debugValue), 1.0);
```

Your pattern could use stripes, rings, a split, a soft edge, repeated cells, or another simple idea. Be ready to identify the input that creates spatial variation, the operations that shape it, and what the grayscale intermediate revealed.

## Part D — Add time

First inspect a value that changes globally:

```glsl
float pulse = 0.5 + 0.5 * sin(time);
FragColor = vec4(vec3(pulse), 1.0);
```

Every fragment receives the same `time`, so the whole triangle changes together.

Now compare it with a spatially varying expression:

```glsl
float wave = 0.5 + 0.5 * sin(localCoord.x * 15.0 + time);
FragColor = vec4(vec3(wave), 1.0);
```

Here, `localCoord.x` differs across the triangle while `time` changes each frame.

Animate the pattern you already made in Part C. Do not start over. Add `time` to one meaningful part such as an offset, phase, or threshold.

Explain the difference between:

```glsl
sin(time)
```

and:

```glsl
sin(localCoord.x * frequency + time)
```

## Part E — Visual shader debugging

When a shader becomes black, noisy, frozen, or surprising, use this sequence:

1. **Output a constant** to confirm the fragment shader draws.
2. **Output the input/coordinate** to confirm the data varies as expected.
3. **Output one scalar as grayscale** using `vec3(value)`.
4. **Isolate one expression** instead of debugging the complete effect.
5. **Freeze or remove time** so the image stops changing; temporarily replacing `time` with a constant is enough.
6. **Reduce complexity** until the result is predictable.
7. **Rebuild incrementally**, adding one operation at a time.

Apply this sequence to your Part D pattern. Deliberately make one value confusing—for example, remove a remap or use an extreme frequency—then diagnose and repair it using evidence rather than random edits.

## Part F — Project bridge

Use the remaining workshop time to connect shader experiments to one project candidate.

Write four short answers:

1. What is one visual or technical question you want to answer?
2. What is the smallest first experiment that could answer it?
3. What is your biggest unknown or risk?
4. What is your fallback if the first idea is too large or does not work?

Prefer an experiment you could recognize visually. It does not need to become the whole project.

## Optional toolbox

Use this as a reference, not a checklist.

- **Blend/control:** `mix`, `min`, `max`, `clamp`
- **Threshold:** `step`, `smoothstep`
- **Repeat/quantize:** `fract`, `floor`
- **Shape/distance:** `abs`, `length`, `distance`
- **Oscillation:** `sin`, `cos`
- **Vector utility:** `normalize`

When trying an unfamiliar function, first feed it a simple value and visualize its output range.

## Finish

Before leaving, aim to be able to show or describe one static pattern, how time changes it, and one intermediate grayscale debug view. Proposal work retains priority. Discuss your project-bridge answers and restore a working shader state that you understand.

Be ready to explain the complete process:

**What input did you use, how did you transform it, and how did you verify what the shader was doing?**
