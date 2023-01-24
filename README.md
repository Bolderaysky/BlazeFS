# vfs-asar
**vfs-asar** is virtual filesystem that allows you to read, write text and binary data, store C structures, C++ classes and in fact, store anything inside virtual filesystem.
## Features
- In-memory
- Can be loaded from asar
- Can store any types
- Fast
- Secure
- Behaves a lot like unix filesystems
- Cross-platform
## About cross-platform
This project is linux-first, but also working on Windows and MacOS. If you find some bugs, want to get new feature or something like this, open issue using template and provide your ideas and information.
## How it's working?
**vfs-asar** uses `std::unordered_map` to store tree. Directory and file names are treated as `std::string` key, and data as `std::any`, so you can store anything as long as it can be casted in and from `std::any`. This project can be used even for big filesystem, because it uses smart pointers, which mean that there will be no chance for buffer overflow.
## How to build and use
This project uses cmake as build system, so clone the repository and just do in the terminal:
```
mkdir build && cd build
cmake ..
make -j4
```
If you are on Windows, cmake will create a `.sln` project file instead of `makefile`, that you can open in Visual Studio, or build from commandline using
```
msbuild vfs.sln -m /property:Configuration=Release
```

For examples just look in the repository.

## License
MIT. Feel free to use anything in this repository.