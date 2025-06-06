#include <chrono>

#include <gtest/gtest.h>

#include <errno.h>

#include "Common/Network/Selector.hpp"
#include "Common/Log/Manager.hpp"

#define TEST_TO_SELECTOR 1000
#define TEST_IP_TCP (127 << 24 | 1)
#define TEST_UNIX_ADDR "/tmp/UT.socket"

using SocketTypeList = testing::Types<net::UnixStream, net::INetTcp>;

class SocketTypeName {
    public:
        template <IsSocketDomain SocketType>
        static std::string GetName(int) {
            if constexpr (std::is_same_v<SocketType, net::UnixStream>) return "Unix Stream";
            if constexpr (std::is_same_v<SocketType, net::INetTcp>) return "Stream TCP";
        }
};

template<IsSocketDomain SocketType>
class EmptySelector : public testing::Test
{
    protected:
        void SetUp() override
        {
            selector.timeout((float)TEST_TO_SELECTOR);
        }

        net::Selector<SocketType> selector;
};

TYPED_TEST_SUITE(EmptySelector, SocketTypeList, SocketTypeName);

TYPED_TEST(EmptySelector, timout_check)
{
    ASSERT_EQ(this->selector.timeout(), TEST_TO_SELECTOR);
}


TYPED_TEST(EmptySelector, no_client)
{
    ASSERT_EQ(this->selector.size(), 0);
}

TYPED_TEST(EmptySelector, null_client)
{
    ASSERT_FALSE(this->selector.client(nullptr));
}

template<IsSocketDomain SocketType>
class SingleClientSelector : public testing::Test
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

            selector.timeout((float)TEST_TO_SELECTOR);

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
            selector.client(client);
        }

        SocketType socket;
        net::Acceptor<SocketType>::Client client;
        net::Selector<SocketType> selector;
        net::Acceptor<SocketType> acceptor;
};

TYPED_TEST_SUITE(SingleClientSelector, SocketTypeList, SocketTypeName);

TYPED_TEST(SingleClientSelector, unique_client)
{
    ASSERT_EQ(this->selector.size(), 1);
}

TYPED_TEST(SingleClientSelector, pull_to)
{
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<typename net::Selector<TypeParam>::Client> clients = this->selector.pull();
    auto end = std::chrono::high_resolution_clock::now();

    auto elapsed = std::chrono::duration<double>(end - start);

    ASSERT_GT(elapsed.count(), 1.f);
    ASSERT_LT(elapsed.count(), 1.5);
    ASSERT_EQ(clients.size(), 0);
}

TYPED_TEST(SingleClientSelector, same_client)
{
    ASSERT_FALSE(this->selector.client(this->client));
    ASSERT_EQ(this->selector.size(), 1);
}

TYPED_TEST(SingleClientSelector, new_client)
{
    TypeParam socket2;

    if constexpr (TypeParam::Domain == AF_INET) {
        if (!socket2.connect(TEST_IP_TCP, this->acceptor.getPort()))
            FAIL() << "Failed to connect to the endpoint inet: " << strerror(errno);
    } else if constexpr (TypeParam::Domain == AF_UNIX) {
        if (!socket2.connect(TEST_UNIX_ADDR))
            FAIL() << "Failed to connect to the endpoint unix: " << strerror(errno);
    } else {
        static_assert(false, "Not supported domain type");
    }
    typename net::Selector<TypeParam>::Client client2 = this->acceptor.accept();

    ASSERT_TRUE(this->selector.client(client2));
    ASSERT_EQ(this->selector.size(), 2);
}

TYPED_TEST(SingleClientSelector, erase_client)
{
    this->selector.erase(this->client);
    ASSERT_EQ(this->selector.size(), 0);
}

TYPED_TEST(SingleClientSelector, erase_nullptr)
{
    this->selector.erase(nullptr);
    ASSERT_EQ(this->selector.size(), 1);
}

TYPED_TEST(SingleClientSelector, pull_client)
{
    const std::string msg = "this is a test";
    ASSERT_EQ(this->socket.send(msg), msg.size());

    std::vector<typename net::Selector<TypeParam>::Client> clients = this->selector.pull();
    ASSERT_EQ(clients.size(), 1);

    int error = 0;
    std::string str = clients.at(0)->receive(msg.size(), error);
    ASSERT_EQ(error, msg.size());
    EXPECT_EQ(str.size(), msg.size());
    ASSERT_EQ(str, msg);
}

template<IsSocketDomain SocketType>
class MultiClientSelector : public testing::Test
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

            selector.timeout((float)TEST_TO_SELECTOR);

            if constexpr (SocketType::Domain == AF_INET) {
                if (!socket1.connect(TEST_IP_TCP, this->acceptor.getPort()))
                    FAIL() << "Failed to connect to the endpoint inet (1): " << strerror(errno);
                if (!socket2.connect(TEST_IP_TCP, this->acceptor.getPort()))
                    FAIL() << "Failed to connect to the endpoint inet (2): " << strerror(errno);
            } else if constexpr (SocketType::Domain == AF_UNIX) {
                if (!socket1.connect(TEST_UNIX_ADDR))
                    FAIL() << "Failed to connect to the endpoint unix (1): " << strerror(errno);
                if (!socket2.connect(TEST_UNIX_ADDR))
                    FAIL() << "Failed to connect to the endpoint unix (2): " << strerror(errno);
            } else {
                static_assert(false, "Not supported domain type");
            }
            client1 = acceptor.accept();
            selector.client(client1);
            client2 = acceptor.accept();
            selector.client(client2);
        }

        SocketType socket1;
        SocketType socket2;
        net::Acceptor<SocketType>::Client client1;
        net::Acceptor<SocketType>::Client client2;
        net::Selector<SocketType> selector;
        net::Acceptor<SocketType> acceptor;
};

TYPED_TEST_SUITE(MultiClientSelector, SocketTypeList, SocketTypeName);

TYPED_TEST(MultiClientSelector, pull_multiple)
{
    const std::string msg = "this is a test";

    ASSERT_EQ(this->socket2.send(msg), msg.size());
    ASSERT_EQ(this->socket1.send(msg), msg.size());

    std::vector<typename net::Selector<TypeParam>::Client> clients = this->selector.pull();
    ASSERT_EQ(clients.size(), 2);

    int error = 0;
    std::string str = clients.at(0)->receive(msg.size(), error);
    ASSERT_EQ(error, msg.size());
    EXPECT_EQ(str.size(), msg.size());
    ASSERT_EQ(str, msg);

    error = 0;
    str = clients.at(1)->receive(msg.size(), error);
    ASSERT_EQ(error, msg.size());
    EXPECT_EQ(str.size(), msg.size());
    ASSERT_EQ(str, msg);
}