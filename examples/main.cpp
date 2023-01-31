#include <iostream>
#include "BlazeFS/BlazeFS.hpp"

int main() {

    // Creating filesystem object
    blazefs::BlazeFS filesystem;

    // String that we will write to the file
    std::string myStr = "Hello World from Virtual filesystem!";

    // Writing into `/file`. / is considered as root node like in linux/unix
    // filesystems. Filesystem also accepts usage of non absolute path
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
