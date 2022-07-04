#include <iostream>
#include "quickfix/fix42/ExecutionReport.h"
#include "application.h"

void CTestApplication::onCreate(const FIX::SessionID& sid)
{
    std::cout << "onCreate: session_id=" << sid << std::endl;
}

void CTestApplication::onLogon(const FIX::SessionID& sid)
{
    std::cout << "onLogon: session_id=" << sid << std::endl;
}

void CTestApplication::onLogout(const FIX::SessionID& sid)
{
    std::cout << "onLogout: session_id=" << sid << std::endl;
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

void CTestApplication::toApp(FIX::Message& msg, const FIX::SessionID& sid) EXCEPT(FIX::DoNotSend)
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
    EXCEPT(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon)
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
    EXCEPT(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType)
{
    FIX::MsgSeqNum seq_num;
    msg.getHeader().getField(seq_num);

    std::cout << "test [fromApp]: seq=" << seq_num << std::endl;
    crack(msg, sid);
}

void CTestApplication::onMessage(const FIX42::NewOrderSingle& new_order, const FIX::SessionID& sid)
{
    FIX::Symbol symbol;
    FIX::Side side;
    FIX::OrdType ordType;
    FIX::OrderQty orderQty;
    FIX::Price price;
    FIX::ClOrdID clOrdID;
    FIX::Account account;

    new_order.get( ordType );

    if ( ordType != FIX::OrdType_LIMIT )
    {
        throw FIX::IncorrectTagValue(ordType.getField());
    }

    new_order.get(symbol);
    new_order.get(side);
    new_order.get(orderQty);
    new_order.get(price);
    new_order.get(clOrdID);

    // 发送执行报告给客户端
    FIX42::ExecutionReport executionReport = FIX42::ExecutionReport
      ( FIX::OrderID(genOrderID()),
        FIX::ExecID(genExecID()),
        FIX::ExecTransType(FIX::ExecTransType_NEW),
        FIX::ExecType(FIX::ExecType_NEW),
        FIX::OrdStatus(FIX::OrdStatus_NEW),
        symbol,
        side,
        FIX::LeavesQty(0),
        FIX::CumQty(orderQty),
        FIX::AvgPx(price));

    executionReport.set(clOrdID);
    executionReport.set(orderQty);
    executionReport.set(FIX::LastShares(orderQty));
    executionReport.set(FIX::LastPx(price));

    if(new_order.isSet(account))
    {        
        executionReport.setField(new_order.get(account));
    }

    try
    {
        FIX::Session::sendToTarget(executionReport, sid);
    }
    catch (FIX::SessionNotFound&)
    {
        std::cout << "send execution report to target failed: session_id=" << sid << ", msg=" << new_order << std::endl;
    }
}
