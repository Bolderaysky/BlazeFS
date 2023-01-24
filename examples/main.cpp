#include <iostream>
#include "vfs.hpp"

int main(int argc, char* argv[]) {

    vfs filesystem;

    filesystem.createDir("directory/");
    filesystem.createDir("dir2/");
    filesystem.writeFile("/testfile1", std::string("Hello World"));
    filesystem.writeFile("/dir2/testfile2", std::string("Hello World"));

    filesystem.cd("directory/");

    std::string val = filesystem.readFile<std::string>("../testfile1");
    std::cout << val << '\n';

    filesystem.remove("/dir2/testfile2");

    std::cout << filesystem.exists("/dir2/testfile2") << '\n';

    auto dirList = filesystem.listDir("/dir2/../directory/");

    for (const auto& entry : *dirList) std::cout << entry << '\n';

    return 0;
}
