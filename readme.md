# Soft Renderer

A CPU-based 3D software renderer implemented in C++ that supports both rasterization and ray tracing pipelines.

## Features

### Rendering Pipelines
- **Rasterization Pipeline**: Traditional graphics pipeline with vertex shaders, rasterization, and fragment shaders
- **Ray Tracing Pipeline**: Physics-based ray tracing with BVH acceleration structure

### Core Capabilities
- **Geometry Processing**: Support for loading 3D models (via Assimp library)
- **Shading**: 
  - Blinn-Phong shading model
  - Custom shader support (vertex/fragment shaders for rasterization, ray tracing shaders)
- **Lighting**: 
  - Point lights
  - Directional lights
  - Ambient lighting
- **Materials**:
  - Texture mapping (diffuse, height maps)
  - Reflective materials
  - Refractive materials with IOR (Index of Refraction)
  - Diffuse and glossy materials
- **Advanced Features**:
  - BVH (Bounding Volume Hierarchy) for ray tracing acceleration
  - Depth testing and Z-buffer
  - Backface culling
  - Perspective-correct interpolation
  - Bilinear texture filtering
  - Barycentric coordinate interpolation

### Mathematical Components
- Vector and matrix operations (2D, 3D, 4D)
- Transformation matrices (rotation, scaling, translation)
- Ray-geometry intersection tests
- Color space operations

## Dependencies

- **CMake** (version 3.12 or higher)
- **C++20** compatible compiler
- **OpenCV**: For image I/O and display
- **Assimp**: For 3D model loading (included as git submodule)

## Building the Project

### 1. Clone the repository with submodules
```bash
git clone --recursive https://github.com/annajcy/soft_renderer.git
cd soft_renderer
```

If you already cloned without `--recursive`, initialize submodules:
```bash
git submodule update --init --recursive
```

### 2. Install dependencies

#### Ubuntu/Debian
```bash
sudo apt-get install cmake g++ libopencv-dev
```

#### macOS
```bash
brew install cmake opencv
```

#### Windows
Install CMake and OpenCV following their official documentation.

### 3. Build
```bash
mkdir build
cd build
cmake ..
make
```

## Usage

### Running the Main Application
```bash
./soft_renderer
```

The main application (defined in `main.cpp`) demonstrates:
- Loading a 3D model (Spot the cow model)
- Setting up camera and lighting
- Rendering with ray tracing using Blinn-Phong shading

### Configuration
You can modify `main.cpp` to customize:
- Window dimensions (`width`, `height`)
- Camera parameters (FOV, position, orientation)
- Model paths and textures
- Lighting setup
- Ray tracing parameters (max depth, BVH settings)

### Example Code
```cpp
// Initialize renderer
auto raytracer = Raytracer();
raytracer.init(width, height);

// Setup scene
auto scene = std::make_shared<Scene>();
scene->set_camera(camera);
scene->set_light(lighting);
scene->add_model(model);

// Render
raytracer.render_scene(scene);
```

## Project Structure

```
soft_renderer/
├── application/        # Application window and image handling
├── assets/            # 3D models, textures, and images
│   ├── img/          # Test images
│   └── obj/          # 3D model files (.obj)
├── base/              # Base types and utilities
├── curve/             # Curve rendering utilities
├── ext/               # External dependencies (Assimp)
├── framework/         # Core framework (singleton, event system)
├── gpu/               # GPU abstraction layer (color buffer, depth buffer)
├── math/              # Mathematical utilities
│   ├── vec.h         # Vector operations
│   ├── mat.h         # Matrix operations
│   ├── transform.h   # Transformation matrices
│   ├── ray.h         # Ray definitions
│   └── color.h       # Color operations
├── mesh/              # Mesh and model loading
├── raster/            # Rasterization pipeline
│   ├── rasterizer.h  # Main rasterizer
│   └── raster_shader.h # Raster shaders
├── raytrace/          # Ray tracing pipeline
│   ├── raytracer.h   # Main ray tracer
│   └── raytrace_shader.h # Ray tracing shaders
├── rendering/         # Rendering components
│   ├── camera.h      # Camera implementation
│   ├── light.h       # Light sources
│   ├── material.h    # Material properties
│   ├── primitive.h   # Geometric primitives
│   ├── bvh.h         # BVH acceleration structure
│   └── scene.h       # Scene management
├── test/              # Test applications and examples
│   ├── examples/     # Example programs
│   ├── rasterization/ # Rasterization tests
│   └── raytrace/     # Ray tracing tests
├── utils/             # Utility functions
└── main.cpp          # Main application entry point
```

## Examples and Tests

The `test/` directory contains various example programs demonstrating different features:

### Rasterization Examples
- **Line Drawing**: Bresenham algorithm, anti-aliasing
- **Triangle Rasterization**: Colored and textured triangles
- **Model Rendering**: Full 3D model rendering

### Ray Tracing Examples
- Basic ray tracing setup
- Material and lighting tests

### Math Tests
- Vector and matrix operations
- Transformation tests

To build and run tests, you'll need to modify the CMakeLists.txt to include the specific test files you want to compile.

## Technical Details

### Rasterization Pipeline
1. **Vertex Shader**: Transforms vertices from model space to clip space
2. **Primitive Assembly**: Assembles vertices into triangles
3. **Clipping**: Clips primitives to view frustum
4. **Rasterization**: Converts primitives to fragments
5. **Fragment Shader**: Computes final pixel colors
6. **Depth Testing**: Z-buffer for hidden surface removal

### Ray Tracing Pipeline
1. **Ray Generation**: Generate rays from camera through each pixel
2. **BVH Traversal**: Efficiently find ray-primitive intersections
3. **Hit Testing**: Compute intersection points and normals
4. **Shading**: Calculate color based on material and lighting
5. **Recursive Rays**: Handle reflections and refractions

### BVH Acceleration
The BVH (Bounding Volume Hierarchy) structure accelerates ray tracing by organizing primitives into a tree structure with bounding boxes, reducing the number of intersection tests needed.

## Performance Considerations

- This is a **CPU-based** renderer, so performance will be limited compared to GPU renderers
- BVH parameters can be tuned for better performance:
  - `BVH_node::max_depth`: Maximum tree depth
  - `BVH_node::max_primitives_count`: Minimum primitives per leaf node
- Ray tracing depth (`max_depth`) affects quality vs. performance trade-off

## License

[Add your license information here]

## Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues for bugs and feature requests.

## Acknowledgments

- Uses [Assimp](https://github.com/assimp/assimp) for 3D model loading
- Uses [OpenCV](https://opencv.org/) for image processing
- Test models include the classic "Spot" cow model