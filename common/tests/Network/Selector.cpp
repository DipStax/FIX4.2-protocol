#include <chrono>

#include <gtest/gtest.h>

#include <errno.h>

#include "Common/Network/Selector.hpp"
#include "Common/Log/Manager.hpp"

#define TEST_TO_SELECTOR 1000
#define TEST_IP_TCP "127.0.0.1"

class Selector_empty : public testing::Test
{
    protected:
        void SetUp() override
        {
            selector.timeout((float)TEST_TO_SELECTOR);
        }

        net::Selector<net::StreamTcp> selector;
};

TEST_F(Selector_empty, timout_check)
{
    ASSERT_EQ(selector.timeout(), TEST_TO_SELECTOR);
}


TEST_F(Selector_empty, no_client)
{
    ASSERT_EQ(selector.size(), 0);
}

TEST_F(Selector_empty, null_client)
{
    ASSERT_FALSE(selector.client(nullptr));
}

class Selector_single : public testing::Test
{
    protected:
        void SetUp() override
        {
            if (!acceptor.listen(8080))
                FAIL() << "Failed to bind and listen: " << strerror(errno);
            selector.timeout((float)TEST_TO_SELECTOR);
            if (!socket.connect(TEST_IP_TCP, acceptor.getPort()))
                FAIL() << "Failed to connect to the endpoint: " << strerror(errno);
            client = acceptor.accept();
            selector.client(client);
        }

        net::StreamTcp socket;
        net::Acceptor<net::StreamTcp>::Client client;
        net::Selector<net::StreamTcp> selector;
        net::Acceptor<net::StreamTcp> acceptor;
};

TEST_F(Selector_single, pull_to)
{
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<net::Selector<net::StreamTcp>::Client> clients = selector.pull();
    auto end = std::chrono::high_resolution_clock::now();

    auto elapsed = std::chrono::duration<double>(end - start);

    ASSERT_GT(elapsed.count(), 1.f);
    ASSERT_LT(elapsed.count(), 1.5);
    ASSERT_EQ(clients.size(), 0);
}

TEST_F(Selector_single, unique_client)
{
    ASSERT_EQ(selector.size(), 1);
}

TEST_F(Selector_single, same_client)
{
    ASSERT_FALSE(selector.client(client));
    ASSERT_EQ(selector.size(), 1);
}

TEST_F(Selector_single, erase_client)
{
    selector.erase(client);
    ASSERT_EQ(selector.size(), 0);
}

TEST_F(Selector_single, erase_nullptr)
{
    selector.erase(nullptr);
    ASSERT_EQ(selector.size(), 1);
}

TEST_F(Selector_single, pull_client)
{
    const std::string msg = "this is a test";
    ASSERT_EQ(socket.send(msg), msg.size());

    std::vector<net::Selector<net::StreamTcp>::Client> clients = selector.pull();
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