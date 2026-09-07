# Graphics techniques and mathematics for student projects

This document helps translate a project idea into a small graphics experiment. It is not a requirement list and not a promise that every technique belongs in the course starter.

The current repository already provides a useful experimental base:

- one triangle with local positions and interpolated colour;
- model, view, and perspective projection matrices;
- an interpolated 2D `localCoord` domain;
- a `time` uniform;
- editable vertex and fragment shaders.

It does **not** yet provide textures, a camera, indexed meshes, depth testing, normals, lighting, model loading, framebuffers, or post-processing. Techniques needing those systems are marked as later infrastructure.

## Start with the visual question

Before choosing mathematics or architecture, complete this sentence:

> I want the image to visibly demonstrate ________.

Then ask:

1. What changes across the surface: colour, position, brightness, direction, or visibility?
2. What should it depend on: local position, world position, view direction, screen position, texture coordinates, or time?
3. Which shader stage owns the change?
4. What is the smallest geometry and data needed to test it?
5. What intermediate value could be shown as colour to prove it works?

The smallest recognizable experiment is normally a better first milestone than a complete content pipeline.

## A practical selection map

| Desired result | Useful starting technique | Main mathematics | Likely stage | Baseline support |
|---|---|---|---|---|
| Stripes, waves, pulses | procedural scalar pattern | `sin`, frequency, phase, remapping | fragment | available now |
| Repeated tiles or cells | coordinate repetition | `fract`, `floor`, scale | fragment | available now |
| Circles, rings, soft shapes | distance field | `length`, `distance`, thresholds | fragment | available now |
| Colour gradients | interpolation and remapping | linear interpolation, `mix` | fragment | available now |
| Moving procedural pattern | coordinate + time | offset, phase, velocity | fragment | available now |
| Object movement | model transformation | translation, rotation, scale | CPU + vertex | available now |
| Vertex deformation | change positions procedurally | vectors, waves, matrix spaces | vertex | available now, limited geometry |
| Texture artwork or sprites | texture sampling and UVs | coordinate mapping, filtering | fragment | later infrastructure |
| Navigable 3D scene | camera and depth | view matrices, perspective, depth | CPU + vertex + fixed pipeline | later infrastructure |
| Lit surface | normals and lights | dot products, normalization | vertex/fragment | later infrastructure |
| Imported 3D model | mesh/model loading | indexed geometry, transforms | CPU + vertex | later infrastructure |
| Outline, blur, image distortion | post-processing | screen-space sampling, kernels | fragment | later infrastructure |
| Particles | many small moving elements | velocity, lifetime, random seeds | CPU/GPU + shaders | later infrastructure |

## Technique families available in the current baseline

### 1. Colour gradients and masks

**Useful for:** stylized backgrounds, heat maps, fades, bands, warning zones, palette experiments, transitions, or debugging.

**Core idea:** create a scalar that changes over the triangle, then use it as brightness or as the blend amount between two colours.

```glsl
float mask = localCoord.y * 0.5 + 0.5;
vec3 colour = mix(vec3(0.1, 0.2, 0.8), vec3(1.0, 0.4, 0.1), mask);
```

**Mathematics:** remapping, clamping, linear interpolation, thresholds.

**Why it works:** a scalar field gives every fragment a number. Colour makes that number visible, while `mix` turns it into a controlled transition.

**Smallest experiment:** display `mask` as grayscale first. Then use exactly the same mask to mix two colours.

**Common mistake:** assuming the input already lies in `[0, 1]`. Inspect its range and remap deliberately.

### 2. Stripes, waves, and oscillation

**Useful for:** scan lines, energy fields, ripples, animated signals, flags, water suggestions, or abstract motion graphics.

```glsl
float wave = sin(localCoord.x * frequency + time * speed);
float visibleWave = wave * 0.5 + 0.5;
```

**Mathematics:** sine/cosine, amplitude, frequency, phase, speed, range remapping.

- **amplitude** controls how strong the variation is;
- **frequency** controls how many cycles fit in the domain;
- **phase** shifts where a cycle begins;
- adding `time * speed` changes phase continuously.

**Smallest experiment:** show the remapped sine as grayscale. Only then threshold or colour it.

**Common mistake:** applying `sin(time)` and expecting spatial movement. That expression is global because every fragment receives the same time. Include a coordinate to make the result vary across space.

### 3. Repetition and cells

