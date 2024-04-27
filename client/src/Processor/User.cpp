#include <cstring>

#include <getopt.h>

#include "Client/Processor/User.hpp"
#include "Common/Message/Logon.hpp"
#include "Common/Message/Logout.hpp"
#include "Common/Message/Tag.hpp"
#include "Common/Core/Logger.hpp"

namespace proc
{
    bool User::handle(fix::Serializer::AnonMessage &_msg, const Context &_ctx) const
    {
        return _msg.at(fix::Tag::MsgType) == fix::Logon::MsgType || (_ctx.Loggin && _msg.at(fix::Tag::MsgType) == fix::Logout::MsgType);
    }

    bool User::handle(const Entry &_entry, const Context &_ctx) const
    {
        return std::strcmp(_entry.args.at(0), "logon") == 0 || (_ctx.Loggin && std::strcmp(_entry.args.at(0), "logout"));
    }

    std::optional<fix::Message> User::process(fix::Serializer::AnonMessage &_msg, Context &_ctx)
    {
        if (_msg.at(fix::Tag::MsgType) == fix::Logon::MsgType) {
            _ctx.User = _msg.at(fix::Tag::TargetCompId);
            _ctx.HeartBit = utils::to<uint32_t>(_msg.at(fix::Tag::HearBtInt));
            _ctx.Loggin = true;
        } else if (_ctx.Loggin && _msg.at(fix::Tag::MsgType) == fix::Logout::MsgType) {
            _ctx.Loggin = false;
        }
        return {};
    }

    std::optional<fix::Message> User::process(const Entry &_entry, Context &_ctx)
    {
        int param = 0;

        if (std::strcmp(_entry.args.at(0), "logon") == 0) {
            while (param != -1) {
                param = getopt(_entry.args.size(), const_cast<char * const *>(_entry.args.data()), "u:");
                switch (param) {
                    case ':':       // missing param
                    case '?':       // unknow
                        return {};
                    case 'u':
                        return buildLogon(optarg);
                };
            }
        // } else if (words.at(0) == "new_order") {
        //     std::vector<const char *> cwords;
        //     std::string orderId = "";
        //     std::string quantity = "";
        //     std::string price = "";
        //     std::string side = "";
        //     std::string symbol = "";

        //     for (auto &_word : words)
        //         cwords.emplace_back(_word.c_str());

        //     while (param != -1) {
        //         param = getopt(words.size(), const_cast<char * const *>(cwords.data()), "i:q:p:s:S:");
        //         switch (param) {
        //             case ':':       // missing param
        //             case '?':       // unknow
        //                 return {};
        //             case 'i':
        //                 orderId = optarg;
        //                 break;
        //             case 'q':
        //                 quantity = optarg;
        //                 break;
        //             case 'p':
        //                 price = optarg;
        //                 break;
        //             case 's':
        //                 side = optarg;
        //                 break;
        //             case 'S':
        //                 symbol = optarg;
        //                 break;
        //         };
        //     }
        //     if (orderId.empty() || quantity.empty() || price.empty() || side.empty() || symbol.empty()) {
        //         Logger::Log("[User Input]: Missing parameter for new_order");
        //         return {};
        //     }
        //     return buildNewOrder(orderId, quantity, price, side, symbol);
        // } else if (words.at(0) == "cancel_order") {
        //     std::vector<const char *> cwords;
        //     std::string cancelOrderId = "";
        //     std::string targetOrderId = "";
        //     std::string symbol = "";
        //     std::string side = "";

        //     for (auto &_word : words)
        //         cwords.emplace_back(_word.c_str());

        //     while (param != -1) {
        //         param = getopt(words.size(), const_cast<char * const *>(cwords.data()), "i:t:S:s:");
        //         switch (param) {
        //             case ':':       // missing param
        //             case '?':       // unknow
        //                 return {};
        //             case 'i': cancelOrderId = optarg;
        //                 break;
        //             case 't': targetOrderId = optarg;
        //                 break;
        //             case 'S': symbol = optarg;
        //                 break;
        //             case 's': side = optarg;
        //                 break;
        //         };
        //     }
        //     if (cancelOrderId.empty() || targetOrderId.empty() || symbol.empty() || side.empty()) {
        //         Logger::Log("[User Input]: Missing parameter for cancel_order");
        //         return {};
        //     }
        //     return buildOrderCancel(cancelOrderId, targetOrderId, symbol, side);
        // } else if (words.at(0) == "cancel_replace") {
        //     std::vector<const char *> cwords;
        //     std::string cancelReplaceOrderId = "";
        //     std::string targetOrderId = "";
        //     std::string quantity = "";
        //     std::string price = "";
        //     std::string side = "";
        //     std::string symbol = "";

        //     for (auto &_word : words)
        //         cwords.emplace_back(_word.c_str());

        //     while (param != -1) {
        //         param = getopt(words.size(), const_cast<char * const *>(cwords.data()), "i:t:q:p:s:S:");
        //         switch (param) {
        //             case ':':       // missing param
        //             case '?':       // unknow
        //                 return {};
        //             case 'i':
        //                 cancelReplaceOrderId = optarg;
        //                 break;
        //             case 't':
        //                 targetOrderId = optarg;
        //                 break;
        //             case 'q':
        //                 quantity = optarg;
        //                 break;
        //             case 'p':
        //                 price = optarg;
        //                 break;
        //             case 's':
        //                 side = optarg;
        //                 break;
        //             case 'S':
        //                 symbol = optarg;
        //                 break;
        //         };
        //     }
        //     if (cancelReplaceOrderId.empty() || targetOrderId.empty() || quantity.empty() || price.empty() || side.empty() || symbol.empty()) {
        //         Logger::Log("[User Input]: Missing parameter for cancel_replace");
        //         return {};
        //     }
        //     _ctx.userInfos.setOrderToCancel(targetOrderId);
        //     return buildOrderCancelReplace(cancelReplaceOrderId, targetOrderId, quantity, price, side, symbol);
        } else if (std::strcmp(_entry.args.at(0), "logout") == 0) {
            return buildLogout();
        } else if (std::strcmp(_entry.args.at(0), "help") == 0) {
            std::cout << "Commands:" << std::endl;
            std::cout << "\tlogon\t-u <user>" << std::endl;
            std::cout << "\tnew\t\t-i <id> -q <quantity> -p <price> -s <side>(buy|sell) -S <symbol>(GOLD|USD|EURO)" << std::endl;
            std::cout << "\tcancel\t-i <id> -t <target_id> -s <side>(buy|sell) -S <symbol>(GOLD|USD|EURO)" << std::endl;
            std::cout << "\treplace\t-i <id> -t <target_id> -q <quantity> -p <price> -s <side>(buy|sell) -S <symbol>(GOLD|USD|EURO)" << std::endl;
            std::cout << "\tfresh\t-i <id> -d <depth> -s <side>(buy|sell) -S <symbol>(GOLD|USD|EURO)" << std::endl;
            std::cout << "\tsub\t\t-i <id> -d <depth> -s <side>(buy|sell) -S <symbol>(GOLD|USD|EURO)" << std::endl;
            std::cout << "\tob\t\t-s <side>(bid|ask) -S <symbol>(GOLD|USD|EURO)" << std::endl;
            std::cout << "\tlogout" << std::endl;
            std::cout << "\tstatus" << std::endl;
            std::cout << "\torder_history" << std::endl;
            std::cout << "\thelp" << std::endl;
            return {};
        } else if (std::strcmp(_entry.args.at(0), "order_history") == 0) {
            std::cout << _ctx.userInfos.getHistory() << std::endl;
        }
        return {};
    }

