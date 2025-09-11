#pragma once

namespace fix42::tag
{
    constexpr const char AvgPx[] = "6";                     ///< Tag [6](https://www.onixs.biz/fix-dictionary/4.2/tagNum_6.html).
    constexpr const char BeginString[] = "8";               ///< Tag [8](https://www.onixs.biz/fix-dictionary/4.2/tagNum_8.html).
    constexpr const char BodyLength[] = "9";                ///< Tag [9](https://www.onixs.biz/fix-dictionary/4.2/tagNum_9.html).
    constexpr const char CheckSum[] = "10";                 ///< Tag [10](https://www.onixs.biz/fix-dictionary/4.2/tagNum_10.html).
    constexpr const char ClOrdID[] = "11";                  ///< Tag [11](https://www.onixs.biz/fix-dictionary/4.2/tagNum_11.html).
    constexpr const char CumQty[] = "14";                   ///< Tag [14](https://www.onixs.biz/fix-dictionary/4.2/tagNum_14.html).
    constexpr const char ExecId[] = "17";                   ///< Tag [17](https://www.onixs.biz/fix-dictionary/4.2/tagNum_17.html).
    constexpr const char ExecTransType[] = "20";            ///< Tag [20](https://www.onixs.biz/fix-dictionary/4.2/tagNum_20.html).
    constexpr const char HandlInst[] = "21";                ///< Tag [21](https://www.onixs.biz/fix-dictionary/4.2/tagNum_21.html).
    constexpr const char MsqSeqNum[] = "34";                ///< Tag [34](https://www.onixs.biz/fix-dictionary/4.2/tagNum_34.html).
    constexpr const char MsgType[] = "35";                  ///< Tag [35](https://www.onixs.biz/fix-dictionary/4.2/tagNum_35.html).
    constexpr const char OrderID[] = "37";                  ///< Tag [37](https://www.onixs.biz/fix-dictionary/4.2/tagNum_37.html).
    constexpr const char OrderQty[] = "38";                 ///< Tag [38](https://www.onixs.biz/fix-dictionary/4.2/tagNum_38.html).
    constexpr const char OrdStatus[] = "39";                ///< Tag [39](https://www.onixs.biz/fix-dictionary/4.2/tagNum_39.html).
    constexpr const char OrdType[] = "40";                  ///< Tag [40](https://www.onixs.biz/fix-dictionary/4.2/tagNum_40.html).
    constexpr const char OrigClOrdID[] = "41";              ///< Tag [41](https://www.onixs.biz/fix-dictionary/4.2/tagNum_41.html).
    constexpr const char Price[] = "44";                    ///< Tag [44](https://www.onixs.biz/fix-dictionary/4.2/tagNum_44.html).
    constexpr const char RefSeqNum[] = "45";                ///< Tag [45](https://www.onixs.biz/fix-dictionary/4.2/tagNum_45.html).
    constexpr const char SenderCompId[] = "49";             ///< Tag [49](https://www.onixs.biz/fix-dictionary/4.2/tagNum_49.html).
    constexpr const char SendingTime[] = "52";              ///< Tag [52](https://www.onixs.biz/fix-dictionary/4.2/tagNum_52.html).
    constexpr const char Side[] = "54";                     ///< Tag [54](https://www.onixs.biz/fix-dictionary/4.2/tagNum_54.html).
    constexpr const char Symbol[] = "55";                   ///< Tag [55](https://www.onixs.biz/fix-dictionary/4.2/tagNum_55.html).
    constexpr const char TargetCompId[] = "56";             ///< Tag [56](https://www.onixs.biz/fix-dictionary/4.2/tagNum_56.html).
    constexpr const char Text[] = "58";                     ///< Tag [58](https://www.onixs.biz/fix-dictionary/4.2/tagNum_58.html).
    constexpr const char TransactTime[] = "60";             ///< Tag [60](https://www.onixs.biz/fix-dictionary/4.2/tagNum_60.html).
    constexpr const char EncryptMethod[] = "98";            ///< Tag [98](https://www.onixs.biz/fix-dictionary/4.2/tagNum_98.html).
    constexpr const char HeartBtInt[] = "108";               ///< Tag [108](https://www.onixs.biz/fix-dictionary/4.2/tagNum_108.html).
    constexpr const char MinQty[] = "110";                  ///< Tag [110](https://www.onixs.biz/fix-dictionary/4.2/tagNum_110.html).
    constexpr const char TestReqId[] = "112";               ///< Tag [112](https://www.onixs.biz/fix-dictionary/4.2/tagNum_112.html).
    constexpr const char NoRelatedSym[] = "146";            ///< Tag [146](https://www.onixs.biz/fix-dictionary/4.2/tagNum_146.html).
    constexpr const char ExecType[] = "150";                ///< Tag [150](https://www.onixs.biz/fix-dictionary/4.2/tagNum_150.html).
    constexpr const char LeavesQty[] = "151";               ///< Tag [151](https://www.onixs.biz/fix-dictionary/4.2/tagNum_151.html).
    constexpr const char MDReqID[] = "262";                 ///< Tag [262](https://www.onixs.biz/fix-dictionary/4.2/tagNum_262.html).
    constexpr const char SubscriptionRequestType[] = "263"; ///< Tag [263](https://www.onixs.biz/fix-dictionary/4.2/tagNum_263.html).
    constexpr const char MarketDepth[] = "264";             ///< Tag [264](https://www.onixs.biz/fix-dictionary/4.2/tagNum_264.html).
    constexpr const char MDUpdateType[] = "265";            ///< Tag [265](https://www.onixs.biz/fix-dictionary/4.2/tagNum_265.html).
    constexpr const char NoMDEntryTypes[] = "267";          ///< Tag [267](https://www.onixs.biz/fix-dictionary/4.2/tagNum_267.html).
    constexpr const char NoMDEntries[] = "268";             ///< Tag [268](https://www.onixs.biz/fix-dictionary/4.2/tagNum_268.html).
    constexpr const char MDEntryType[] = "269";             ///< Tag [269](https://www.onixs.biz/fix-dictionary/4.2/tagNum_269.html).
    constexpr const char MDEntryPx[] = "270";               ///< Tag [270](https://www.onixs.biz/fix-dictionary/4.2/tagNum_270.html).
    constexpr const char MDUpdateAction[] = "279";          ///< Tag [279](https://www.onixs.biz/fix-dictionary/4.2/tagNum_279.html).
    constexpr const char MDentryRefID[] = "280";            ///< Tag [280](https://www.onixs.biz/fix-dictionary/4.2/tagNum_280.html).
    constexpr const char RefTagId[] = "371";                ///< Tag [371](https://www.onixs.biz/fix-dictionary/4.2/tagNum_371.html).
    constexpr const char RefMsgType[] = "372";              ///< Tag [372](https://www.onixs.biz/fix-dictionary/4.2/tagNum_372.html).
    constexpr const char SessionRejectReason[] = "373";     ///< Tag [373](https://www.onixs.biz/fix-dictionary/4.2/tagNum_373.html).
    constexpr const char BusinessRejectRefId[] = "379";     ///< Tag [379](https://www.onixs.biz/fix-dictionary/4.2/tagNum_379.html).
    constexpr const char BusinessRejectReason[] = "380";     ///< Tag [380](https://www.onixs.biz/fix-dictionary/4.2/tagNum_380.html).
}