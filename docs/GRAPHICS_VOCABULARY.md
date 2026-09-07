# Course graphics vocabulary — Weeks 1–3

This document explains the graphics-programming language encountered in the course so far. It is a reference, not a list to memorize. Use it when a word in the code, exercises, compiler output, or classroom discussion has become unfamiliar.

The terms are organized by the journey from source code to image. That order matters: graphics concepts make more sense when we see what each one contributes to the complete process.

```text
C++ application
    -> vertex data and uniforms
    -> draw call
    -> vertex shader
    -> primitive assembly, clipping and rasterization
    -> fragment shader
    -> framebuffer
    -> displayed image
```

## How to use this reference

When you encounter an unfamiliar term, ask four questions:

1. **What is it?** A value, operation, resource, stage, or coordinate system?
2. **Where does it live?** CPU memory, GPU-accessible memory, shader code, or the graphics pipeline?
3. **What purpose does it serve?** What problem would we have without it?
4. **What visible consequence can it have?** Can it change position, shape, colour, timing, or visibility?

## The two sides of the program

### CPU

The **central processing unit** runs the C++ application. It creates the window, reads files, prepares vertex data and transformation matrices, uploads values, and submits draw commands.

The CPU does not normally calculate the final colour of every fragment in this program. Its role is to prepare resources and describe work for the GPU.

### GPU

The **graphics processing unit** executes the graphics pipeline. It is designed to perform similar calculations for many vertices and fragments in parallel.

Our programmable GPU work lives mainly in the vertex and fragment shaders. Other stages, including primitive assembly and rasterization, are configured through OpenGL but operate automatically.

### CPU → GPU data flow

This phrase describes values crossing from the C++ application into GPU work.

In the current repository there are two especially visible paths:

- vertex positions and colours are copied into a vertex buffer;
- matrices and time are uploaded as uniforms.

Understanding which path a value uses helps answer whether it changes per vertex, per draw, or per frame.

## Application and API vocabulary

### API

An **application programming interface** is a set of functions and rules used to communicate with another system. OpenGL is a graphics API. Calls such as `glBufferData` and `glDrawArrays` ask an OpenGL implementation to manage data or rendering work.

An API call describes an operation; it does not expose every driver or GPU step used to perform it.

### OpenGL

**OpenGL** is the graphics API used by the course starter. It provides the vocabulary for buffers, shaders, vertex attributes, drawing, viewports, and other graphics operations.

The project requests OpenGL 3.3 Core. This keeps the course focused on the programmable pipeline instead of older immediate-mode drawing.

### OpenGL state

OpenGL is a **state machine**. Many calls change the currently selected program, buffer, vertex array, clear colour, or other state. Later calls use that current state.

For example, `glDrawArrays` does not name every required resource in its arguments. It relies on the program and vertex array that were bound earlier. This is why correct calls in the wrong order can still produce a blank or incorrect image.

### Core profile

The **core profile** exposes the modern OpenGL workflow and removes deprecated features from older versions. Students must provide shaders, buffers, and vertex descriptions explicitly.

That explicitness is useful here because it makes the path from data to image visible.

### Context

An OpenGL **context** is the environment that owns OpenGL state and resources and connects API calls to a graphics implementation.

A context must exist and be current before GLAD loads OpenGL functions or the application creates OpenGL resources. In this repository, GLFW creates the window and its context together.

### GLFW

**GLFW** is a small library used for window creation, OpenGL context creation, input, events, framebuffer-size queries, timing, and buffer swapping.

GLFW is not the renderer. It creates the environment in which OpenGL rendering can happen.

### GLAD

**GLAD** is the OpenGL function loader. Many OpenGL functions must be obtained from the active graphics driver at runtime.

The context is created first; then `gladLoadGL` obtains the function addresses. Without this step, calls such as `glCreateShader` would not be safely available.

### GLM

**OpenGL Mathematics**, or **GLM**, is a C++ mathematics library whose vector and matrix types follow common GLSL conventions.

The current application uses GLM to construct model, view, and perspective projection matrices. GLM calculates values on the CPU; it does not draw anything by itself.

### Driver

The graphics **driver** is system software that implements OpenGL for the available GPU. It compiles shaders, manages resources, and translates API requests into hardware work.

