#include <string>
#include "gtest/gtest.h"
#include "blaze/BlazeFS/BlazeFS.hpp"


TEST(createSymlinkTest, createSymlinkTest) {

    blaze::BlazeFS filesystem;

    filesystem.createDir("dir/");
    filesystem.write("/dir/file", std::string("some data"));

    ASSERT_EQ(true, filesystem.createSymlink("/symlink_dir/", "dir/"));
    ASSERT_EQ("some data", filesystem.read<std::string>("symlink_dir/file"));
}