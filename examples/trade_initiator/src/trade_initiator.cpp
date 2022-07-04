#include <string>
#include <iostream>
#include "quickfix/config.h"
#include "quickfix/FileStore.h"
#include "quickfix/SocketInitiator.h"
#include "quickfix/SessionSettings.h"
//#include "quickfix/Log.h"
#include "quickfix/FileLog.h"
#include "test_application.h"

int main(int argc, char** argv)
{
    if (argc > 1 && argc != 2)
    {
        std::cerr << "usage: " << argv[0] << " [CONFIG FILE]" << std::endl;
        return 0;
    }

    std::string fix_conf_file = "../conf/fix.ini";
    std::string spec_file = "../conf/FIX42.xml";
    if (argc == 2)
    {
        fix_conf_file = argv[1];
    }

    //! 创建 initiator
    FIX::Initiator* initiator = nullptr;
    try
    {
        FIX::SessionSettings settings(fix_conf_file);
        // 存储路径
        FIX::FileStoreFactory store_factory(settings);
        // 日志路径: 输出到屏幕
        //FIX::ScreenLogFactory log_factory(settings);
        FIX::FileLogFactory log_factory(settings);
        FIX::Message::InitializeXML(spec_file);

        CTestApplication app(settings);
        initiator = new FIX::SocketInitiator(app, store_factory, settings, log_factory);
        initiator->start();

        // 测试发送 NewOrderSingle 消息
        app.test_send_newordersingle();
        // 等待接收消息
        while (true)
        {
            FIX::process_sleep(1.0);
        }
        initiator->stop();
        delete initiator;
        initiator = nullptr;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        if (initiator != nullptr)
        {
            delete initiator;
            initiator = nullptr;
        }
        return 1;
    }
    
    return 0;
}
