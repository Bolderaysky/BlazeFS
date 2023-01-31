#include <string>
#include "gtest/gtest.h"
#include "BlazeFS/BlazeFS.hpp"


TEST(isDirectoryTest, isDirectoryTest) {

    blazefs::BlazeFS filesystem;

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