Shader behavior and error messages are governed by the OpenGL and GLSL specifications, but exact diagnostics and performance can differ between drivers.

### Handle / object name

An OpenGL **handle** is a small integer that identifies a resource managed by OpenGL. Variables such as `vao`, `vbo`, `shaderProgram`, and individual shader IDs are handles.

The integer is not the vertex data or shader machine code itself. It is how later API calls refer to the managed object.

### Bind

To **bind** an OpenGL object is to make it current for a particular purpose. For example, binding a VBO to `GL_ARRAY_BUFFER` makes later buffer operations refer to that VBO.

Binding explains many OpenGL sequences:

```text
create an object -> bind/select it -> configure or use it
```

### Resource lifetime

A resource's **lifetime** is the period between its creation and destruction. The application creates buffers, a vertex array, and a shader program, uses them during rendering, then deletes them before the context is destroyed.

Explicit cleanup makes ownership visible and avoids leaking driver-managed resources.

## Build and language vocabulary

### CMake

**CMake** reads `CMakeLists.txt` and generates a build system for the current platform. It describes targets, source files, dependencies, compiler requirements, and shader-copying steps.

CMake does not render the triangle and is not part of each frame. It prepares the program before runtime.

### Configure, build, and run

These are different phases:

- **configure:** CMake examines the project and dependencies and generates build files;
- **build:** the compiler and linker produce the executable, and CMake copies the shaders;
- **run:** the operating system starts the finished application.

A shader edit currently requires another build so the edited shader is copied beside the executable.

### Compiler

A **compiler** translates source code into another form the machine can execute. The C++ compiler builds the CPU application. The OpenGL driver compiles GLSL shaders at runtime.

A successful C++ build does not guarantee successful GLSL compilation because they are separate languages compiled at different times.

### Linker and shader linking

The C++ **linker** combines compiled native code and libraries into an executable.

OpenGL also **links** compiled shader stages into a shader program. Shader linking checks whether stages form a compatible pipeline—for example, whether a fragment-shader input matches a vertex-shader output.

### C++

**C++** is the CPU-side programming language used by the application. It handles setup, files, data, matrices, uniforms, drawing commands, the application loop, and cleanup.

### GLSL

The **OpenGL Shading Language** is the language used for GPU shader stages. Files ending in `.vert` and `.frag` contain GLSL.

GLSL includes graphics-oriented types such as `vec2`, `vec3`, `vec4`, and `mat4`, plus functions such as `mix`, `smoothstep`, `length`, and `sin`.

### `#version 330 core`

This shader directive selects GLSL version 3.30 and the core profile rules. It must appear before normal shader declarations.

The GLSL version is related to, but distinct from, the C++ language version and CMake version.

## Geometry and vertex data

### Geometry

**Geometry** is the shape information submitted for drawing. In the current starter it is only three vertices forming one triangle.

More complex geometry still reduces to collections of primitives such as triangles, but may use many vertices, indices, meshes, and models.

### Vertex

A **vertex** is one submitted set of attribute values. It often represents a corner of a primitive, but it is better understood as a packet of input data.

Each current vertex contains:

- a 3D local position;
- an RGB colour.

A vertex is not a pixel. The vertex shader runs for vertices before rasterization generates fragments.

### Vertex attribute

A **vertex attribute** is one per-vertex input, such as position, colour, normal, or UV coordinates.

`aPosition` and `aColor` are the current shader attributes. Their values can differ for every vertex.

### Interleaved data

**Interleaved vertex data** stores several attributes next to each other for each vertex:

```text
position, colour, position, colour, position, colour
```

The alternative would be separate arrays for positions and colours. Interleaving keeps each current vertex's data together.

### Float

A **floating-point number** represents a number with a fractional part, such as `0.65` or `-2.0`. GPU graphics calculations commonly use 32-bit floats.

The `f` suffix in C++, as in `1.0f`, requests a `float` rather than the language's default `double` literal.

### Component

A vector **component** is one named number within it. Components are commonly accessed as `.x`, `.y`, `.z`, `.w` for geometric meaning or `.r`, `.g`, `.b`, `.a` for colour meaning.

These are alternate names for positions in the same vector; `.x` and `.r` both refer to its first component.

### VBO

A **vertex buffer object** is an OpenGL buffer used here to store the triangle's vertex bytes in GPU-accessible memory.

