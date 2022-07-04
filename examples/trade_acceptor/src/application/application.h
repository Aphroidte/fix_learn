#ifndef TRADE_ACCEPTOR_APPLICATION_APPLICATION_H_
#define TRADE_ACCEPTOR_APPLICATION_APPLICATION_H_

#include <sstream>
#include "quickfix/Application.h"
#include "quickfix/MessageCracker.h"
#include "quickfix/Values.h"
#include "quickfix/Utility.h"
#include "quickfix/Mutex.h"
#include "quickfix/fix42/NewOrderSingle.h"

//! 测试消息的接收与处理：接收客户端的消息，并回发执行报告消息
class CTestApplication : public FIX::Application, public FIX::MessageCracker
{
 public:
    CTestApplication() : m_orderID(0), m_execID(0) {}
    virtual ~CTestApplication() {}

    //! Application 虚回调接口的实现
    virtual void onCreate(const FIX::SessionID& sid);
    virtual void onLogon(const FIX::SessionID& sid);
    virtual void onLogout(const FIX::SessionID& sid);
    virtual void toAdmin(FIX::Message& msg, const FIX::SessionID& sid);
    virtual void toApp(FIX::Message& msg, const FIX::SessionID& sid) EXCEPT(FIX::DoNotSend);
    virtual void fromAdmin(const FIX::Message& msg, const FIX::SessionID& sid) EXCEPT(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon);
    virtual void fromApp(const FIX::Message& msg, const FIX::SessionID& sid) EXCEPT(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType);

    //! MessageCracker 虚回调接口的实现
    virtual void onMessage(const FIX42::NewOrderSingle& new_order, const FIX::SessionID& sid);

 protected:
    std::string genOrderID()
    {
        std::stringstream stream;
        stream << ++m_orderID;
        return stream.str();
    }
  
    std::string genExecID()
    {
        std::stringstream stream;
        stream << ++m_execID;
        return stream.str();
    }

 private:
    int32_t     m_orderID;
    int32_t     m_execID;
};

#endif
