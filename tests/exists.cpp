#include <string>
#include "gtest/gtest.h"
#include "blaze/BlazeFS/BlazeFS.hpp"


TEST(existsTest, existsTest) {

    blaze::BlazeFS filesystem;
    filesystem.createDir("dir/");
    filesystem.write("/test_file", std::string("some data"));

    ASSERT_EQ(true, filesystem.exists("/test_file"));
    ASSERT_EQ(true, filesystem.exists("test_file"));
    ASSERT_EQ(true, filesystem.exists("./test_file"));
    ASSERT_EQ(true, filesystem.exists("../test_file"));
    ASSERT_EQ(true, filesystem.exists("dir/"));

    ASSERT_EQ(false, filesystem.exists("non_existing_dir/"));
    ASSERT_EQ(false, filesystem.exists(""));
    ASSERT_EQ(false, filesystem.exists("/non_existing_file"));
}