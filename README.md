# 3D and Shader Programming — Starter Project

Minimal course starter for learning the real-time rasterization pipeline with C++, OpenGL and GLSL.

The project started small and now renders one lit, textured cube using one vertex shader and one fragment shader. It is extended gradually as each course concept becomes relevant.

The goal is not to hide the graphics pipeline behind a large framework. The goal is to make the important data flow and rendering steps visible and understandable.

---

## What this starter demonstrates

- creating an OpenGL 3.3 Core context with GLFW;
- loading OpenGL functions with GLAD;
- uploading vertex data to the GPU;
- describing vertex attributes;
- compiling and linking GLSL shaders from separate files;
- creating model, view and perspective projection matrices with GLM;
- uploading matrix uniforms from C++ to a vertex shader;
- passing data from the vertex shader to the fragment shader;
- transforming and interpolating surface normals and world-space positions;
- interpolating UV coordinates and sampling a generated 2D texture;
- calculating simple ambient, Lambert diffuse and Blinn-Phong specular lighting;
- using depth testing for solid 3D geometry;
- issuing a draw call;
- displaying the resulting fragments in the framebuffer.

It deliberately does **not** yet contain camera controls, model loading, image-file loading, multiple light/texture systems, scene graphs, UI, framebuffers or other higher-level systems.

Those concepts will be introduced when they become relevant during the course.

---

## Officially supported course path

Primary classroom support is provided for:

- Windows 10/11
- Visual Studio 2022
- Visual Studio workload: **Desktop development with C++**
- Git
- CMake 3.24+
- C++20
- OpenGL 3.3 Core
- GLSL 330

The project uses:

- **GLFW 3.4** for window creation, input and the OpenGL context;
- **GLAD** for loading OpenGL functions;
- **GLM 1.0.3** for vectors and transformation matrices.

GLAD is already included in this repository.

GLFW and GLM are downloaded automatically by CMake the first time the project is configured.

You do **not** need to manually download/build GLFW or GLM, or generate GLAD.

Other IDEs, operating systems and build systems are welcome, but you may need to handle environment-specific setup and troubleshooting yourself.

For example, using xmake, CLion, VS Code or another workflow is fine if you prefer it, but the course instructions and most classroom support will assume the setup described here.

---

## First-time setup

### 1. Install Visual Studio 2022

Visual Studio Community is sufficient.

In the Visual Studio Installer, make sure the following workload is installed:

**Desktop development with C++**

This should provide the MSVC compiler and Windows SDK required by the project.

### 2. Install Git

Check that Git is available:

```powershell
git --version
```

### 3. Install CMake

CMake version 3.24 or newer is required.

Check:

```powershell
cmake --version
```

---

## Clone the repository

Clone the course repository:

```powershell
git clone <COURSE-REPO-URL>
cd 3d-shader-course-starter
```

If you already cloned the repository, simply open a terminal in the repository root.

You should see files/folders similar to:

```text
3d-shader-course-starter/
├── CMakeLists.txt
├── README.md
├── src/
├── shaders/
├── exercises/
├── docs/
└── external/
```

---

## Configure and build — command line

From the repository root:

```powershell
cmake -S . -B build
```

Then build:

```powershell
cmake --build build --config Debug
```

The first configure downloads the pinned GLFW and GLM sources from GitHub.

An internet connection is therefore required the first time the project is configured.

After the first successful configure, GLFW and GLM remain inside the local `build` directory until that directory is removed.

---

## Run the project

From PowerShell:

```powershell
.\build\bin\shader_course.exe
```

From Git Bash:

```bash
./build/bin/shader_course.exe
```

Expected result:

- a window titled **3D and Shader Programming**;
- a dark background;
- one static cube with a crisp generated checker texture on every face;
- sampled surface colour modulated by ambient and diffuse lighting, with a restrained specular highlight.

The terminal should also print information similar to:

```text
OpenGL: 3.3.0 ...
Renderer: ...
```

The exact OpenGL version and renderer depend on your graphics hardware and driver.

Press **Escape** or close the window to quit.

---

## Visual Studio workflow

You can use Visual Studio in either of two ways.

### Option 1 — Open the CMake project directly

Open Visual Studio and choose:

**Open a local folder**

Select the repository root.

Visual Studio should detect `CMakeLists.txt` and configure the project.

### Option 2 — Generate a Visual Studio solution

