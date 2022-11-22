- Changed README.md (usage)
- Fixed files indentation (tabs replaced with spaces)
- Added the Mandelbrot set example (``core/cl_mandelbrot.h``, ``core/cl_mandelbrot.cpp``)
    - Author of kernel: Willem Melching
    - https://blog.willemmelching.nl/random/2020/04/12/mandelbrot/

2.0.0 - 2022-11-22
------------------

- Fixed potential ``std::stoi`` bug
- Added an argument to select first or second laboratory work ``-l <number>`` or ``--laboratory-work <number>``
- Added changelog
- Changed application help information
- Changed indent type (now with spaces instead of tabs)
- Changed ColumnLimit (was 130, now 180)
- Changed ``kernel_loaded``. Now ``kernel_loader`` based on ``SDL_RWops``
- Added verbose output argument ``-b`` or ``--verbose``
- Added ``exit`` setting
- Added SDL2 support
- Added platform specific settings (``core/platform.h``, ``core/platform.cpp``)
- Added graphics (SDL2 interaction) wrapper (``core/graphics.h``, ``core/graphics.cpp``)
- Disabled clangd (``.clang-format``) auto ``#include``
- Changed CMake configuration
- Added buffer representation (``core/buffer_representation.h``, ``core/buffer_representation.cpp``)
- Added image representation (``core/image_representation.h``, ``core/image_representation.cpp``)
- Added new gpu interaction (``core/new_gpu.h``, ``core/new_gpu.cpp``)
- Released new major version (1.1.0 -> 2.0.0)
- Changed project structure
- Changed OpenMP connection in CMake
- Changed CMake linking (linking is now static by default )