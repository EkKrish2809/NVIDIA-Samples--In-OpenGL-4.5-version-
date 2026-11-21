rm ./a.out

FILES=(
    oceanFFT_kernel.cu
    OceanFFT.cpp
    ../../core/shader.cpp
    ../../core/buffer.cpp
)

nvcc "${FILES[@]}" -lX11 -lGL -lGLEW -lcufft -lSOIL

__NV_PRIME_RENDER_OFFLOAD=1 __GLX_VENDOR_LIBRARY_NAME=nvidia ./a.out