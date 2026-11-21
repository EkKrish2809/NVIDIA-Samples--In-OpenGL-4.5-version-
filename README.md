# Computer Graphics

This repository contains modern OpenGL experiments and ports of NVIDIA CUDA samples from the legacy fixed-function pipeline to shader-based OpenGL 4.5+. Development is organized into a reusable OpenGL engine and individual NVIDIA sample implementations.

The project uses OpenGL context and shader APIs, CUDA/OpenGL interoperability, and platform-specific windowing code. It is intended as a practical reference for learning how to replace immediate mode and legacy matrix APIs with VAOs, VBOs, GLSL shaders, and CUDA-managed graphics resources.

## Repository structure

- `OpenGL/Engine/` - Cross-platform C++ OpenGL engine with X11/Win32 window management, an OpenGL 4.5-style renderer, GLSL shader helpers, Dear ImGui, camera utilities, logging, and a PBR example.
- `OpenGL/ImGui_Demo/` - Standalone Dear ImGui and OpenGL demo application.
- `NVIDIA-Samples/CudaGLFFTOcean/impl/` - CUDA FFT ocean simulation port. CUDA kernels generate and update the ocean height and slope maps while OpenGL renders the mesh.
- `NVIDIA-Samples/CudaGLFFTOcean/ref/` - Reference material for the ocean sample.

## Current samples

### CUDA FFT Ocean

The ocean sample demonstrates a GPU-based height-field simulation:

- Generates the wave spectrum with CUDA.
- Executes an inverse FFT with cuFFT.
- Updates OpenGL vertex/height and slope buffers through CUDA/OpenGL interoperability.
- Calculates surface normals from the generated slope field.
- Renders the animated ocean with GLSL 460 vertex and fragment shaders.

### OpenGL engine

The engine includes a shader-based Hello Triangle scene, PBR rendering code, an integrated camera, timers, logging, and Dear ImGui. It is being used as the common foundation for new and ported samples.

## Technologies

- C++ and GLSL
- OpenGL 4.5+ and GLSL 460
- GLEW
- CUDA Runtime and cuFFT
- CUDA/OpenGL graphics interoperability
- X11 on Linux and Win32 on Windows
- Simple OpenGL Image Library (SOIL)
- Dear ImGui

## Practical requirements

Requirements depend on the sample being built.

### For the OpenGL engine

- A C++ compiler
- GLEW and OpenGL development libraries
- X11 development libraries on Linux
- Visual Studio C++ tools and GLEW on Windows
- A GPU and driver supporting the shaders and OpenGL features used by the sample

### For the CUDA FFT Ocean sample

- An NVIDIA GPU
- A CUDA Toolkit providing `nvcc`, the CUDA Runtime, and cuFFT
- GLEW
- SOIL
- X11 development libraries
- An NVIDIA OpenGL/CUDA interop-compatible driver

The repository does not currently provide a complete CMake build. Most components use the provided shell or batch build scripts.

## Building

Run build commands from the directory containing the relevant script so relative shader, model, and asset paths resolve correctly.

### Linux: OpenGL engine

```bash
cd OpenGL/Engine
chmod +x build.sh
./build.sh
```

The script compiles the engine sources, links against X11, OpenGL, and GLEW, and starts `Engine`.

### Linux: ImGui demo

```bash
cd OpenGL/ImGui_Demo
chmod +x build.sh
./build.sh
```

The script creates and runs `3Dshapes`.

### Linux: CUDA FFT Ocean

```bash
cd NVIDIA-Samples/CudaGLFFTOcean/impl
chmod +x build.sh
./build.sh
```

The script uses `nvcc` to compile the CUDA and C++ sources and links X11, OpenGL, GLEW, SOIL, and cuFFT. It also enables NVIDIA PRIME render-offload variables before starting the sample.

### Windows: OpenGL engine

Open a Visual Studio developer command prompt, set GLEW's include and library paths as required by `build.bat`, then run:

```bat
cd OpenGL\Engine
build.bat
```

## Ocean controls

| Key | Action |
| --- | --- |
| `F` / `f` | Toggle fullscreen |
| `W` / `w` | Toggle wireframe |
| `P` / `p` | Toggle point rendering |
| `Space` | Pause or resume animation |
| `Escape` | Exit |

## Development notes

- Keep portable sample code separate from each platform-specific window implementation.
- Prefer VAOs, VBOs, DSA or equivalent modern buffer workflows, and GLSL shader programs over fixed-function OpenGL APIs.
- Match shader GLSL versions with the capabilities requested by the application context.
- Synchronize explicitly between CUDA and OpenGL when a shared graphics resource is being accessed.
- Release CUDA resources and OpenGL objects during teardown.
- Run each sample from its implementation directory because scripts and source code can use relative paths.

This repository is a work in progress. Some legacy samples and platform-specific build steps are still being migrated or cleaned up.