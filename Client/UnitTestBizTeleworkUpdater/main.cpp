#include <gtest/gtest.h>

// Defined BIZ_TELEWORK_UPDATER_TESTING macro on project properties.

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
