#include <string>
#include "gtest/gtest.h"
#include "blaze/BlazeFS/BlazeFS.hpp"


TEST(readTest, readTest) {

    blaze::BlazeFS filesystem;
    filesystem.write("/test_file", std::string("some data"));

    ASSERT_EQ("some data", filesystem.read<std::string>("/test_file"));
    ASSERT_EQ("some data", filesystem.read<std::string>("test_file"));
    ASSERT_EQ("some data", filesystem.read<std::string>("./test_file"));
    ASSERT_EQ("some data", filesystem.read<std::string>("../test_file"));
    ASSERT_EQ("", filesystem.read<std::string>(""));
    ASSERT_EQ("", filesystem.read<std::string>("/non_existing_file"));
}
