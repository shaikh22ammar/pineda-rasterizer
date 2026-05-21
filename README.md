Based on Fabian Giesen's blog https://fgiesen.wordpress.com/2013/02/10/optimizing-the-basic-rasterizer/

# Build instructions
SDL3 and pkg-config are required dependencies. Additionally, a CPU possessing ARM NEON capabilities is required for the `simd` branch. 
On macOS,
1. `brew install SDL3 pkg-config`
2. `make`
