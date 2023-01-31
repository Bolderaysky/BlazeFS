#include <string>
#include "gtest/gtest.h"
#include "BlazeFS/BlazeFS.hpp"


TEST(reserveTest, reserveTest) {

    blazefs::BlazeFS filesystem;

    filesystem.createDir("/dir/");

    ASSERT_EQ(true, filesystem.reserve("dir/", 1024));
    ASSERT_EQ(true, filesystem.reserve("./dir/", 1024));
    ASSERT_EQ(true, filesystem.reserve("../dir/", 1024));
    ASSERT_EQ(true, filesystem.reserve("/", 1024));

    ASSERT_EQ(false, filesystem.reserve("file", 1024));
    ASSERT_EQ(false, filesystem.reserve("dir1/", 1024));
}