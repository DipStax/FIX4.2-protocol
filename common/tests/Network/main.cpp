#include <gtest/gtest.h>

#include "Common/Log/Manager.hpp"
#include "Common/Log/Imp/Console.hpp"

int main(int argc, char **argv) {
    logger::Manager::registerDefaultLogger<logger::imp::Console>();

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}