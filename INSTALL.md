# Install
It works on Windows (via MinGW-w64), it works on Unix.
Check ``CMakeLists.txt`` in order to see all required packages.

## Unix
1. Install compiler.
2. Install all dependencies.
3. Clone repository: ``git clone https://github.com/SavaLione/urfu-cl-lab``
4. Open created directory: ``cd urfu-cl-lab``
5. Create build directory: ``mkdir build``
6. Open build directory: ``cd build``
7. Configure CMake:
    * ``cmake .. -GNinja``
    * OR
    * ``cmake ..``
8. Build project
    * ``ninja -j``
    * OR
    * ``make -j``
9. Run application: ``./bin/nyx --help``

## Windows
1. Install compiler
    * Almost any modern compiler would suffice, though I prefer MinGW-w64
    * I really recommend you to try compiler and tools (including boost library) compiled by Stephan T. Lavavej: https://nuwen.net/mingw.html
2. Install all dependencies
    * OpenCL
    * OpenGL
    * spdlog (https://github.com/gabime/spdlog)
    * GLEW (https://github.com/nigels-com/glew)
    * Boost (you don't need to manually compile this if you use kit by Stephan T. Lavavej) (https://www.boost.org/)
    * glm (you don't need to manually compile this if you use kit by Stephan T. Lavavej)
    * SDL2 (you don't need to manually compile this if you use kit by Stephan T. Lavavej)
3. Clone repository: ``git clone https://github.com/SavaLione/urfu-cl-lab``
4. Open created directory: ``cd urfu-cl-lab``
5. Create build directory: ``mkdir build``
6. Open build directory: ``cd build``
7. Configure CMake:
    * ``cmake .. -GNinja``
    * OR
    * ``cmake ..``
8. Build project
    * ``ninja -j``
    * OR
    * ``nmake``
9. Run application: ``./bin/nyx.exe --help``


<!-- ```
cmake .. -GNinja -D CMAKE_INSTALL_PREFIX=opencl-sdk -D CMAKE_BUILD_TYPE=Release -D OPENCL_SDK_BUILD_UTILITY_LIBRARIES=OFF -D OPENCL_SDK_BUILD_SAMPLES=OFF
cmake .. -GNinja -D CMAKE_INSTALL_PREFIX=spdlog -D CMAKE_BUILD_TYPE=Release
cmake .. -GNinja -D CMAKE_INSTALL_PREFIX=fmt -D CMAKE_BUILD_TYPE=Release
//cmake .. -GNinja -D CMAKE_INSTALL_PREFIX=glew -D CMAKE_BUILD_TYPE=Release
cmake .. -GNinja -D CMAKE_INSTALL_PREFIX=boost -D CMAKE_BUILD_TYPE=Release
```

ninja install
copy to bin

* download or install GLEW
    * https://github.com/nigels-com/glew/releases -->