The VBO owns the data. It does not, by itself, explain how those bytes should be divided into positions and colours.

### VAO

A **vertex array object** stores the configuration for supplying vertex attributes to the vertex shader. It remembers which enabled attribute locations read from which buffer layout.

A useful distinction is:

- VBO: the bytes;
- VAO: how to interpret the bytes for drawing.

### Stride

The **stride** is the byte distance from one occurrence of an attribute to the next occurrence for the following vertex.

Each current vertex contains six floats, so the stride is `6 * sizeof(float)`. A wrong stride makes OpenGL step through the buffer incorrectly.

### Offset

An attribute **offset** is the byte position where that attribute begins within one vertex record.

Position begins at byte offset zero. Colour begins after the first three floats. This use of “offset” describes memory layout; a geometric offset instead means a displacement added to a position.

### Attribute location

An **attribute location** is the numbered connection between a VAO description and a vertex-shader input.

The current agreement is:

- location `0` → `aPosition`;
- location `1` → `aColor`.

The numbers must agree on the C++ and GLSL sides.

### Primitive

A **primitive** is a basic shape assembled from vertices. The draw mode `GL_TRIANGLES` groups every three submitted vertices into an independent triangle.

Lines and points are other possible primitive types, but the triangle is the foundation of most real-time surface rendering.

### Draw call

A **draw call** tells OpenGL to render geometry using the currently selected state and resources.

```cpp
glDrawArrays(GL_TRIANGLES, 0, 3);
```

means: interpret three sequential vertices, starting at vertex zero, as triangles. The call initiates pipeline work; it does not directly set each screen pixel.

## Shader program and data interfaces

### Shader

A **shader** is a small GPU program for one programmable stage of the graphics pipeline. The course currently uses a vertex shader and a fragment shader.

The word historically refers to calculating surface appearance, but modern shader stages also transform geometry and perform many general parallel calculations.

### Shader stage

A **shader stage** is one programmable part of the pipeline with a specific responsibility and input/output contract.

The current stages are:

- vertex shader: runs for submitted vertices;
- fragment shader: runs for rasterized fragment candidates.

### Shader object

A **shader object** contains one compiled GLSL stage. The C++ code compiles the vertex and fragment shader objects separately.

### Shader program

A **shader program** is the linked collection of compatible shader stages used by a draw call. `glUseProgram` selects it as current OpenGL state.

### `in` and `out`

In GLSL, `in` declares data entering a shader stage and `out` declares data leaving it.

The meaning depends on the stage:

- vertex-shader `in`: per-vertex attributes from the vertex setup;
- vertex-shader `out`: values passed toward rasterization and later stages;
- fragment-shader `in`: interpolated values arriving from earlier stages;
- fragment-shader `out`: colour results sent onward to framebuffer operations.

### Uniform

A **uniform** is a shader input whose value is constant for all invocations in one draw call. “Uniform” describes uniformity across that draw, not necessarily permanence over time.

The matrices and `time` can be uploaded again every frame. Within one draw, however, every processed vertex or fragment sees the same corresponding uniform value.

### Uniform location

A **uniform location** is the program-specific integer used by C++ to upload a uniform value. `glGetUniformLocation` asks the linked shader program for this connection.

A location of `-1` commonly means the named uniform does not exist or was optimized away because the active shader does not use it. This is expected during some course experiments.

### Upload

To **upload** means to transfer or provide data from CPU-side code for GPU use. `glBufferData`, `glUniformMatrix4fv`, and `glUniform1f` upload different kinds of values.

The term describes direction and responsibility: the CPU supplies data that shader work will consume.

### Varying / interpolated value

“**Varying**” is a common informal name for a value written by the vertex shader and interpolated across a primitive for the fragment shader.

In modern GLSL the code uses matching `out` and `in` declarations. `vertexColor` and `localCoord` are current examples.

### Active and inactive shader input

An **active** shader input contributes to the linked program's observable result and receives a usable location where appropriate.

An unused uniform can be **inactive** because the GLSL compiler optimizes it away. Declaring a uniform is not enough to guarantee that it remains active; the shader must use it in relevant output calculations.

### Optimization

**Optimization** means transforming code so it produces the same observable result with less work or fewer resources. Shader compilers may remove unused calculations and inputs.

