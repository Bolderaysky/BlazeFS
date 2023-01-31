#include <string>
#include "gtest/gtest.h"
#include "BlazeFS/BlazeFS.hpp"


TEST(removeTest, removeTest) {

    blazefs::BlazeFS filesystem;
    filesystem.createDir("dir/");
    filesystem.write("/test_file", std::string("some data"));

    ASSERT_EQ(true, filesystem.remove("/test_file"));
    ASSERT_EQ(true, filesystem.remove("dir/"));

    ASSERT_EQ(false, filesystem.remove(""));
    ASSERT_EQ(false, filesystem.remove("/non_existing_dir"));
    ASSERT_EQ(false, filesystem.remove("/non_existing_file"));
}