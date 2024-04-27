#include "Common/Message/Header.hpp"
#include "Common/Message/Fix.hpp"

#include <gtest/gtest.h>

class HeaderTest : public ::testing::Test
{
    protected:
        void SetUp() override
        {
            m_header.set49_SenderCompId("CLIENT");
            m_header.set56_TargetCompId("TARGET");
            m_header.set9_bodyLength("123");
            m_header.set35_MsgType("A");
            m_header.set34_msgSeqNum("32");
        }

        fix::Header m_header{};
};

TEST_F(HeaderTest, TestHeaderToString)
{
    std::string expected = "8=FIX.4.2" + std::string(1, FIX_DELIMITER) +
                           "9=123" + std::string(1, FIX_DELIMITER) +
                           "35=A" + std::string(1, FIX_DELIMITER) +
                           "49=CLIENT" + std::string(1, FIX_DELIMITER) +
                            "56=TARGET" + std::string(1, FIX_DELIMITER) +
                            "34=32" + std::string(1, FIX_DELIMITER) +
                            "52=" + std::string(1, FIX_DELIMITER);

    EXPECT_EQ(static_cast<std::string>(m_header), expected);
}