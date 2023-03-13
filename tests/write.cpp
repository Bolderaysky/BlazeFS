#include <string>
#include "gtest/gtest.h"
#include "blaze/BlazeFS/BlazeFS.hpp"


TEST(writeTest, writeTest) {

    blaze::BlazeFS filesystem;
    ASSERT_EQ(true, filesystem.write("/test_file", std::string("some data")));
    ASSERT_EQ(true, filesystem.write("./test_file1", std::string("some data")));
    ASSERT_EQ(true,
              filesystem.write("../test_file2", std::string("some data")));
    ASSERT_EQ(true, filesystem.write("test_file3", std::string("some data")));
    ASSERT_EQ(false, filesystem.write("", std::string("some data")));
    ASSERT_EQ(false, filesystem.write("/non_existing_dir/file",
                                      std::string("some data")));
}