#include <gtest/gtest.h>

#include <iostream>

#include "Common/Network/Socket.hpp"
#include "Common/Network/Acceptor.hpp"

#define TEST_IP_TCP (127 << 24 | 1)
#define TEST_UNIX_ADDR "/tmp/UT.socket"

using SocketTypeList = testing::Types<net::UnixStream, net::INetTcp>;

class SocketTypeName {
    public:
        template <IsStreamSocketType SocketType>
        static std::string GetName(int) {
            if constexpr (std::is_same_v<SocketType, net::UnixStream>) return "Unix Stream";
            if constexpr (std::is_same_v<SocketType, net::INetTcp>) return "Stream TCP";
        }
};

template<IsStreamSocketType SocketType>
class SocketConnectClose : public testing::Test
{
    protected:
        void SetUp() override
        {
            if constexpr (SocketType::Domain == AF_INET) {
                if (!acceptor.listen(0))
                    FAIL() << "Failed to bind and listen inet: " << strerror(errno);
            } else if constexpr (SocketType::Domain == AF_UNIX) {
                if (!acceptor.listen(TEST_UNIX_ADDR))
                    FAIL() << "Failed to bind and listen unix: " << strerror(errno);
            } else {
                static_assert(false, "Not supported domain type");
            }

            port = acceptor.getPort();
        }

        net::Acceptor<SocketType> acceptor;
        uint32_t port = 0;
};

TYPED_TEST_SUITE(SocketConnectClose, SocketTypeList, SocketTypeName);

TYPED_TEST(SocketConnectClose, close_server_side) {
    TypeParam socket;


    if constexpr (TypeParam::Domain == AF_INET) {
        ASSERT_TRUE(socket.connect(TEST_IP_TCP, this->port));
    } else if constexpr (TypeParam::Domain == AF_UNIX) {
        ASSERT_TRUE(socket.connect(TEST_UNIX_ADDR));
    } else {
        static_assert(false, "Not supported domain type");
    }

    typename net::Acceptor<TypeParam>::Client client = this->acceptor.accept();

    ASSERT_NE(client, nullptr);

    EXPECT_TRUE(socket.isOpen());
    EXPECT_TRUE(client->isOpen());

    ASSERT_TRUE(client->close());

    ASSERT_FALSE(client->isOpen());
}

TYPED_TEST(SocketConnectClose, close_client_side) {
    TypeParam socket;

    if constexpr (TypeParam::Domain == AF_INET) {
        ASSERT_TRUE(socket.connect(TEST_IP_TCP, this->port));
    } else if constexpr (TypeParam::Domain == AF_UNIX) {
        ASSERT_TRUE(socket.connect(TEST_UNIX_ADDR));
    } else {
        static_assert(false, "Not supported domain type");
    }

    typename net::Acceptor<TypeParam>::Client client = this->acceptor.accept();

    ASSERT_NE(client, nullptr);

    EXPECT_TRUE(socket.isOpen());
    EXPECT_TRUE(client->isOpen());

    ASSERT_TRUE(socket.close());

    ASSERT_FALSE(socket.isOpen());
}


template<IsStreamSocketType SocketType>
class SocketSendRecv : public testing::Test
{
    protected:
        void SetUp() override
        {
            if constexpr (SocketType::Domain == AF_INET) {
                if (!acceptor.listen(0))
                    FAIL() << "Failed to bind and listen inet: " << strerror(errno);
            } else if constexpr (SocketType::Domain == AF_UNIX) {
                if (!acceptor.listen(TEST_UNIX_ADDR))
                    FAIL() << "Failed to bind and listen unix: " << strerror(errno);
            } else {
                static_assert(false, "Not supported domain type");
            }

            if constexpr (SocketType::Domain == AF_INET) {
                if (!socket.connect(TEST_IP_TCP, acceptor.getPort()))
                    FAIL() << "Failed to connect to the endpoint inet: " << strerror(errno);
            } else if constexpr (SocketType::Domain == AF_UNIX) {
                if (!socket.connect(TEST_UNIX_ADDR))
                    FAIL() << "Failed to connect to the endpoint unix: " << strerror(errno);
            } else {
                static_assert(false, "Not supported domain type");
            }
            client = acceptor.accept();
        }

        SocketType socket;
        net::Acceptor<SocketType> acceptor;
        net::Acceptor<SocketType>::Client client;
};

TYPED_TEST_SUITE(SocketSendRecv, SocketTypeList, SocketTypeName);

TYPED_TEST(SocketSendRecv, single_send_single_recv)
{
    const std::string send1 = "this is a test";

    ASSERT_EQ(this->socket.send(send1), send1.size());

    int error = 0;
    std::vector<std::byte> bytes = this->client->receive(send1.size(), error);
    std::string recv(reinterpret_cast<const char *>(bytes.data()), bytes.size());

    ASSERT_EQ(error, send1.size());
    ASSERT_EQ(recv.size(), send1.size());
    ASSERT_EQ(recv, send1);
}

TYPED_TEST(SocketSendRecv, multi_send_single_recv)
{
    const std::string send1 = "this is ";
    const std::string send2 = "a test";

    ASSERT_EQ(this->socket.send(send1), send1.size());
    ASSERT_EQ(this->socket.send(send2), send2.size());

    int error = 0;
    std::vector<std::byte> bytes = this->client->receive(send1.size() + send2.size(), error);
    std::string recv(reinterpret_cast<const char *>(bytes.data()), bytes.size());

    ASSERT_EQ(error, send1.size() + send2.size());
    ASSERT_EQ(recv.size(), send1.size() + send2.size());
    ASSERT_EQ(recv, send1 + send2);
}

TYPED_TEST(SocketSendRecv, single_send_multi_recv)
{
    const std::string send1 = "this is a test";
    // The sample data to send should have an odd length

    ASSERT_EQ(this->socket.send(send1), send1.size());

    int error = 0;
    std::vector<std::byte> bytes = this->client->receive(send1.size() / 2, error);
    std::string recv(reinterpret_cast<const char *>(bytes.data()), bytes.size());


    ASSERT_EQ(error, send1.size() / 2);
    ASSERT_EQ(recv.size(), send1.size() / 2);
    ASSERT_EQ(recv, send1.substr(0, send1.size() / 2));

    error = 0;
    bytes = this->client->receive(send1.size() / 2, error);
    recv.assign(reinterpret_cast<const char *>(bytes.data()), bytes.size());

    ASSERT_EQ(error, send1.size() / 2);
    ASSERT_EQ(recv.size(), send1.size() / 2);
    ASSERT_EQ(recv, send1.substr(send1.size() / 2, send1.size()));
}