This is why the course application treats inactive experimental uniforms as informational rather than fatal.

## Vertex processing and transformations

### Vertex shader

The **vertex shader** runs once for each submitted vertex. It receives attributes, performs calculations, writes `gl_Position`, and may output values to interpolate.

Its required positional responsibility is to produce a clip-space position. It does not fill the triangle with pixels.

### `gl_Position`

`gl_Position` is a built-in vertex-shader output containing the vertex's homogeneous clip-space position.

Later pipeline stages use it for clipping, perspective division, primitive placement, and rasterization. If it is incorrect, geometry may be distorted, misplaced, or invisible even when fragment colour code is correct.

### Vector

A **vector** is an ordered group of numbers. Its interpretation comes from context.

The same `vec3` type might represent:

- a position: where something is;
- a direction: which way something points;
- an offset: how far and in which direction to move;
- a colour: red, green, and blue amounts.

The type alone does not supply the meaning; the variable's role does.

### Scalar

A **scalar** is one number rather than a vector. Field of view, time, aspect ratio, and the grayscale intermediate values in Lecture 3 are scalars.

Many procedural effects begin by creating one spatially varying scalar and interpreting it as brightness, a threshold, a blend amount, or a mask.

### Matrix

A **matrix** is a rectangular arrangement of numbers used here to represent and compose transformations.

A `mat4` can encode translation, rotation, scale, projection, or a combination. Its purpose is not merely to store numbers: it provides one consistent operation that transforms many vertices.

### Identity matrix

The **identity matrix** leaves a value unchanged when multiplied by it. `glm::mat4(1.0f)` creates a 4×4 identity matrix.

It is a useful known-good starting transform: the model matrix exists in the pipeline, but initially does not alter the triangle.

### Translation

A **translation** moves a position by an offset. It changes location without inherently changing size or orientation.

Translation is represented in a 4×4 matrix so it can be composed with rotations, scales, and projection.

### Scale

A **scale** changes size along one or more axes. Equal scale factors produce uniform scaling; different axis values produce non-uniform scaling.

Scaling around an origin also changes every point's distance from that origin, which is why the chosen pivot matters.

### Rotation

A **rotation** changes orientation around an axis and pivot. In the current triangle example, rotating around the Z axis creates an immediately visible rotation in the image plane.

Angles may be expressed in degrees or radians. GLM rotation functions expect radians, so `glm::radians` converts readable degree values.

### Transform composition and order

**Composition** combines transformations. Matrix multiplication allows several transformations to become one matrix operation.

Order matters because matrix multiplication is generally not commutative: translate-then-scale need not equal scale-then-translate. With the current column-vector convention, the transformation closest to the vertex in the written expression acts first.

### Homogeneous coordinates

**Homogeneous coordinates** represent a 3D position using four components. The course writes `vec4(aPosition, 1.0)`.

At a practical level:

- `w = 1.0` represents a position, so translation affects it;
- `w = 0.0` can represent a direction, so translation does not affect it.

The fourth component also makes perspective projection and the later perspective divide possible.

### Column-vector convention

The repository follows the conventional GLM/OpenGL **column-vector workflow**:

```glsl
projection * view * model * position
```

Read the effects on the position from right to left: model first, then view, then projection. No manual transpose is introduced during matrix upload.

## Coordinate spaces

A **coordinate space** is a frame of reference in which numbers have meaning. A position is never merely “at `(x, y, z)`”; it is at those coordinates relative to some origin and axes.

### Local space / object space

**Local** or **object space** is the model's own coordinate system. The triangle positions in the current VBO begin here.

Local coordinates are convenient because the object can be authored around its own origin, then placed elsewhere with a model transform.

### World space

**World space** is the shared coordinate system in which objects are placed relative to one another.

The model matrix converts local positions to world positions. The current identity model matrix means local and world coordinates happen to have equal numbers, but they remain conceptually different spaces.

### View space / eye space / camera space

**View space** describes the world relative to the viewer. The view matrix converts world-space positions into this coordinate system.

The starter uses a fixed view translation rather than a camera class. This keeps the responsibility visible without adding navigation infrastructure.

### Clip space

**Clip space** is the homogeneous coordinate space produced by the projection transform and written to `gl_Position`.