**Useful for:** grids, tiles, windows, repeated symbols, digital patterns, checker-like divisions, or quantized animation.

```glsl
vec2 repeated = fract(localCoord * scale);
```

`fract` turns increasing coordinates into a repeating ramp. `floor(localCoord * scale)` can identify which repeated cell a fragment occupies.

**Mathematics:** multiplication as domain scaling, fractional part, integer-like cell indices, parity or modular patterns.

**Smallest experiment:** visualize `repeated.x`, then `repeated`, before constructing cell boundaries.

**Common mistakes:**

- treating a discontinuity at each cell boundary as an error when it is the purpose of `fract`;
- using a frequency too high for the framebuffer, causing aliasing;
- combining many operations before checking the repeated coordinates.

### 4. Distance fields and radial shapes

**Useful for:** circles, rings, targets, radar displays, glows, soft blobs, simple icons, or proximity effects.

```glsl
float distanceFromCentre = length(localCoord - centre);
float circle = 1.0 - smoothstep(radius, radius + softness, distanceFromCentre);
```

**Mathematics:** vector subtraction, Euclidean length, radius, thresholding, smooth boundaries.

**Why it works:** subtracting `centre` expresses the fragment relative to the chosen centre. `length` reduces that 2D offset to one distance value. A threshold turns distance into inside/outside.

**Smallest experiment:** show raw distance as grayscale. Then apply a single `step` or `smoothstep`.

**Common mistake:** trying to draw a circle by testing X and Y separately. That usually creates a box because each axis is bounded independently; distance combines both axes radially.

### 5. Combining scalar fields

**Useful for:** more complex symbols, layered procedural artwork, transitions between effects, cutouts, and simple constructive shape design.

Useful operations include:

- `min(a, b)` to favor the smaller distance;
- `max(a, b)` to favor the larger constraint;
- subtraction or inverted masks for cutouts;
- `mix(a, b, mask)` for blends;
- multiplication for intersection-like mask behavior.

**Mathematics:** scalar fields, Boolean-like mask logic, minimum/maximum, interpolation.

**Smallest experiment:** show each field separately, then show the combined field, then threshold it.

**Common mistake:** combining already-thresholded shapes too early. Keeping continuous distance or mask values longer often produces more controllable edges.

### 6. Time-based animation

**Useful for:** pulses, moving waves, colour cycling, breathing effects, shifting thresholds, or revealing how a system changes.

Time does not create motion by itself. It must modify a meaningful parameter:

```glsl
float movingX = localCoord.x + time * speed;        // offset
float wave = sin(localCoord.x * frequency + time); // phase
float pulse = 0.5 + 0.5 * sin(time);               // global value
```

**Mathematics:** functions of time, rate/speed, phase, periodic motion.

**Smallest experiment:** replace `time` temporarily with a constant. Compare several constants to understand the animation as a sequence of still images.

**Common mistake:** making every parameter time-dependent at once. Animate one parameter so its responsibility remains visible.

### 7. Vertex deformation

**Useful for:** wobbling surfaces, waves, breathing shapes, stylized distortion, flags, terrain displacement, or audio-reactive forms.

The vertex shader can modify local position before the model/view/projection chain:

```glsl
vec3 displacedPosition = aPosition;
displacedPosition.y += sin(aPosition.x * frequency + time) * amplitude;
gl_Position = projection * view * model * vec4(displacedPosition, 1.0);
```

**Mathematics:** displacement vectors, sine waves, amplitude/frequency, coordinate spaces.

**Smallest experiment:** move one coordinate by a constant, then by a spatial function, then add time.

**Current limitation:** the baseline has only three vertices. A vertex shader can only move existing vertices; it cannot create a smoothly subdivided wave from one triangle. A denser mesh would be needed for detailed deformation.

**Common mistake:** expecting fragment-level detail from vertex displacement. Rasterization interpolates between transformed vertices, so geometric resolution matters.

### 8. Object transformation and hierarchy thinking

**Useful for:** placing project elements, spinning symbols, arranging repeated objects, orbiting, simple scene composition, or comparing local and world behavior.

**Mathematics:** translation, rotation, scale, matrix multiplication, pivots, transformation order.

**Smallest experiment:** use the model matrix to apply one transformation. Then combine two and swap their order.

To rotate around another point:

```text
move pivot to origin -> rotate -> move back
```

