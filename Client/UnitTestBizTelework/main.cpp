#include "pch.h"
#include <gtest/gtest.h>

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
    {
        fprintf(stderr, "AfxWinInit(...) failed.");
        return 1;
    }

    return RUN_ALL_TESTS();
}
