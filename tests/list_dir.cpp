#include <string>
#include <vector>
#include "gtest/gtest.h"
#include "blaze/BlazeFS/BlazeFS.hpp"


TEST(listDirTest, listDirTest) {

    std::vector<std::string> vec;
    vec.reserve(6);
    vec.emplace_back("./");
    vec.emplace_back("../");
    vec.emplace_back("file1");
    vec.emplace_back("file2");
    vec.emplace_back("dir1/");
    vec.emplace_back("dir2/");

    blaze::BlazeFS filesystem;

    filesystem.createDir("/dir/");
    filesystem.createDir("dir/dir1/");
    filesystem.createDir("dir/dir2/");
    filesystem.write("dir/file1", "some data");
    filesystem.write("dir/file2", "another data");

    auto res = filesystem.listDir("dir/");
    bool isSuccessfull = true;

    for (const auto &entry : *res) {

        auto it = find(vec.begin(), vec.end(), entry);
        if (it == vec.end()) {

            isSuccessfull = false;
            break;
        }
    }

    ASSERT_EQ(true, isSuccessfull);
}