With column vectors, remember that the operation closest to the vertex acts first, even though C++ helper-call order may require careful reading of how GLM composes the matrix.

**Common mistake:** trying to repair transform-order problems by changing arbitrary signs. Write down the intended sequence in words first.

### 9. Coordinate-space choice as a design tool

The mathematics may be identical while the chosen coordinate space changes the visual behavior.

| Domain | Pattern tends to follow | Useful for |
|---|---|---|
| local/object position | the object itself | object markings, deformation, object-centred effects |
| world position | placement in a shared scene | world grids, height bands, location-based effects |
| view position | the viewer's frame | distance/view-dependent effects |
| UV coordinates | authored 2D surface mapping | textures, decals, stable surface patterns |
| screen coordinates | the output image | post-processing, screen overlays, pixel effects |

The current fragment shader has local/object XY through `localCoord`. Other domains require passing or deriving appropriate data rather than merely renaming `localCoord`.

**Smallest experiment:** state which coordinate space a pattern should follow before writing the expression. Move the object and ask whether the pattern should move with it.

## Project families and sensible first prototypes

### Abstract procedural artwork

**Good first techniques:** scalar visualization, waves, repetition, distance fields, masks, colour palettes, time.

**Useful mathematics:** remapping, `sin`, `fract`, `length`, `step`, `smoothstep`, `mix`, coordinate transformations.

**First prototype:** one recognizable static pattern on the triangle with one grayscale intermediate view. Add time only after the static structure is understood.

**Growth path:** add a full-screen surface or more appropriate domain later; then explore antialiasing, multiple layers, noise, or interaction if justified.

### Space, nebulas, fractals, stars, and planets

“Space” can describe several technically different projects. A nebula is mainly a layered colour-and-density problem. A starfield is a distribution-and-shape problem. A rotating lit planet is a 3D geometry, normal, lighting, and camera problem. A fractal is an iterative mathematical structure that might be used alone or as one ingredient in another effect.

Separate these questions before choosing infrastructure. A convincing first prototype does not need to solve all of them at once.

#### Nebula or gas-cloud study

**Useful techniques:** scalar fields, layered noise, colour ramps, masks, coordinate distortion, slow time-based motion, and optional additive-looking highlights.

**Useful mathematics:** remapping, interpolation, multiple frequencies, amplitude reduction, deterministic noise, and domain warping.

A common procedural-noise building block is **fractal Brownian motion**, usually abbreviated **fBm**. Despite the name, this graphics technique does not automatically produce a mathematical fractal object. It layers several versions—often called **octaves**—of a noise function:

```text
result = broad, strong variation
       + finer, weaker variation
       + finer still, weaker still
       + ...
```

For each octave, frequency normally increases while amplitude decreases. This creates detail at several scales, which is useful for clouds, smoke, terrain, and nebulas.

Conceptually:

```text
float value = 0.0;
float amplitude = 0.5;
float frequency = 1.0;

for each octave:
    value += amplitude * noise(position * frequency);
    frequency *= 2.0;
    amplitude *= 0.5;
```

The exact ratios are artistic parameters, not laws. More octaves increase detail and shader cost. Begin with two or three clearly visible layers rather than a large loop whose contribution is difficult to inspect.

**Domain warping** means using one field to distort the coordinates supplied to another:

```text
warp = noise(position)
density = noise(position + warp * strength)
```

Instead of merely changing the final colour, the first field bends the domain in which the second field is evaluated. This can produce flowing, folded, cloud-like structure useful for nebulas.

**Smallest baseline prototype:** the current repository does not yet provide a noise function, but the idea can be tested with existing mathematics. Combine two low-frequency sine fields into one scalar, visualize it as grayscale, distort the coordinate of one field with the other, then map the result between two or three space-like colours. Add very slow time only after the static density field is understandable.

**Debug views:**

1. original coordinate;
2. first scalar field;
3. warp amount;
4. warped scalar field;
5. final colour ramp.

**Likely later needs:** a full-screen surface or larger domain; a deliberate hash/noise implementation or noise texture; blending if multiple transparent layers are composed; possibly an off-screen pass for bloom-like glow.

**Common risks:**

- calling several arbitrary sine waves “noise” without understanding their visible repetition;
- adding too many octaves before inspecting each one's range and contribution;
- moving every layer at the same speed, making the result look like a sliding texture rather than changing gas;
- treating post-processing glow as necessary before the underlying density and colour structure works.

#### Fractal study

