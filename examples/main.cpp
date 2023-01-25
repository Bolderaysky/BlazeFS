#include <iostream>
#include <string>
#include "vfs.hpp"

int main(int argc, char *argv[]) {

    // Creating filesystem object
    vfs filesystem;

    // String that we will write to the file
    std::string myStr = "Hello World from Virtual filesystem!";

    // Writing into `/file`. / is considered as root node like in linux/unix
    // filesystems. Filesystem also accepts usage of not absolute path
    //
    // filesystem.write("./file", myStr);
    //
    // filesystem.write("file", myStr);
    filesystem.write("/file", myStr);

    // By default, current directory is root directory, so there's no need to
    // specify root node. Also, we can use something like './'
    //
    // std::cout << filesystem.read<std::string>("./file");

    std::cout << filesystem.read<std::string>("file") << std::endl;


    return 0;
}
