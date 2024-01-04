# Project Nyx
OpenCL laboratory works

## Usage
```
Usage: nyx [OPTION]

Options:
  -g, --gpu                       Perform gpu tests (on gpu via OpenCL)
  -c, --cpu                       Perform cpu tests (on cpu)
  -v, --vector-size <size>        Vector of elements size (default: 102400000)
  -i, --iteration-count <count>   Count of iterations (default: 100)
  -t, --task-number <number>      Task number
                                  --task-number must be: 1, 2, 3, 4, 5, 6, 7, or 8 where:
                                      1 - draw some buffer via OpenCL buffer (deprecated)
                                      2 - draw Mandelbrot set via OpenCL buffer
                                      3 - draw OpenGL RGB triangle
                                      4 - draw OpenGL VAO triangle
                                      5 - draw OpenGL VAO triangle with matrices support
                                      6 - draw Mandelbrot set via OpenGL with interoperability
                                      7 - draw OpenCL particles with interoperability
                                      8 - draw OpenGL RGB cube
                                      9 - draw OpenGL RGB textured cube
  -b, --verbose                   Verbose output
  -h, --help                      Display help information and exit
  -u, --build-info                Display build information end exit
```

## Additional readme files
* Tasks and laboratory works you can see here: [TASKS.md](TASKS.md)
* Example application output you can see here: [OUTPUT.md](OUTPUT.md)

## Compile and install
Check [INSTALL.md](INSTALL.md).

## Additional information
* Author of this project: Savelii Pototskii ([savalione.com](https://savalione.com))
* Link to Github: https://github.com/SavaLione/urfu-cl-lab
* Date of creation: 2022-09-13