The word **fractal** can refer to different approaches:

- escape-time fractals such as Mandelbrot or Julia sets;
- recursively or iteratively repeated shapes;
- self-similar coordinate folding;
- fractal noise such as fBm, which layers detail across scales.

These share repetition or structure across scales, but they are not interchangeable algorithms.

**Useful techniques:** repeated coordinate transformation, iteration with a fixed maximum count, distance estimation or escape tests, palette mapping, zoom and pan, and visualizing iteration count.

**Useful mathematics:** complex-number-like 2D operations, scaling, rotation, absolute-value folding, recurrence, logarithms for smoother colouring, and careful numeric ranges.

**Smallest prototype:** choose one formulation and one question. For an escape-time fractal, first output normalized iteration count as grayscale. For a folding fractal, display the coordinate after one iteration, then two, before using many iterations.

**Shader-stage choice:** this is normally fragment-shader work because every fragment evaluates a different coordinate. The current `localCoord` can test the mathematics on the triangle, although a rectangular/full-screen domain would present the complete image more naturally.

**Common risks:**

- using a high iteration count before verifying that one iteration is correct;
- confusing fBm with Mandelbrot-style iteration because both are called fractal;
- animating zoom, parameters, and colour simultaneously;
- assuming greater iteration count always creates useful visible detail;
- ignoring floating-point precision during deep zooms.

#### Procedural starfield

**Useful techniques:** repeated cells, a deterministic hash, distance-based point shapes, brightness variation, sparse thresholds, colour variation, and optional twinkling.

A practical 2D starfield recipe is:

```text
coordinate
    -> scale into a grid
    -> separate cell ID with floor
       from position within the cell with fract
    -> hash the cell ID to get stable pseudo-random values
    -> use those values for star presence, position, size and brightness
    -> draw the star from distance to its position
```

A **hash** is important because it gives each cell a repeatable pseudo-random-looking value. The star should remain stable from frame to frame. Calling the result random describes its appearance; for the same input it is deterministic.

The local star shape can begin with:

```glsl
float distanceToStar = length(positionInCell - starPosition);
float star = 1.0 - smoothstep(radius, radius + softness, distanceToStar);
```

**Smallest baseline prototype:** make a regular grid with `fract`, put one circular point in every cell using `length`, and visualize it. The next approved step would be adding a small deterministic hash so only some cells contain stars and their sizes vary.

**Twinkling:** vary brightness slightly with time, but give stars different phases derived from their cell IDs. If every star uses only `sin(time)`, the entire field pulses together.

**Emissive appearance:** an emissive star contributes its own bright colour rather than being illuminated like a matte surface. A bright core plus a wider, dimmer distance-based halo can suggest emission. True bloom spreading beyond the star normally requires framebuffer/post-processing infrastructure.

**Likely later needs:** a full-screen domain, an aspect-ratio or resolution-aware coordinate, optional layered star depths for parallax, blending or post-processing for richer glow.

**Common risks:**

- changing the hash with time, which makes stars pop to unrelated positions;
- making every cell contain an equally sized star;
- using a grid frequency too high to sample cleanly;
- calling a single 2D layer a navigable 3D starfield without adding depth or parallax behavior.

#### Rotating lit planet

A shaded circle can look planet-like, but a genuinely rotating 3D planet requires geometry and surface orientation. Keep these two prototypes conceptually distinct.

**2D planet-card prototype:** use `length` to create a circular mask, reconstruct or approximate a sphere-facing normal inside the circle, apply one light direction, and rotate the procedural surface coordinates. This can test appearance quickly but remains an image drawn on flat geometry.

**3D planet prototype:** use sphere geometry, transform it with model/view/projection matrices, transform and interpolate normals, and calculate lighting per fragment. Rotate the model matrix to rotate the sphere.

A sensible staged path is:

```text
1. show the circle or sphere silhouette
2. visualize normals as RGB
3. visualize the diffuse dot product as grayscale
4. apply one directional-light colour
5. rotate the model or surface deliberately
6. add simple surface detail
7. add atmosphere or specular response only if useful
```

**Diffuse lighting:** for normalized surface normal `N` and direction toward the light `L`:

```glsl
float diffuse = max(dot(N, L), 0.0);
```

The dot product measures how directly the surface faces the light. Clamping at zero prevents the light from illuminating the back-facing side with a negative value.