Clipping decisions and perspective division use these four-component coordinates. Clip space is not yet screen space, and its visible range should not be confused directly with pixels.

### Clipping

**Clipping** removes or trims primitive portions outside the visible clip volume. Geometry can compile and draw successfully yet remain invisible because it is outside that volume.

The near and far planes are part of the perspective viewing volume.

### Perspective divide

The **perspective divide** is the automatic division of clip-space `x`, `y`, and `z` by `w`. It produces normalized device coordinates.

This step is a central reason distant geometry appears smaller under perspective projection. Students do not need to perform it manually in the current shader.

### NDC

**Normalized device coordinates** are the coordinates after perspective division. Visible OpenGL NDC generally spans `-1` to `1` across each axis.

Do not confuse NDC with `localCoord`: `localCoord` comes from the triangle's original local XY values and does not automatically span the NDC range.

### Viewport

The **viewport** maps normalized device coordinates to a rectangular region of the framebuffer. `glViewport` defines that region.

Keeping it synchronized with framebuffer size prevents rendering from using stale drawable dimensions after a resize.

### Screen/window coordinates

**Screen** or **window coordinates** identify positions in the output window, commonly in pixel-related units after viewport mapping.

They occur late in the pipeline. Local positions, world positions, and clip positions are not screen coordinates even if some numbers coincidentally look similar.

### Aspect ratio

The **aspect ratio** is width divided by height. It describes the shape of the framebuffer.

The perspective projection uses the current framebuffer aspect ratio so resizing the window does not stretch the triangle.

### Field of view

The perspective **field of view** describes the angular extent visible through the projection. A narrower field of view tends to make objects appear larger; a wider field shows more of the scene and tends to make them appear smaller.

### Near and far planes

The **near** and **far clipping planes** bound visible view-space depth for the perspective projection.

They are not merely arbitrary visibility distances: their relationship also affects depth precision once depth testing is introduced. For now, their immediate purpose is defining the visible projection volume.

### Perspective projection

**Perspective projection** creates the familiar effect that farther objects appear smaller. It transforms view-space positions into clip space and prepares the `w` value used by the perspective divide.

The current projection is built with field of view, aspect ratio, near plane, and far plane.

### Orthographic projection

An **orthographic projection** is a projection in which apparent size does not decrease with distance. It has not been implemented in the current baseline, but it is a useful contrast to perspective and is common in diagrams, UI, technical views, and some stylized games.

## From triangle to fragments

### Primitive assembly

**Primitive assembly** groups processed vertices according to the draw mode. With `GL_TRIANGLES`, every group of three vertices becomes a triangle primitive.

### Rasterization

**Rasterization** determines which discrete sample locations a projected primitive covers and generates fragment candidates for them.

It bridges continuous geometric shapes and the discrete framebuffer grid.

### Interpolation

**Interpolation** estimates values between vertex outputs across a primitive. The triangle stores only three vertex colours, yet fragments receive smoothly changing `vertexColor` values.

The same process supplies changing `localCoord` values between the vertices. Those intermediate values are generated by the pipeline; they are not individually stored in the VBO.

### Fragment

A **fragment** is a candidate contribution to a framebuffer sample produced by rasterization. It carries interpolated inputs and other data into the fragment shader.

“Fragment” and “pixel” are often used informally as if identical, but they are not strictly the same. Multiple fragments may compete for or contribute to a pixel, and later tests or blending can affect whether a fragment updates the framebuffer.

### Fragment shader

The **fragment shader** runs for generated fragments and calculates output values such as colour.

In the baseline it writes interpolated RGB vertex colour. Lecture 3 uses it as a visual mathematics laboratory by turning coordinates and intermediate scalars into colour.

### `FragColor`

`FragColor` is the user-named fragment-shader output in this repository. It is a four-component RGBA colour sent to later framebuffer operations.

Unlike `gl_Position`, its name is not built into GLSL; the shader declares it explicitly with `out vec4`.

### RGB and RGBA

**RGB** stores red, green, and blue colour components. **RGBA** adds alpha.

Alpha often represents opacity or coverage for later blending, though the current starter does not enable blending and simply writes `1.0` for fully opaque output.

### Framebuffer

A **framebuffer** is the collection of image attachments receiving rendered results. The default framebuffer is ultimately presented in the GLFW window.