    std::optional<fix::Message> User::build(char _tag, const Context &_ctx) const
    {
        if (_tag == fix::Logon::cMsgType)
            return buildLogon(_ctx.User);
        else if (_tag == fix::Logout::cMsgType)
            return buildLogout();
        return {};
    }

    fix::Message User::buildLogon(const UserId &_id) const
    {
        fix::Logon logon;

        logon.header.set49_SenderCompId(_id);
        logon.set98_EncryptMethod("0");
        logon.set108_HeartBtInt("30");
        return logon;
    }

    fix::Message User::buildNewOrder(const std::string &_orderId, const std::string &_quantity, const std::string &_price, const std::string &_side, const std::string &_symbol) const
    {
        fix::NewOrderSingle order;

        order.set11_clOrdID(_orderId);
        order.set21_handlInst("3");
        order.set38_orderQty(_quantity);
        order.set40_ordType("2");
        order.set44_price(_price);
        order.set54_side((_side == "buy") ? "3" : "4");
        order.set55_symbol(_symbol);
        order.set60_transactTime(utils::get_timestamp());

        return order;
    }

    fix::Message User::buildOrderCancel(const std::string &_cancelOrderId, const std::string &_targetOrderId, const std::string &_symbol, const std::string &_side) const
    {
        fix::OrderCancelRequest cancel;

        cancel.set11_clOrdID(_cancelOrderId);
        cancel.set41_origClOrdID(_targetOrderId);
        cancel.set55_symbol(_symbol);
        cancel.set54_side((_side == "buy") ? "3" : "4");

        return cancel;
    }

    fix::Message User::buildOrderCancelReplace(const std::string &_cancelReplaceOrderId, const std::string &_targetOrderId, const std::string &_quantity, const std::string &_price, const std::string &_side, const std::string &_symbol) const
    {
        fix::OrderCancelReplaceRequest cancelReplace;

        cancelReplace.set11_clOrdID(_cancelReplaceOrderId);
        cancelReplace.set21_handlInst("3");
        cancelReplace.set38_orderQty(_quantity);
        cancelReplace.set40_ordType("2");
        cancelReplace.set41_origClOrdID(_targetOrderId);
        cancelReplace.set44_price(_price);
        cancelReplace.set54_side((_side == "buy") ? "3" : "4");
        cancelReplace.set55_symbol(_symbol);

        return cancelReplace;
    }

    fix::Message User::buildLogout() const
    {
        return fix::Logout();
    }
}