**Rotation:** rotating sphere geometry with the model matrix changes both its positions and its normals. Rotating only a procedural colour pattern can suggest surface rotation while the geometric sphere remains stationary. These may look similar in a simple image but represent different operations.

**Surface detail:** begin with a simple latitude band, radial coordinate, or procedural mask. Textured continents require UV coordinates and texture sampling; procedural continents require a suitable spherical domain and noise-like field.

**Atmosphere rim:** a simple rim factor compares the surface normal with the direction toward the viewer:

```glsl
float facing = max(dot(normalize(N), normalize(V)), 0.0);
float rim = 1.0 - facing;
```

Shaping `rim` with a power or `smoothstep` concentrates it near the silhouette. This is a stylized view-dependent mask, not a physically complete atmosphere simulation. A halo extending outside the planet silhouette may require extra geometry, transparency/blending, or post-processing.

**Likely later needs for the 3D version:** sphere/indexed geometry, normals, normal transformation, depth testing, camera/view control, and optionally UVs/textures. Specular highlights need the view direction as well as the normal and light direction.

**Common risks:**

- attempting final lighting before visualizing normals;
- rotating positions but not transforming normals consistently;
- confusing a rotating texture/pattern with rotating geometry;
- adding multiple lights, shadows, atmosphere, and post-processing before one diffuse light works;
- expecting the current three-vertex triangle to become a detailed 3D sphere through fragment colour alone.

#### Combining a space scene

Treat the scene as composable layers with different responsibilities:

```text
background density/nebula
    + stable star distribution
    + foreground planet geometry and lighting
    + restrained atmosphere/emission
    + optional post-processing glow later
```

Build and debug the layers independently. A practical project could deliberately stop after any one layer if it already demonstrates the chosen question well.

**Recommended smallest integrated experiment:** create one static nebula-like scalar field as a colour background study, one separately debugged star-cell pattern, or one lit planet—not all three at first. Choose the strongest result only after those isolated tests reveal the real cost and visual potential.

### Stylized logo, icon, or motion graphic

**Good first techniques:** 2D distance fields, masks, colour transitions, controlled timing.

**Useful mathematics:** distances to points/lines, smooth thresholds, easing, rotation and scale.

**First prototype:** construct one component—circle, ring, line, or split—not the complete logo. Animate one reveal or pulse.

**Likely later need:** a rectangular/full-screen surface, aspect-ratio correction, or authored vector/texture assets for complex exact shapes.

### Water, energy, fire, aura, or magical effect

**Good first techniques:** layered waves, distortion of coordinates, scrolling patterns, colour ramps, masks.

**Useful mathematics:** multiple sine frequencies, phase offsets, vector addition, gradients, time, noise concepts.

**First prototype:** prove one moving band or ripple. Combine two frequencies only after each is visible separately.

**Likely later need:** textures/noise, transparency and blending, depth-aware composition, or particles depending on the intended result.

**Risk:** naming an effect “water” or “fire” does not define its observable features. Decide whether the key is motion, silhouette, refraction, colour, surface deformation, or particles.

### Terrain, planet, or organic surface

**Good first techniques:** radial distance, vertex displacement, height-based colouring, transformation.

**Useful mathematics:** functions over position, normal vectors, gradients, spherical coordinates, noise, interpolation.

**First prototype:** on simple geometry, visualize a height function as colour before using it to displace vertices.

**Likely later need:** denser/indexed geometry, normals, lighting, camera, depth testing, and possibly procedural noise.

**Risk:** the current three-vertex triangle cannot demonstrate detailed terrain geometry.

### 3D object viewer or scene

**Good first techniques:** model/view/projection, input-controlled camera, depth testing, mesh data, lighting.

**Useful mathematics:** coordinate spaces, transformation composition, perspective, direction vectors, dot products.

**First prototype:** keep the current triangle and deliberately place two simple objects or views only after the needed geometry/depth baseline is approved.

**Likely later need:** camera controls, indexed meshes, depth buffer, model loading, normals, materials.

**Risk:** building a general engine can consume the project without producing the intended visual study. Add only the systems the chosen scene actually needs.

### Lighting or material study

**Good first techniques:** surface normals, light direction, diffuse and specular terms, view direction, colour/material parameters.

**Useful mathematics:** normalized vectors, dot products, reflection or half vectors, inverse-transpose normal transformation, attenuation.

**First prototype:** visualize normals as RGB, then implement one directional diffuse light on one simple object.

