#include <gtest/gtest.h>
#include "Common/Core/Utils.hpp"
#include "Common/Message/Logon.hpp"

class TestLogon : public ::testing::Test
{
    protected:
        void SetUp()
        {
            fix::Logon logon{};

            logon.header.set49_SenderCompId("CLIENT");
            logon.header.set56_TargetCompId("TARGET");
            logon.set98_EncryptMethod("0");
            logon.set108_HeartBtInt("30");
            logon.header.set34_msgSeqNum("1");
            formatedLogon = logon.to_string();
            sendingTime = formatedLogon.substr(formatedLogon.find("52=") + 3, 21);
        }

        std::string sendingTime;
        std::string formatedLogon;
};

TEST_F(TestLogon, TestLogonToString)
{
    std::string expected = "8=FIX.4.2" + std::string(1, FIX_DELIMITER) +
                           "9=67" + std::string(1, FIX_DELIMITER) +
                           "35=A" + std::string(1, FIX_DELIMITER) +
                           "49=CLIENT" + std::string(1, FIX_DELIMITER) +
                           "56=TARGET" + std::string(1, FIX_DELIMITER) +
                           "34=1" + std::string(1, FIX_DELIMITER) +
                           "52=" + sendingTime + std::string(1, FIX_DELIMITER) +
                           "98=0" + std::string(1, FIX_DELIMITER) +
                           "108=30" + std::string(1, FIX_DELIMITER);

    expected += "10=" + fix::Message::getChecksum(expected) + std::string(1, FIX_DELIMITER);
    EXPECT_EQ(formatedLogon, expected);
}