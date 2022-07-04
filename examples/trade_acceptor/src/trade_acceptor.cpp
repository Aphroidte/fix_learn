#include <string>
#include <fstream>
#include <iostream>
#include "quickfix/config.h"
#include "quickfix/FileStore.h"
#include "quickfix/SocketAcceptor.h"
//#include "quickfix/Log.h"
#include "quickfix/FileLog.h"
#include "quickfix/SessionSettings.h"
#include "quickfix/Acceptor.h"
#include "quickfix/Utility.h"
#include "application.h"

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

    //! 创建 acceptor
    FIX::Acceptor* acceptor = nullptr;
    try
    {
        FIX::SessionSettings settings(fix_conf_file);
        // 存储路径
        FIX::FileStoreFactory store_factory(settings);
        // 日志路径: 输出到屏幕
        //FIX::ScreenLogFactory log_factory(settings);
        FIX::FileLogFactory log_factory(settings);
        FIX::Message::InitializeXML(spec_file);

        CTestApplication app;
        acceptor = new FIX::SocketAcceptor(app, store_factory, settings, log_factory);
        acceptor->start();

        // 等待接收消息
        while (true)
        {
            FIX::process_sleep(1.0);
        }
        acceptor->stop();
        delete acceptor;
        acceptor = nullptr;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        if (acceptor != nullptr)
        {
            delete acceptor;
            acceptor = nullptr;
        }
        return 1;
    }
    
    return 0;
}