**Likely later need:** normals in vertex data, depth testing, suitable 3D geometry, and possibly a camera.

**Risk:** debugging final lit colour before confirming normals and directions. Visualize each vector or scalar term separately.

### Texture, sprite, or pixel-art project

**Good first techniques:** UV coordinates, texture sampling, nearest/linear filtering, atlas regions, alpha blending.

**Useful mathematics:** 2D coordinate mapping, scaling/offset, repetition, sampling, colour multiplication.

**First prototype:** sample one known texture on one simple surface and display it without lighting.

**Likely later need:** image loading, a texture object, sampler uniform, UV vertex attribute, and possibly blending.

**Risk:** confusing local coordinates with authored UVs. UVs describe where surface points sample an image; they need a deliberate mapping.

### Pixelation, blur, distortion, outline, or post-processing

**Good first techniques:** render-to-texture, full-screen pass, neighbouring samples, screen coordinates.

**Useful mathematics:** convolution kernels, finite differences, coordinate offsets, resolution/texel size, luminance.

**First prototype:** display an off-screen colour texture unchanged through a second pass. Only then add one distortion or kernel.

**Likely later need:** framebuffer objects, textures, a full-screen triangle/quad, and a resolution or texel-size input.

**Risk:** trying to implement post-processing directly on the current triangle's local coordinates. Post-processing operates on an already-rendered image in screen space.

### Particles, sparks, rain, or crowds of small elements

**Good first techniques:** per-particle position/velocity/lifetime, many repeated draws or instancing, blending, billboard orientation.

**Useful mathematics:** velocity integration, acceleration, lifetime normalization, random/hash functions, distributions.

**First prototype:** animate a small fixed number of points on the CPU and render them visibly before adding thousands or GPU simulation.

**Likely later need:** more geometry, dynamic buffers or instancing, blending, camera/depth decisions.

**Risk:** optimizing scale before one particle's lifecycle and visual style are correct.

### Interactive visualization or simulation

**Good first techniques:** input mapping, parameter uniforms, coordinate conversion, clear visual state, controlled update step.

**Useful mathematics:** normalization, interpolation, rates over time, vector fields, numerical integration depending on the simulation.

**First prototype:** map one keyboard or mouse input to one clearly visible parameter.

**Likely later need:** richer input handling, coordinate conversion from mouse/window to the chosen domain, data buffers, or compute-like techniques.

**Risk:** coupling simulation rules, rendering, and input before each can be inspected separately.

## Mathematics toolkit by purpose

### Position, direction, and offset

- **position:** where something is in a coordinate space;
- **direction:** which way, without an inherent location;
- **offset/displacement:** the vector from one position to another.

```text
offset = destination - origin
destination = origin + offset
```

This distinction supports movement, camera reasoning, lighting, and distance calculations.

### Vector length and distance

```glsl
float magnitude = length(v);
float separation = distance(a, b); // length(a - b)
```

Use length when magnitude matters and distance when the relationship between two positions matters.

### Normalization

```glsl
vec3 direction = normalize(destination - origin);
```

Normalization separates direction from magnitude. This is essential when a dot product should describe angle rather than be distorted by vector lengths.

Guard conceptually against normalizing a zero-length vector, whose direction is undefined.

### Dot product

The **dot product** converts two vectors into one scalar describing directional alignment when the vectors are normalized.

```text
 1  -> same direction
 0  -> perpendicular
-1  -> opposite directions
```

Useful for diffuse lighting, facing tests, view-dependent effects, projections, and angular masks.

This technique is not yet used in the baseline but becomes central once normals and lighting arrive.

### Cross product

The 3D **cross product** produces a vector perpendicular to two input vectors. It is useful for constructing coordinate frames, camera axes, face normals, and orientation reasoning.

Its direction depends on input order, so swapping arguments reverses the result.

### Linear interpolation

Linear interpolation moves between values:

```glsl
value = mix(start, end, amount);
```

Use it for colours, positions, parameters, and transitions. The amount is commonly in `[0, 1]`, although GLSL `mix` can extrapolate outside that range.

### Remapping ranges

To map a value from `[oldMin, oldMax]` to `[newMin, newMax]`, first normalize its position in the old range and then scale into the new range:

```text
t = (value - oldMin) / (oldMax - oldMin)
result = newMin + t * (newMax - newMin)
```

