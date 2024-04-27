#include "Common/Message/Serializer.hpp"
#include "Common/Message/Logon.hpp"
#include <gtest/gtest.h>

class SerializerTest : public ::testing::Test
{
    protected:
        void SetUp() override
        {
            fix::Logon logon{};

            logon.header.set49_SenderCompId("CLIENT");
            logon.header.set56_TargetCompId("TARGET");
            logon.set98_EncryptMethod("0");
            logon.set108_HeartBtInt("30");
            logon.header.set34_msgSeqNum("1");
            message = logon.to_string();
            message2 = logon.to_string();
            sendingTime = message.substr(message.find("52=") + 3, 21);
        }

        std::string message{};
        std::string message2{};
        std::string sendingTime{};
};

TEST_F(SerializerTest, TestSerialize)
{
    fix::Serializer::AnonMessage anonMessage{};
    fix::Serializer::Error err = fix::Serializer::run(message, anonMessage);

    ASSERT_EQ(err, fix::Serializer::Error::None);
    ASSERT_TRUE(anonMessage.contains("35"));
    EXPECT_EQ(anonMessage.at("35")[0], 'A');
    ASSERT_TRUE(anonMessage.contains("49"));
    EXPECT_EQ(anonMessage.at("49"), "CLIENT");
    ASSERT_TRUE(anonMessage.contains("56"));
    EXPECT_EQ(anonMessage.at("56"), "TARGET");
    ASSERT_TRUE(anonMessage.contains("98"));
    EXPECT_EQ(anonMessage.at("98"), "0");
    ASSERT_TRUE(anonMessage.contains("108"));
    EXPECT_EQ(anonMessage.at("108"), "30");
    ASSERT_TRUE(anonMessage.contains("34"));
    EXPECT_EQ(anonMessage.at("34"), "1");
    ASSERT_TRUE(anonMessage.contains("52"));
    EXPECT_EQ(anonMessage.at("52"), sendingTime);
}

TEST_F(SerializerTest, TestSerializeInvalidKey)
{
    fix::Serializer::AnonMessage anonMessage{};

    message[14] = (char)FIX_DELIMITER;
    message[30] = '=';

    fix::Serializer::Error err = fix::Serializer::run(message, anonMessage);

    ASSERT_EQ(err == fix::Serializer::Error::InvalidKey, true);
}

TEST_F(SerializerTest, TestSerializeInvalidEnd)
{
    fix::Serializer::AnonMessage anonMessage{};

    message = message.substr(0, message.size() - 10);

    fix::Serializer::Error err = fix::Serializer::run(message, anonMessage);

    ASSERT_EQ(err == fix::Serializer::Error::InvalidEnd, true);
}

TEST_F(SerializerTest, TestSerializeNoEqual)
{
    fix::Serializer::AnonMessage anonMessage{};
    message = "8=FIX.4.2" + std::string(1, FIX_DELIMITER) +
              "9-72" + std::string(1, FIX_DELIMITER);
    fix::Serializer::Error err = fix::Serializer::run(message, anonMessage);

    ASSERT_EQ(err == fix::Serializer::Error::NoEqual, true);
}

TEST_F(SerializerTest, TestLogonVerify)
{
    fix::Serializer::AnonMessage anonMessage{};
    fix::Serializer::Error err = fix::Serializer::run(message2, anonMessage);
    std::pair<bool, fix::Reject> reject = fix::Logon::Verify(anonMessage);

    EXPECT_FALSE(reject.first);
}