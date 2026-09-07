# Course repository scope

This document is intended to clarify the scope of the course repository, and to provide a reference for students, instructors and AI.
This repository is an educational graphics-programming base, not a game engine.

## Principles

1. Keep the current lecture's mechanics visible.
2. Prefer small, readable code over abstraction.
3. Add infrastructure only when it removes irrelevant repetition.
4. Preserve a working baseline.
5. Examples should map to standard OpenGL/GLSL concepts and public references.
6. Students should be able to identify what data is on the CPU, what data is on the GPU, and what each shader stage does.

## Lecture checkpoint policy

`main` represents the latest teaching baseline. Before a lecture materially
changes the shared starter, create an immutable `lecture-XX` tag for the
starting state expected by that lecture's exercises. Exercise READMEs must name
the tag they assume.

- Create a checkpoint only after its baseline is known-good.
- Do not move a tag after it has been published.
- Students should normally create a local branch from a tag instead of editing
  in detached HEAD.
- Create a new lecture tag only when the shared baseline changes materially.

## Planned progression

The repository may later gain, approximately in this order:

1. shader utilities / uniforms;
2. indexed geometry;
3. GLM and transforms;
4. textures;
5. camera;
6. normals and lighting;
7. model/material examples if needed;
8. framebuffer / screen-space example;
9. small optimization demonstrations.
