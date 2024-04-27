#pragma once

#include "Common/Core/Order.hpp"
#include "Common/Core/Logger.hpp"

class History
{
    public:
        History() = default;
        ~History() = default;

        /**
         * @brief Add an order to the list
         * @param _order The order to add
         */
        void addOrder(const OrderClient &_order);

        /**
         * @brief Add an order to the history
         * @param _order The order to add
         */
        void addHistory(const OrderClient &_order);

        /**
         * @brief Remove an order from the list
         * @param _orderId The id of the order to remove
         */
        void removeOrder(const std::string &_orderId);

        /**
         * @brief Replace an order by another one (delete the old one and add the new one)
         * @param _orderId The id of the order to replace
         * @param _order The new order
         */
        void replaceOrder(const std::string &_orderId, const OrderClient &_order);

        /**
         * @brief Reset the history and the orders
         */
        void reset();

        void setOrderToCancel(const std::string &_orderId) { m_orderToCancel = _orderId; };

        void clearOrderToCancel() { m_orderToCancel = ""; };

        friend std::ostream &operator<<(std::ostream &_os, const History &_history);
        std::vector<OrderClient> getOrders() const { return m_orders; };
        std::vector<OrderClient> getHistory() const { return m_history; };
        std::string getOrderToCancel() const { return m_orderToCancel; };
    private:
        std::vector<OrderClient> m_orders{};
        std::vector<OrderClient> m_history{};
        std::string m_orderToCancel = "";
};

std::ostream &operator<<(std::ostream &_os, const std::vector<OrderClient> &_orders);