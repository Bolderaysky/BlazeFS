#include <string>
#include "gtest/gtest.h"
#include "BlazeFS/BlazeFS.hpp"


TEST(cdTest, cdTest) {

    blazefs::BlazeFS filesystem;
    filesystem.createDir("dir/");
    filesystem.write("/test_file", std::string("some data"));
    filesystem.write("/dir/test_file", std::string("another data"));

    ASSERT_EQ(true, filesystem.cd("/dir/"));
    ASSERT_EQ(true, filesystem.cd("./"));
    ASSERT_EQ(true, filesystem.cd("../"));
    ASSERT_EQ(true, filesystem.cd("dir/"));
    ASSERT_EQ(true, filesystem.cd("/"));
    ASSERT_EQ(true, filesystem.cd("dir/"));

    ASSERT_EQ("another data", filesystem.read<std::string>("test_file"));

    ASSERT_EQ(true, filesystem.cd("/"));
    ASSERT_EQ("some data", filesystem.read<std::string>("test_file"));

    ASSERT_EQ(false, filesystem.cd("/non_existing_dir/"));
    ASSERT_EQ(false, filesystem.cd("/non_existing_dir"));
}