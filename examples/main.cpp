#include <iomanip>
#include <iostream>
#include <unordered_map>
#include "blaze/BlazeFS/BlazeFS.hpp"

/*
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
}*/

void func() {

    std::cout << "";
}

int main(int, char **) {

    blaze::BlazeFS filesystem;

    /*filesystem.readArchive("app.asar");

    auto dirListing = filesystem.listDir("/");

    // std::cout << dirListing->size() << std::endl;

    for (const auto &entry : *dirListing) std::cout << entry << '\n';

    auto subDirListing = filesystem.listDir("assets/");

    std::cout << "\n------------------\n";

    for (const auto &entry : *subDirListing) std::cout << entry << '\n';*/

    clock_t before, after;

    // std::vector<std::string> iStr;

    // before = clock();
    // for (std::uint64_t i = 0; i < 10'000'000; ++i)
    //     iStr.emplace_back(std::to_string(i));
    // after = clock();

    // std::cout << std::fixed << std::setprecision(6)
    //           << double(after - before) / CLOCKS_PER_SEC
    //           << "s for vector write\n";


    filesystem.reserve("/", 10'000'000);

    before = clock();
    for (std::uint64_t i = 0; i < 10'000'000; ++i)
        // filesystem.write<std::uint64_t, true>(iStr[i], i);
        filesystem.write<std::uint64_t, true>(std::to_string(i), i);
    after = clock();

    std::cout << std::fixed << std::setprecision(6)
              << double(after - before) / CLOCKS_PER_SEC << "s for vfs write\n";

    before = clock();
    for (std::uint64_t i = 0; i < 10'000'000; ++i)
        // filesystem.read<std::uint64_t, true>(iStr[i]);
        filesystem.read<std::uint64_t, true>(std::to_string(i));
    // func();
    after = clock();

    std::cout << std::fixed << std::setprecision(6)
              << double(after - before) / CLOCKS_PER_SEC << "s for vfs read\n";


    // std::map<std::string, std::uint64_t> map1;

    // before = clock();
    // for (std::uint64_t i = 0; i < 10'000'000; ++i)
    //     map1.emplace(std::to_string(i), i);
    // after = clock();

    // std::cout << std::fixed << std::setprecision(6)
    //           << double(after - before) / CLOCKS_PER_SEC
    //           << "s for map without custom allocator write\n";

    return 0;
}
