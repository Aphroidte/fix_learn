#ifndef TRADE_INITIATION_APPLICATION_TEST_APPLICATION_H_
#define TRADE_INITIATION_APPLICATION_TEST_APPLICATION_H_

#include <queue>
#include "quickfix/Application.h"
#include "quickfix/MessageCracker.h"
#include "quickfix/Values.h"
#include "quickfix/Mutex.h"
#include "quickfix/fix42/NewOrderSingle.h"
#include "quickfix/fix42/ExecutionReport.h"
#include "quickfix/fix42/OrderCancelRequest.h"
#include "quickfix/fix42/OrderCancelReject.h"
#include "quickfix/fix42/OrderCancelReplaceRequest.h"

//! 测试消息的交互：下单、撤单等操作
class CTestApplication : public FIX::Application, public FIX::MessageCracker
{
 public:
    explicit CTestApplication(FIX::SessionSettings& settings) : m_settings(settings) {}
    virtual ~CTestApplication() {}

    //! Application 的虚接口实现
    virtual void onCreate(const FIX::SessionID& sid);
    virtual void onLogon(const FIX::SessionID& sid);
    virtual void onLogout(const FIX::SessionID& sid);
    virtual void toAdmin(FIX::Message& msg, const FIX::SessionID& sid);
    virtual void toApp(FIX::Message& msg, const FIX::SessionID& sid) EXCEPT (FIX::DoNotSend);
    virtual void fromAdmin(const FIX::Message& msg, const FIX::SessionID& sid) EXCEPT (FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon);
    virtual void fromApp(const FIX::Message& msg, const FIX::SessionID& sid) EXCEPT (FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType);

    //! MessageCracker 的虚接口实现
    virtual void onMessage(const FIX42::ExecutionReport& msg, const FIX::SessionID& sid);
    virtual void onMessage(const FIX42::OrderCancelReject& msg, const FIX::SessionID& sid);

 public:
    //! 测试发送 NewOrderSingle 消息
    void test_send_newordersingle();

 protected:
    FIX::SessionSettings    m_settings;     // 配置
};

#endif
