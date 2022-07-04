#include <iostream>
#include <set>
#include "quickfix/config.h"
#include "quickfix/Session.h"
#include "test_application.h"

////////////////////////////////////////////////////////////////////////////////////////////////
//! Application 的虚接口实现

void CTestApplication::onCreate(const FIX::SessionID& sid)
{
    std::cout << "create: sid=" << sid << std::endl;
}

void CTestApplication::onLogon(const FIX::SessionID& sid)
{
    std::cout << "logon: sid=" << sid << std::endl;
}

void CTestApplication::onLogout(const FIX::SessionID& sid)
{
    std::cout << "logout: sid=" << sid << std::endl;
}

void CTestApplication::toAdmin(FIX::Message& msg, const FIX::SessionID& sid)
{
    FIX::MsgSeqNum seq_num;
    msg.getHeader().getField(seq_num);

    std::cout << "test [toAdmin]: seq=" << seq_num << std::endl;

    FIX::MsgType msg_type;
    msg.getHeader().getField(msg_type);
    if (msg_type.getValue() == FIX::MsgType_Heartbeat)
    {
        std::cout << "toAdmin: session_id=" << sid << ", msg_type=Heartbeat" << std::endl;
    }
    else
    {
        std::cout << "toAdmin Event: session_id=" << sid << ", msg_type=" << msg_type << ", msg=" << msg << std::endl;
    }
}

void CTestApplication::toApp(FIX::Message& msg, const FIX::SessionID& sid)
    EXCEPT (FIX::DoNotSend)
{
    FIX::MsgSeqNum seq_num;
    msg.getHeader().getField(seq_num);

    std::cout << "test [toApp]: seq=" << seq_num << std::endl;

    FIX::MsgType msg_type;
    msg.getHeader().getField(msg_type);
    if (msg_type.getValue() != FIX::MsgType_ExecutionReport)
    {
        return;
    }

    std::cout << "toApp: session_id=" << sid << ", msg=" << msg << std::endl;
}

void CTestApplication::fromAdmin(const FIX::Message& msg, const FIX::SessionID& sid)
    EXCEPT (FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon)
{
    FIX::MsgSeqNum seq_num;
    msg.getHeader().getField(seq_num);

    std::cout << "test [fromAdmin]: seq=" << seq_num << std::endl;

    FIX::MsgType msg_type;
    msg.getHeader().getField(msg_type);
    if (msg_type.getValue() != FIX::MsgType_Heartbeat)
    {
        std::cout << "fromAdmin: session_id=" << sid << ", msg_type=" << msg_type << ", msg=" << msg << std::endl;
    }
    else
    {
        std::cout << "fromAdmin: session_id=" << sid << ", msg_type=Heartbeat" << std::endl;
    }

    // 处理消息间隙，同步序列号
    if (msg_type.getValue() == FIX::MsgType_Logout)
    {
        FIX::Text txt;
        if (msg.isSetField(txt))
        {
            msg.getField(txt);
            std::string txt_val = txt;
            int32_t seqNum = 0;
            std::string::size_type pos = txt_val.find("expecting");
            if (pos != txt_val.npos)
            {
                seqNum = std::strtol(txt_val.substr(pos + 10).c_str(), nullptr, 10);
                FIX::Session::lookupSession(sid)->setNextSenderMsgSeqNum(seqNum);
            }
        }
    }
}

void CTestApplication::fromApp(const FIX::Message& msg, const FIX::SessionID& sid)
    EXCEPT (FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType)
{
    FIX::MsgSeqNum seq_num;
    msg.getHeader().getField(seq_num);

    std::cout << "test [fromApp]: seq=" << seq_num << std::endl;
    crack(msg, sid);
}

////////////////////////////////////////////////////////////////////////////////////////////////
//! MessageCracker 的虚接口实现

void CTestApplication::onMessage(const FIX42::ExecutionReport& msg, const FIX::SessionID& sid)
{
    std::cout << "onMessage: sid=" << sid << ", msg=" << msg << std::endl;
}

void CTestApplication::onMessage(const FIX42::OrderCancelReject& msg, const FIX::SessionID& sid)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////
//! 测试下单消息的发送

//! 测试发送 NewOrderSingle 消息
void CTestApplication::test_send_newordersingle()
{
    FIX::OrdType ordType;

    FIX42::NewOrderSingle newOrderSingle(
        FIX::ClOrdID("at_coi_001"),
        FIX::HandlInst('1'),
        FIX::Symbol("at_symb_001"),
        FIX::Side(FIX::Side_BUY),
        FIX::TransactTime(),
        ordType = FIX::OrdType(FIX::OrdType_LIMIT)
    );

    newOrderSingle.set(FIX::OrderQty(100000.0));
    newOrderSingle.set(FIX::TimeInForce(FIX::TimeInForce_AT_THE_OPENING));
    if (ordType == FIX::OrdType_LIMIT || ordType == FIX::OrdType_STOP_LIMIT)
    {
        newOrderSingle.set(FIX::Price(100.0));
    }

    if (ordType == FIX::OrdType_STOP || ordType == FIX::OrdType_STOP_LIMIT)
    {
        newOrderSingle.set(FIX::StopPx(110.0));
    }

    FIX::Header header = newOrderSingle.getHeader();
    std::set<FIX::SessionID> sids = m_settings.getSessions();
    for (auto sid : sids)
    {
        header.setField(sid.getSenderCompID());
        header.setField(sid.getTargetCompID());
        header.setField(sid.getBeginString());

        try
        {
            FIX::Session::sendToTarget(newOrderSingle, sid);
        }
        catch (FIX::SessionNotFound&)
        {
            std::cout << "send execution report to target failed: session_id=" << sid << ", msg=" << newOrderSingle << std::endl;
        }
    }
}