From the repository root:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
```

Then open:

```text
build/ShaderCourseStarter.sln
```

Build and run the `shader_course` target.

The project configures Visual Studio to use the executable directory as its working directory so the shader files can be found correctly.

---

## Project structure

The most important files at the start of the course are:

```text
src/main.cpp
shaders/basic.vert
shaders/basic.frag
```

### `src/main.cpp`

Contains the CPU-side application code.

At this stage it handles things such as:

- creating the window;
- initializing OpenGL;
- defining cube position, normal and UV data;
- uploading that data to the GPU;
- configuring vertex attributes;
- loading/compiling shaders;
- creating and uploading model, view, projection and normal matrices;
- uploading explicit light, viewer and material uniforms;
- creating and binding a small generated 2D texture;
- issuing the draw call;
- running the application loop.

You do **not** need to memorize every OpenGL function immediately.

Focus first on understanding the purpose of each part and how data moves through the program.

### `shaders/basic.vert`

The vertex shader.

It receives data for each vertex and produces the final vertex position used by the graphics pipeline.

It also passes world-space position, normal and UV data forward to the fragment shader.

### `shaders/basic.frag`

The fragment shader.

It receives interpolated data and determines the colour written for each generated fragment.

This is one of the first files you will modify.

---

## Shader files and rebuilding

The source shader files are stored in:

```text
shaders/
```

During a build, CMake copies them next to the executable:

```text
build/bin/shaders/
```

After editing a shader, build the project again:

```powershell
cmake --build build --config Debug
```

Then rerun the executable.

---

## Lecture 1

Once the RGB triangle runs successfully, continue with:

`exercises/lecture-01/README.md`

The first exercises introduce:

- fragment shader modifications;
- vertex colours and interpolation;
- simple GLSL mathematics;
- shader compiler debugging;
- an optional animated uniform.

Earlier lecture exercise documents describe the repository checkpoints used for
those classes. The current common baseline has progressed beyond the original
RGB triangle.

## Lecture 4

For the current normals, lighting, and materials baseline, continue with:

`exercises/lecture-04/README.md`

The exercises focus on:

- surface normals and normalization;
- coordinate-space consistency;
- visualizing normals and `N dot L`;
- Lambert diffuse and a small ambient term;
- viewer direction and Blinn-Phong specular lighting;
- debugging one intermediate lighting value at a time.

Try to predict the result of each shader change before running it.

## Lecture 5

For the current texturing and applied surface-effects baseline, continue with:

`exercises/lecture-05/README.md`

The exercises focus on:

- UV coordinates and interpolation;
- texture objects, texture units and sampler uniforms;
- nearest/linear filtering and repeat/clamp wrapping;
- sampled colour as a lit material;
- texture channels as masks and procedural inputs;
- the distinction between shader alpha and framebuffer blending.

---

## The first mental model

At this stage, focus on this simplified data flow:

```text
CPU vertex data
    ↓
Vertex Buffer / Vertex Attributes
    ↓
Vertex Shader
    ↓
Primitive Assembly
    ↓
Rasterization + Interpolation
    ↓
Fragment Shader
    ↓
Framebuffer
    ↓
