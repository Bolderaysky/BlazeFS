# BlazeFS
**BlazeFS** is virtual filesystem that allows you to read, write text and binary data, store C structures, C++ classes and in fact, store anything inside virtual filesystem.
## Features
- In-memory
- Can be loaded from asar
- Can store any types
- Fast
- Secure
- Behaves a lot like unix filesystems
- Cross-platform

## Performance
As I was benchmarking, vfs outperforms STL containers such as `std::map<std::string, int>` and `std::unordered_map<std::string, int>`. Benchmarks will be publicly available soon.

## Caveats
- This project uses Microsoft mimalloc and overrides default `new` and `delete` operator.
- File path may be only 4096 chars.
- File or folder name may be only 4096 chars.
- Folder name must end with `/` char.
- Every folder has `./` and `../` symlinks. Don't remove them.

## About cross-platform
This project is linux-first, but also working on Windows and MacOS. If you find some bugs, want to get new feature or something like this, open issue using template and provide your ideas and information.
## How it's working?
**BlazeFS** uses `tsl::bhopscotch_map` to store tree. Directory and file names are treated as `std::string` key, and data as `std::any`, so you can store anything as long as it can be casted in and from `std::any`. This project can be used even for big filesystem, because it uses smart pointers, which mean that there will be no chance for memory leaks etc.
## How to build
This project uses cmake as build system, so clone the repository and just do in the terminal:
```
mkdir build && cd build
cmake ..
make -j4
```
If you are on Windows, cmake will create a `.sln` project file instead of `makefile`, that you can open in Visual Studio, or build from commandline using
```
msbuild blazefs.sln -m /property:Configuration=Release
```

## Usage
You can initialize filesystem just using
```cpp
blazefs::BlazeFS filesystem;
```
### Writing
For example, we want to write file `test.txt` with content `Hello World`. We can do that in several ways:
1. Write in the current directory. By default, current directory is `/`
```cpp
filesystem.write("test.txt", "Hello World");
```
2. Write in the `/` directly:
```cpp
filesystem.write("/test.txt", "Hello World");
```
3. Write into that folder using `./`:
```cpp
filesystem.write("./test.txt", "Hello World");
```

### Directory creation

We can create directory by using function `createDir()`:
```cpp
filesystem.createDir("dir1/");
filesystem.createDir("/dir2/");
filesystem.createDir("./dir3/");
```
This will create 3 directories that will be placed in the `/`. If you changed dir using `cd` previously, then only `dir2/` will be placed in the `/`.

### Reading
This is the same as writing, but for reading we also have to provide what type is written into file. For example:
```cpp

struct myStruct {

    int a;
    std::string b;

};

// ...
myStruct myStructObj;

filesystem.write("file_with_string", std::string("something"));
filesystem.write("file_with_struct", myStructObj);

filesystem.read<std::string>("file_with_string"); // OK, we wrote std::string and are reading std string
filesystem.read<myStruct>("file_with_struct"); // OK, we wrote myStruct and are reading myStruct

filesystem.read<const char*>("file_with_string"); // std::bad_any_cast, trying to read const char* where we wrote std::string
```

### Reserve
If we want to write a lot of files into one folder, we can use `reserve` to pre-allocate memory and speed-up insertion:
```cpp

filesystem.createDir("myDir/"); // create dir where we want to insert a lot of files
filesystem.reserve("mydir/", 1024); // Reserving space for 1024 files

```

Also, when we are constructing filesystem, we can pre-allocate space in `/` like so:
```cpp
blazefs::BlazeFS filesystem(1024);
```

### Changing directory

You can change directory by using `cd` function, like so:

```cpp

filesystem.createDir("/myDir/");
filesystem.write("/myDir/file", "something");
filesystem.exists("file"); // Returns false, because we provide relative path, and current directory is /
filesystem.cd("/myDir/");
filesystem.exists("file"); // Returns true, as we've changed current directory

```

## Used libraries
- [hopschotch-map](https://github.com/Tessil/hopscotch-map)
- [mimalloc](https://github.com/microsoft/mimalloc)
- [rapidjson](https://rapidjson.org/) (used only for parsing asar)

## License
MIT. Feel free to use anything in this repository.