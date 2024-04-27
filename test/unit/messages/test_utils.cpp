#include <gtest/gtest.h>

#include "Common/Core/Utils.hpp"
#include "Common/Message/Fix.hpp"

class ChecksumTest : public ::testing::Test
{
    protected:
        void SetUp() override
        {
            message = std::string("this is a t3st!!") + (char)FIX_DELIMITER;
        }

        std::string message;
};

TEST_F(ChecksumTest, TestChecksum)
{
    EXPECT_EQ(fix::Message::getChecksum(message), "038");
}

class BodyLengthTest : public ::testing::Test
{
    protected:
        void SetUp() override
        {
            message = "hello i am 29 characters long";
        }

        std::string message;
};

TEST_F(BodyLengthTest, TestBodyLength)
{
    EXPECT_EQ(fix::Message::getBodyLength(message), 29);
}