Screen
```

You do not need to understand every detail on the first day.

This mental model will be extended throughout the course.

---

## Troubleshooting

### CMake cannot find a compiler

Typical symptoms include messages saying that no C or C++ compiler could be found.

Make sure Visual Studio 2022 is installed with:

**Desktop development with C++**

After installing or changing Visual Studio components, close and reopen your terminal before trying again.

If needed, remove the old build directory and reconfigure.

### CMake cannot download GLFW or GLM

The first configure requires access to GitHub because GLFW and GLM are downloaded automatically.

Check:

- internet connection;
- VPN;
- firewall;
- proxy restrictions;
- whether GitHub is reachable.

Then retry:

```powershell
cmake -S . -B build
```

If the previous configure failed badly, delete the build directory first.

PowerShell:

```powershell
Remove-Item -Recurse -Force build
cmake -S . -B build
```

Git Bash:

```bash
rm -rf build
cmake -S . -B build
```

### The build directory seems broken

CMake build directories are generated files.

It is normally safe to delete `build/` and regenerate it:

```powershell
cmake -S . -B build
cmake --build build --config Debug
```

Optionally, more specific:
```powershell
cmake --build build --config Debug --target shader_course
```

Do not place your own source code inside `build/`.

### The executable cannot find the shaders

The executable expects the shaders to exist next to it under:

```text
build/bin/shaders/
```

A normal build should copy them automatically.

Try rebuilding:

```powershell
cmake --build build --config Debug
```

Then run:

```powershell
.\build\bin\shader_course.exe
```

If using a different IDE or build system, make sure the application's working directory and shader paths are correct.

### Shader compilation fails

This is normal during shader development.

The program prints GLSL compiler errors to the terminal.

Look for:

- the shader filename;
- line numbers;
- syntax errors;
- mismatched inputs/outputs;
- undeclared variables;
- incorrect GLSL types.

Fix the reported problem and run again.

### The window opens but nothing is visible

A black/empty window is one of the most common graphics-programming symptoms.

Do not immediately rewrite everything.

Check one thing at a time:

- Did the shader compile?
- Did the shader program link?
- Is the draw call being reached?
- Is the viewport correct?
- Is the triangle inside visible clip space?
- Are vertex attributes configured correctly?
- Do vertex shader outputs match fragment shader inputs?
- Is the fragment shader producing a visible colour?

Graphics debugging is often about isolating the smallest stage that is not behaving as expected.

### The program reports an OpenGL version below 3.3

Update your graphics driver first.

If your machine genuinely does not support OpenGL 3.3 Core, contact the teacher so we can find an alternative setup.

### CMake says `pthread` was not found on Windows

You may see output similar to:

```text
Looking for pthread_create...
not found
Found Threads: TRUE
Including Win32 support
```

This is normally harmless.

GLFW checks several possible threading implementations during configuration and then selects the Windows implementation.

---

## Resetting to a clean state

If your source code is correct but the generated project behaves strangely, you can recreate the build directory.

PowerShell:

```powershell
Remove-Item -Recurse -Force build
cmake -S . -B build
cmake --build build --config Debug
```

Git Bash:

```bash
rm -rf build
cmake -S . -B build
cmake --build build --config Debug
```

This does not remove your source code.

---

## Using Git

The `build/` directory and normal IDE/build artifacts are ignored by Git.

Your own work should normally happen in:

```text
src/
shaders/
exercises/
```

Commit your work regularly.

Before committing, it is useful to check:

```powershell
git status
```

Do not commit the generated `build/` directory.

### Lecture checkpoints and catching up

The `main` branch contains the newest course baseline and will keep changing as
new lectures introduce new concepts. Older exercises may therefore refer to
code that is no longer present on the latest `main`.

A Git tag is a named snapshot of the repository at a particular point in the
course. Each exercise README names the lecture tag containing the code state it
expects. If you are caught up and your current work is running correctly, stay
on `main`.

If you need to complete an older lecture, fetch the tags and create a local
catch-up branch from the relevant snapshot. For Lecture 2, use:

```powershell
git fetch --tags
git switch -c catchup-lecture-02 lecture-02
```

The catch-up branch exists only in your local repository unless you choose to
push it. It is a safe place to edit and commit the older lecture state; unlike
checking out a tag directly, it does not leave you in detached HEAD.

When you are ready to return to the newest course version:

```powershell
git switch main
git pull
```

---

## Sources and further reading

The initial project follows the same fundamental progression used by LearnOpenGL:

1. **Creating a Window**
2. **Hello Triangle**
3. **Shaders**

LearnOpenGL is one of the main practical references for the course:

https://learnopengl.com/

You are encouraged to use it alongside the course explanations, demonstrations and exercises.

The purpose of the course is **not** to simply complete LearnOpenGL tutorials in sequence.

Use tutorials and documentation to build understanding, then experiment, modify, combine and apply the concepts yourself.

Other references will be introduced during the course.

---

## A note about OpenGL vs. game engines

OpenGL is used as the common course foundation because it makes important parts of the rendering pipeline visible.

Modern engines such as Unity, Unreal Engine and Godot provide higher-level systems around many of the same underlying graphics concepts.

The important transferable ideas include:

- vertex data;
- coordinate spaces;
- transformations;
- shader stages;
- textures;
- normals;
- lighting;
- render targets;
- visibility;
- GPU performance.

Students may use other environments for their individual project when appropriate, but the course starter provides the most strongly supported path for learning the fundamentals.

---

## Dependency versions

The course starter currently uses:

- **GLFW 3.4**
- **GLM 1.0.3**
- **vendored GLAD 2-generated OpenGL loader**
- **OpenGL 3.3 Core**
- **GLSL 330**
- **C++20**
- **CMake 3.24+**

GLFW is pinned by `CMakeLists.txt`.

GLM is also pinned by `CMakeLists.txt` and used as a header-only dependency.

The GLAD-generated source files are included directly in the repository so students do not need Python or a GLAD generator.

This provides a consistent baseline for the class.

---

## If you get stuck

Try this process before assuming everything is broken:

1. Read the compiler, linker or shader error carefully.
2. Return to the last known-working version.
3. Change only one thing at a time.
4. Simplify the output.
5. Visualize intermediate shader values where possible.
6. Compare against the course starter or relevant documentation.
7. Ask for help and include the actual error/output you are seeing.

A broken image, black screen or shader compiler error is not unusual in graphics programming.

Learning how to systematically investigate those failures is part of the course.
