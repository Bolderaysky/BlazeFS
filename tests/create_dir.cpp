#include <string>
#include "gtest/gtest.h"
#include "blaze/BlazeFS/BlazeFS.hpp"


TEST(createDirTest, createDirTest) {

    blaze::BlazeFS filesystem;

    ASSERT_EQ(true, filesystem.createDir("/dir/"));
    ASSERT_EQ(true, filesystem.createDir("dir1/"));
    ASSERT_EQ(true, filesystem.createDir("./dir2/"));
    ASSERT_EQ(true, filesystem.createDir("../dir3/"));

    ASSERT_EQ(false, filesystem.createDir("/"));
    ASSERT_EQ(false, filesystem.createDir("dir/"));
}