Its size can differ from the logical window size on high-DPI displays, which is why the code queries framebuffer dimensions.

### Clear

To **clear** a framebuffer attachment is to give it a known starting value before drawing a new frame. The application clears the colour buffer to a dark background.

Without clearing, old image contents could remain where the new frame draws nothing.

### Double buffering

**Double buffering** uses a front buffer for the displayed image and a back buffer for the next image being rendered.

The program draws into the back buffer and then swaps buffers. This avoids presenting a partially rendered frame.

### Buffer swap

`glfwSwapBuffers` presents the newly rendered back buffer and makes another buffer available for rendering. It marks the practical transition from “frame being constructed” to “frame visible to the user.”

### VSync

**Vertical synchronization** coordinates buffer presentation with the display's refresh cycle. `glfwSwapInterval(1)` requests one refresh interval between swaps.

Its practical purpose is to reduce visible tearing and keep the small example from presenting frames without limit.

## Shader mathematics and procedural vocabulary

### Procedural effect

A **procedural effect** is generated by rules and mathematics rather than only by stored image data. Stripes from `sin` and `step`, for example, are procedural.

Procedural does not mean random. The same inputs normally produce the same outputs.

### Domain

A mathematical **domain** is the set of input values over which a function or pattern is evaluated.

`localCoord` supplies a simple 2D domain over the triangle. Choosing local, world, view, UV, or screen coordinates as the domain changes what the pattern follows.

### `localCoord`

`localCoord` is the interpolated XY portion of each original local/object-space vertex position. It lets the fragment shader create spatial variation without adding another VBO attribute.

It is not a UV coordinate and not a screen coordinate. Its range comes from the triangle's authored positions.

### UV coordinates

**UVs** are 2D surface coordinates commonly used for texture mapping and procedural effects. A `[0, 1]` range is conventional but not mandatory.

The triangle does not currently have a separate UV attribute. `localCoord` provides a temporary 2D domain using existing local positions; that does not make it a UV.

### Time uniform

The `time` uniform contains seconds from `glfwGetTime`, converted to a float and uploaded each frame.

Time is simply a changing input. `sin(time)` produces one global changing value, while `sin(localCoord.x * frequency + time)` combines spatial variation with temporal change.

### Frequency

**Frequency** describes how often a pattern repeats over a distance or time interval. Multiplying a coordinate before `sin` or `fract` increases the number of repetitions over the same geometry.

Very high spatial frequencies can become unstable or aliased because the framebuffer cannot represent unlimited detail.

### Phase

**Phase** shifts the position within a repeating cycle. Adding `time` inside a sine expression changes its phase, which appears as motion through the spatial pattern.

### Range

A value's **range** is the set of values it can produce. Knowing or visualizing a range makes shader debugging much easier.

For example, `sin` produces values from `-1` to `1`, while display colour is most conveniently inspected in a `0` to `1` range.

### Remap

To **remap** means to shift and scale values from one range into another. This common expression maps `[-1, 1]` to `[0, 1]`:

```glsl
float displayed = value * 0.5 + 0.5;
```

Remapping changes how a value is used or visualized; it does not change what coordinate space originally produced it.

### Grayscale visualization

Turning a scalar into `vec3(value)` displays the same amount in red, green, and blue, producing grayscale.

This is one of the most useful shader-debugging techniques because it converts an invisible intermediate number into a spatial image.

### Mask

A **mask** is a scalar field used to select or blend regions. Values near zero commonly mean “exclude” and values near one mean “include,” with intermediate values allowing soft transitions.

Functions such as `step`, `smoothstep`, `length`, and comparisons help construct masks.

### Threshold

A **threshold** divides values into regions based on a boundary. `step(edge, value)` produces a hard zero-or-one transition.

Thresholds are useful for stripes, cutouts, silhouettes, and turning a continuous field into distinct areas.

### Smooth transition

`smoothstep` produces a gradual transition between two edges rather than a hard jump. It is useful for antialiased-looking boundaries, soft masks, and controllable fades.

### Distance field

A **distance field** stores or computes distance to a point, line, boundary, or shape. For example, `length(localCoord)` is distance from the local origin.

Distance fields turn geometry questions into scalar values that can be visualized, thresholded, repeated, or animated.

