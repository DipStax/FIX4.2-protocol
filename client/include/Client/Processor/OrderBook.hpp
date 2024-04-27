#pragma once

#include <memory>
#include <map>
#include <optional>

#include "Client/Processor/Common/DataRequest.hpp"
#include "Client/Processor/IProcessor.hpp"
#include "Common/Thread/Queue.hpp"
#include "Client/meta.hpp"

namespace data
{
    template<class T>
    std::vector<T> extract(fix::Serializer::AnonMessage &_msg, const std::string &_tag);

    struct IncrRefresh
    {
        IncrRefresh(size_t _size);

        size_t size;
        std::vector<Price> prices;
        std::vector<OrderType> types;
        std::vector<Quantity> quantitys;
        std::vector<std::string> symbols;

        bool is_valid() const;
    };

    struct FullRefresh
    {
        size_t size;
        std::string symbol;
        std::vector<OrderType> types;
        std::vector<Price> prices;
        std::vector<Quantity> quantitys;
    };
}

namespace proc
{
    class OrderBook : public com::DataRequest<proc::IMessage, proc::IEntry>, public proc::IUDP
    {
        public:
            using BidBook = std::map<Price, Quantity, std::less<Price>>;
            using AskBook = std::map<Price, Quantity, std::greater<Price>>;

            OrderBook() = default;
            ~OrderBook() = default;

            [[nodiscard]] virtual bool handle(const Entry &_entry, const Context &_ctx) const override final;
            [[nodiscard]] virtual bool handle(fix::Serializer::AnonMessage &_msg, const Context &_ctx) const override final;

            [[nodiscard]] virtual std::optional<fix::Message> process(const Entry &_entry, Context &_ctx) override final;
            [[nodiscard]] virtual std::optional<fix::Message> process(fix::Serializer::AnonMessage &_msg, Context &_ctx) override final;
            [[nodiscard]] virtual std::optional<data::UDPPackage> process(const data::UDPPackage &_package, Context &_ctx) override final;

        protected:
            void treatIncrRefresh(fix::Serializer::AnonMessage &_msg);
            void treatFullRefresh(fix::Serializer::AnonMessage &_msg);

            [[nodiscard]] data::IncrRefresh loadIncrRefresh(fix::Serializer::AnonMessage &_msg);
            [[nodiscard]] data::FullRefresh loadFullRefresh(fix::Serializer::AnonMessage &_msg);

            [[nodiscard]] fix::Message buildFullRefresh() const;
            [[nodiscard]] fix::Message buildIncrRefresh() const;

        private:
            using BidMap = std::map<std::string, BidBook>;
            using AskMap = std::map<std::string, AskBook>;

            template<class T>
            requires IsBookOf<T, Quantity>
            void displayBook(const T &_book) const;

            using SyncFn = Quantity (*)(Quantity, Quantity);

            [[nodiscard]] static Quantity QtySync(Quantity _left, Quantity _right);
            [[nodiscard]] static Quantity CopySync(Quantity _left, Quantity _right);

            template<class T, class _T>
            static void sync_book(T &_origine, _T &_target, SyncFn _sync);

            // turn this function into class later
            template<class T>
            static void functional_sync(T &_map, const std::string &_sym, Price _price, Quantity _qty, SyncFn _sync);

            bool m_is_sync = false;

            BidMap m_bid;
            AskMap m_ask;

            BidMap m_bid_last;
            AskMap m_ask_last;
    };
}

#include "Client/Processor/OrderBook.inl"