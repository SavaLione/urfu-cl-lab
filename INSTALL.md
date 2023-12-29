# Install


```
cmake .. -GNinja -D CMAKE_INSTALL_PREFIX=opencl-sdk -D CMAKE_BUILD_TYPE=Release -D OPENCL_SDK_BUILD_UTILITY_LIBRARIES=OFF -D OPENCL_SDK_BUILD_SAMPLES=OFF
cmake .. -GNinja -D CMAKE_INSTALL_PREFIX=spdlog -D CMAKE_BUILD_TYPE=Release
cmake .. -GNinja -D CMAKE_INSTALL_PREFIX=fmt -D CMAKE_BUILD_TYPE=Release
//cmake .. -GNinja -D CMAKE_INSTALL_PREFIX=glew -D CMAKE_BUILD_TYPE=Release
cmake .. -GNinja -D CMAKE_INSTALL_PREFIX=boost -D CMAKE_BUILD_TYPE=Release
```

ninja install
copy to bin

* download or install GLEW
    * https://github.com/nigels-com/glew/releases