This is useful for visualization, input mapping, palette lookup, animation phases, and parameter control.

### Thresholds and smooth boundaries

- comparison or `step`: hard region boundary;
- `smoothstep`: gradual transition over a chosen width.

Use a hard threshold when discrete classification is the goal. Use a smooth threshold when a boundary needs a controllable transition or when harsh sampling artifacts are distracting.

### Periodic functions

Sine and cosine repeat. Their reusable pattern is:

```text
amplitude * sin(coordinate * frequency + time * speed + phase)
```

Each term has an interpretable responsibility. Keeping them named makes tuning and debugging easier.

### Matrices and transformations

Matrices provide a consistent way to transform positions and directions and to compose several operations.

The current explicit path is:

```text
local --model--> world --view--> view --projection--> clip
```

Choose the matrix based on responsibility:

- model: place or shape the object;
- view: express the world relative to the viewer;
- projection: define perspective and the visible volume.

### Derivatives and rates of change

A **derivative** describes how quickly a value changes. Even without formal calculus, graphics uses this idea frequently:

- velocity is change in position over time;
- a surface normal relates to how a surface changes;
- screen-space shader derivatives estimate how values vary between nearby fragments;
- antialiasing depends on matching detail to sampling rate.

GLSL derivative functions and formal antialiasing are later topics, but “how quickly does this value change?” is already a useful debugging question.

### Randomness, hashes, and noise

Procedural graphics often needs variation rather than true unpredictable randomness.

- a **hash** converts an input such as a cell ID into a repeatable pseudo-random-looking value;
- **value/gradient noise** blends structured pseudo-random values smoothly;
- **fractal noise** combines several frequencies and amplitudes.

These are useful for terrain, clouds, fire, organic motion, and texture variation, but should be introduced after coordinates, scalar visualization, repetition, and interpolation are comfortable.

## Choosing the shader stage

### Prefer the vertex shader when...

- the effect changes geometry or clip-space position;
- a value is naturally calculated once per vertex;
- interpolation of the result across a primitive is acceptable or desirable.

Remember that vertex-level detail is limited by vertex density.

### Prefer the fragment shader when...

- the effect changes colour or surface appearance;
- detail should vary for each generated fragment;
- procedural masks, textures, or screen-space calculations are involved.

Fragment work can run many more times than vertex work, so expensive expressions have a different performance cost.

### Prefer CPU-side code when...

- preparing resources or data;
- handling files, window state, or input;
- constructing a transform shared by the whole draw;
- coordinating objects and draw calls;
- performing work that does not benefit from per-vertex or per-fragment parallelism.

The right question is not “CPU or GPU is always faster?” It is “at what frequency is this value needed, which stage owns its inputs, and how much data would need to cross the boundary?”

## A project experiment template

Use this compact plan before adding infrastructure:

### Visual question

What visible behavior will the experiment test?

### Domain and space

Which coordinates or inputs should control it, and in which coordinate space?

### Stage

Should it change vertex position, fragment colour, or CPU-side scene state?

### Smallest implementation

What is the fewest new values, operations, and resources required?

### Debug views

Which input and intermediate scalar/vector will be visualized directly?

### Known-good result

What should be visible if the experiment works?

### Biggest risk

What missing concept or infrastructure is most likely to block it?

### Fallback

What smaller version still demonstrates the central idea?

## Example: reducing an ambitious idea

Suppose the idea is “an interactive magical ocean around a moving 3D character.” That description contains several separate problems:

- animated surface motion;
- water appearance;
- 3D geometry;
- camera/navigation;
- character assets and animation;
- interaction;
- composition and possibly transparency or reflection.

A useful first experiment might instead be:

> On the existing triangle, create a two-frequency blue procedural wave from `localCoord`, visualize the combined scalar, then animate only its phase with `time`.

That prototype cannot prove the whole project, but it tests the core shader-math question without first building a game engine. The result provides evidence for the next decision: whether the project most needs denser geometry, texture distortion, transparency, lighting, or a different scope.

## Final principle

Choose mathematics because it explains or controls a visible relationship—not because the function sounds advanced.

A strong project can often be built from a short chain:

```text
choose an input
    -> understand its coordinate space and range
    -> shape it into an interpretable scalar or vector
    -> visualize the intermediate
    -> use it to control colour, position, or blending
    -> add time or interaction only when the static result is understood
```

If every step can be explained and debugged, the technique is serving the project rather than hiding it.