### Visual shader debugging

**Visual debugging** uses shader output to inspect values directly. A practical sequence is:

1. output a constant;
2. output the input coordinate;
3. output one scalar as grayscale;
4. isolate one expression;
5. freeze time;
6. reduce complexity;
7. rebuild incrementally.

This is often more effective than guessing because GPU shader values cannot be inspected like ordinary C++ variables in the current setup.

## Frequently used GLSL operations

These descriptions focus on purpose, not memorization.

### `abs(value)`

Returns magnitude without sign. It can mirror a pattern around zero or turn signed distance into unsigned distance.

### `sin(value)` and `cos(value)`

Produce smooth repeating oscillations. They are useful for waves, pulses, motion, repetition, and circular relationships.

### `fract(value)`

Returns the fractional portion, creating a repeating `0`-to-just-below-`1` ramp as the input increases. It is useful for repeated cells and stripes.

### `floor(value)`

Rounds downward to an integer-valued result. It can label repeated cells, quantize values, or create steps.

### `step(edge, value)`

Returns zero below the edge and one at or above it. Use it to create a hard binary mask.

### `smoothstep(edge0, edge1, value)`

Returns a smooth transition from zero to one across the two edges. Use it when a hard threshold looks too abrupt or unstable.

### `mix(a, b, amount)`

Interpolates between `a` and `b`. At amount zero the result is `a`; at one it is `b`.

It is useful for combining colours or effects through a mask.

### `min`, `max`, and `clamp`

These constrain or combine ranges:

- `min` chooses the smaller value;
- `max` chooses the larger value;
- `clamp` limits a value between lower and upper bounds.

They are useful for controlling output, combining distance fields, and preventing parameters from leaving useful ranges.

### `length(vector)`

Returns a vector's magnitude. For a 2D coordinate it gives distance from the origin, making circles and radial effects possible.

### `distance(a, b)`

Returns the distance between two points. It is equivalent to `length(a - b)` and can be easier to read when the points matter conceptually.

### `normalize(vector)`

Returns a vector pointing in the same direction with length one, provided the original length is not zero.

Normalized directions make comparisons and lighting calculations independent of the original vector's magnitude.

## Debugging and reasoning vocabulary

### Known-good baseline

A **known-good baseline** is a version that builds, runs, and produces an expected image. It gives experiments a reliable return point.

Without one, several accumulated changes can make it unclear which change caused a problem.

### Prediction

A **prediction** states what you expect before running. It turns experimentation into a test of your mental model rather than random trial and error.

An incorrect prediction is useful evidence: it identifies exactly where understanding can improve.

### Intermediate value

An **intermediate value** is a partial calculation used to build the final result. Naming and visualizing intermediates exposes ranges and structure hidden inside a long expression.

### Compile error

A **compile error** means source code could not be translated because it violates language rules or contains invalid references or types.

C++ compile errors happen during the build; GLSL compile errors happen when the application runs and asks the driver to compile shader text.

### Link error

A shader **link error** means individually compiled stages could not form a compatible program. Mismatched vertex outputs and fragment inputs are a common cause.

### Black screen / invisible geometry

A black screen is a symptom, not a diagnosis. Possible causes include shader failure, incorrect vertex layout, wrong active state, clipping, an invalid viewport, outputting black, or never reaching the draw call.

Debug from the outside inward: confirm execution and constant colour, then inputs, transformations, intermediate values, and full effects.

### Aliasing

**Aliasing** occurs when a sampled image cannot represent the detail or frequency of the underlying signal. It can appear as jagged edges, flicker, crawling patterns, or moiré.

Lecture 3's extreme stripe frequencies can reveal aliasing. The solution is not always “more geometry”; later techniques include filtering, derivatives, antialiasing, and appropriate frequency control.

## One compact mental model

```text
The CPU defines and uploads inputs.
The draw call submits work using current OpenGL state.
The vertex shader transforms each vertex into clip space.
The fixed pipeline assembles primitives, clips them, performs the perspective divide and viewport mapping, then rasterizes and interpolates.
The fragment shader turns inputs and mathematics into output colour.
The framebuffer stores the image that GLFW presents.
```

When a term feels abstract, locate it in this path and ask what data it receives, what it changes, and what stage uses its result.
