#include <chrono>

#include <gtest/gtest.h>

#include <errno.h>

#include "Common/Network/Selector.hpp"
#include "Common/Log/Manager.hpp"

#define TEST_TO_SELECTOR 1000
#define TEST_IP_TCP "127.0.0.1"
#define TEST_UNIX_ADDR "/tmp/UT.socket"

using SocketTypeList = testing::Types<net::UnixTcp, net::StreamTcp>;

class SocketTypeName {
    public:
        template <IsSocketDomain SocketType>
        static std::string GetName(int) {
            if constexpr (std::is_same_v<SocketType, net::UnixTcp>) return "Unix TCP";
            if constexpr (std::is_same_v<SocketType, net::StreamTcp>) return "Stream TCP";
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


        void TearDown() override
        {
            if constexpr (SocketType::Domain == AF_UNIX)
                acceptor.unlink();
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

class Selector_multi : public testing::Test
{
    protected:
        void SetUp() override
        {
            acceptor.listen(0);
            selector.timeout((float)TEST_TO_SELECTOR);
            socket1.connect(TEST_IP_TCP, acceptor.getPort());
            socket2.connect(TEST_IP_TCP, acceptor.getPort());
            client1 = acceptor.accept();
            client2 = acceptor.accept();
        }

        net::StreamTcp socket1;
        net::StreamTcp socket2;
        net::Acceptor<net::StreamTcp>::Client client1;
        net::Acceptor<net::StreamTcp>::Client client2;
        net::Selector<net::StreamTcp> selector;
        net::Acceptor<net::StreamTcp> acceptor;
};

TEST_F(Selector_multi, erase_random)
{
    ASSERT_TRUE(selector.client(client1));
}

TEST_F(Selector_multi, add_client_diff)
{
    ASSERT_TRUE(selector.client(client1));
    ASSERT_TRUE(selector.client(client2));
    ASSERT_EQ(selector.size(), 2);
}

TEST_F(Selector_multi, pull_multiple)
{
    const std::string msg = "this is a test";

    ASSERT_TRUE(selector.client(client1));
    ASSERT_TRUE(selector.client(client2));
    ASSERT_EQ(socket2.send(msg), msg.size());
    ASSERT_EQ(socket1.send(msg), msg.size());

    std::vector<net::Selector<net::StreamTcp>::Client> clients = selector.pull();
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