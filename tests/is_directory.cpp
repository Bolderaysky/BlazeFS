#include <string>
#include "gtest/gtest.h"
#include "blaze/BlazeFS/BlazeFS.hpp"


TEST(isDirectoryTest, isDirectoryTest) {

    blaze::BlazeFS filesystem;

    filesystem.createDir("/dir/");
    filesystem.write("file", std::string("some data"));

    ASSERT_EQ(true, filesystem.isDirectory("dir/"));
    ASSERT_EQ(true, filesystem.isDirectory("./dir/"));
    ASSERT_EQ(true, filesystem.isDirectory("../dir/"));
    ASSERT_EQ(true, filesystem.isDirectory("dir/./"));

    ASSERT_EQ(false, filesystem.isDirectory("file"));
    ASSERT_EQ(false, filesystem.isDirectory("./file"));
    ASSERT_EQ(false, filesystem.isDirectory("../file"));
    ASSERT_EQ(false, filesystem.isDirectory("dir/../file"));
}