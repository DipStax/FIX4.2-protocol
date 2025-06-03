#include <gtest/gtest.h>

#include <iostream>

#include "Common/Network/Socket.hpp"
#include "Common/Network/Acceptor.hpp"

#define TEST_IP_TCP "127.0.0.1"

class Socket_ConnectClose : public testing::Test
{
    protected:
        void SetUp() override
        {
            acceptor.listen(0);
            port = acceptor.getPort();
        }

        void TearDown() override
        {
            (void)acceptor.close();
        }

        net::Acceptor<net::StreamTcp> acceptor;
        uint32_t port = 0;
};

TEST_F(Socket_ConnectClose, connect_close_server) {
    net::StreamTcp socket;

    ASSERT_TRUE(socket.connect(TEST_IP_TCP, port));

    net::Acceptor<net::StreamTcp>::Client client = acceptor.accept();

    ASSERT_NE(client, nullptr);
    EXPECT_TRUE(client->close());
    EXPECT_FALSE(client->isOpen());
    EXPECT_FALSE(socket.isOpen());
}

TEST_F(Socket_ConnectClose, connect_close_client) {
    net::StreamTcp socket;

    ASSERT_TRUE(socket.connect(TEST_IP_TCP, port));

    net::Acceptor<net::StreamTcp>::Client client = acceptor.accept();

    ASSERT_NE(client, nullptr);

    ASSERT_TRUE(client->close());
    ASSERT_FALSE(socket.isOpen());
    ASSERT_FALSE(client->isOpen());
}

class Socket_SendRecv : public testing::Test
{
    protected:
        void SetUp() override
        {
            acceptor.listen(0);
            socket.connect(TEST_IP_TCP, acceptor.getPort());
            client = acceptor.accept();
        }

        void TearDown() override
        {
            (void)acceptor.close();
            (void)socket.close();
            client = nullptr;
        }

        net::StreamTcp socket;
        net::Acceptor<net::StreamTcp> acceptor;
        net::Acceptor<net::StreamTcp>::Client client;
};

TEST_F(Socket_SendRecv, single_send_single_recv)
{
    const std::string send1 = "this is a test";
    int error = 0;

    ASSERT_EQ(socket.send(send1), send1.size());

    std::string recv = client->receive(send1.size(), error);
    ASSERT_EQ(error, send1.size());
    ASSERT_EQ(recv.size(), send1.size());
    ASSERT_EQ(recv, send1);
}

TEST_F(Socket_SendRecv, multi_send_single_recv)
{
    const std::string send1 = "this is ";
    const std::string send2 = "a test";
    int error = 0;

    ASSERT_EQ(socket.send(send1), send1.size());
    ASSERT_EQ(socket.send(send2), send2.size());

    std::string recv = client->receive(send1.size() + send2.size(), error);
    ASSERT_EQ(error, send1.size() + send2.size());
    ASSERT_EQ(recv.size(), send1.size() + send2.size());
    ASSERT_EQ(recv, send1 + send2);
}

TEST_F(Socket_SendRecv, single_send_multi_recv)
{
    int error = 0;
    const std::string send1 = "this is a test";
    // The sample data to send should have an odd length

    ASSERT_EQ(socket.send(send1), send1.size());

    std::string recv = client->receive(send1.size() / 2, error);
    ASSERT_EQ(error, send1.size() / 2);
    ASSERT_EQ(recv.size(), send1.size() / 2);
    ASSERT_EQ(recv, send1.substr(0, send1.size() / 2));

    recv = client->receive(send1.size() / 2, error);
    ASSERT_EQ(error, send1.size() / 2);
    ASSERT_EQ(recv.size(), send1.size() / 2);
    ASSERT_EQ(recv, send1.substr(send1.size() / 2, send1.size()));
}