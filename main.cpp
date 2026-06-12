#include <Windows.h>

#ifdef _DEBUG
#include <gtest/gtest.h>
#endif

int main(int argc, char** argv) {
#ifdef _DEBUG
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
#else
    SetConsoleOutputCP(65001);
    return 0;
#endif
}
