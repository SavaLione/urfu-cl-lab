# Project Nyx
OpenCL laboratory works

## Usage
```sh
Usage: nyx [OPTION]

Options:
  -g, --gpu-only                  Perform only gpu tests
  -c, --cpu-only                  Perform only cpu tests
  -v, --vector-size <size>        Vector of elements size (default: 102400000)
  -i, --iteration-count <count>   Count of iterations (default: 100)
  -l, --laboratory-work <number>  Laboratory work number (default: 1)
  -b, --verbose                   Verbose output
  -h, --help                      Display this help and exit
```

## Additional readme files
* Tasks and laboratory works you can see here: [TASKS.md](TASKS.md)
* Example application output you can see here: [OUTPUT.md](OUTPUT.md)

## Compile and install
```sh
$ git clone https://github.com/SavaLione/urfu-cl-lab
$ cd urfu-cl-lab
$ mkdir build
$ cd build
$ cmake ..
$ make
```

## Additional information
* Author of this project: Saveliy Pototskiy ([savalione.com](https://savalione.com))
* Link to Github: https://github.com/SavaLione/urfu-cl-lab
* Date of creation: 2022-09-13