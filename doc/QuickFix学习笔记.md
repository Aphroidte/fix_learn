# QuickFix学习笔记

<!-- TOC -->

- [QuickFix学习笔记](#quickfix学习笔记)
  - [1. 编译QuickFix库](#1-编译quickfix库)
    - [1.1. Windows环境](#11-windows环境)
    - [1.2. Linux/Solaris/FreeBSD/Mac OS X](#12-linuxsolarisfreebsdmac-os-x)
  - [2. QuickFix的配置](#2-quickfix的配置)
    - [2.1. QuickFix配置的样例](#21-quickfix配置的样例)
    - [2.2. QuickFix配置的含义](#22-quickfix配置的含义)
      - [2.2.1. Session相关配置](#221-session相关配置)
      - [2.2.2. Validation(校验)相关配置](#222-validation校验相关配置)
      - [2.2.3. Initiator相关配置](#223-initiator相关配置)
      - [2.2.4. Acceptor相关配置](#224-acceptor相关配置)
      - [2.2.5. 存储相关配置](#225-存储相关配置)
      - [2.2.6. 日志相关](#226-日志相关)
      - [2.2.7. SSL相关配置](#227-ssl相关配置)
      - [2.2.8. 其他](#228-其他)
  - [3. Fix应用程序](#3-fix应用程序)
    - [3.1. Fix::Application接口](#31-fixapplication接口)
    - [3.2. FIX::SynchronizedApplication实现](#32-fixsynchronizedapplication实现)
    - [3.3. FIX::NullApplication实现](#33-fixnullapplication实现)
    - [3.4. Fix创建应用程序的样例](#34-fix创建应用程序的样例)
  - [4. QuickFix消息](#4-quickfix消息)
    - [4.1. 构造消息](#41-构造消息)
      - [4.1.1. 存取消息中的域](#411-存取消息中的域)
      - [4.1.2. 获取消息的标准头跟标准尾](#412-获取消息的标准头跟标准尾)
    - [4.2. 发送消息](#42-发送消息)
      - [4.2.1. 发送消息的方法](#421-发送消息的方法)
    - [4.3. 接收与处理消息](#43-接收与处理消息)
      - [4.3.1. 接收应用消息](#431-接收应用消息)
      - [4.3.2. 处理应用消息](#432-处理应用消息)
      - [4.3.3. QuickFix重复组](#433-quickfix重复组)
        - [4.3.3.1. 创建包含重复组的消息](#4331-创建包含重复组的消息)
        - [4.3.3.2. 从消息中读取重复组的内容](#4332-从消息中读取重复组的内容)
  - [5. QuickFix用户自定义字段](#5-quickfix用户自定义字段)
    - [5.1. 安全地进行自定义字段](#51-安全地进行自定义字段)
    - [5.2. 创建自定义字段的宏](#52-创建自定义字段的宏)
  - [6. 验收测试](#6-验收测试)
  - [7. 消息验证](#7-消息验证)
  - [8. 测试](#8-测试)

<!-- /TOC -->

## 1. 编译QuickFix库

### 1.1. Windows环境

1. 使用 VS 打开 *quickfix_vs12.sln*, *quickfix_vs14.sln*, 或 *quickfix_vs15.sln*。
2. 批量编译所有的项目：*lib/quickfix.lib* 和 *lib/debug/quickfix.lib*。
3. 将编译出来的所有库链接到你的项目中
4. 将对应版本的头文件拷贝到你的项目中。（头文件路径 src/C++/fix版本号/）

编译选项由 *src* 目录下的 *config_windows.h* 文件控制，支持以下选项：

- `#define HAVE_STLPORT 1`：使用 *stlport* 替代标准的 *visual c++ STL* 进行编译。
- `#define HAVE_ODBC 1`：支持 *ODBC*。
- `#define HAVE_MYSQL 1`：支持 *Mysql*。如果启用该选项，*Mysql* 包含和库目录必须在 *Visual Studio* 搜索路径中。
- `#define HAVE_POSTGRESQL 1`：支持 *PostgreSql*。如果启用该选项，*PostgreSql* 包含和库目录必须在 *Visual Studio* 搜索路径中。

### 1.2. Linux/Solaris/FreeBSD/Mac OS X

执行以下命令即可：

    ./configure && ./make
    sudo make install

./configure 支持的选项：

- `--prefix=<base directory>`：选择安装的目录
- `--with-python2`：使用 *Python2 Api* 进行编译
- `--with-python3`：使用 *Python3 Api* 进行编译
- `--with-ruby`：使用 *Ruby Api* 进行编译
- `--with-mysql`：支持 *Mysql*
- `--with-postgresql`：支持 *PostgreSql*
- `--with-stlport=<base directory>`：使用 *stlport* 替代标准的 *Gcc c++ STL* 进行编译。

当使用 *SUNPro* 编译器进行编译时，需要修改 *MakeFile* 中的一些选项：

    CC=<full path to SUNPro CC binary>
    CFLAGS=-g -xs
    CXX=<full path to SUNPro CC binary>
    CXXFLAGS=-g -xs
    LD=<full path to SUNPro CC binary>
    LDFLAGS=-g -xs -lCstd
    AR=<full path to SUNPro CC binary>
    AR_FLAGS=-g -xs -xar -o

## 2. QuickFix的配置

<font color=Crimson>*Fix* 会话在 *QuickFix* 中由 `BeginString`(*Fix* 协议版本号)、`SenderCompID`(发起者的ID) 以及 `TargetCompID`(接收者的ID) 的唯一组合。</font>一个 *QuickFix* 的发起者或接收者可以维护多个 *Fix* 会话。

<font color=Crimson>*QuickFix* 是通过 `SessionSettings` 类来管理配置。</font>它可以从任何 *C++* 流（如文件流）中提取配置。

### 2.1. QuickFix配置的样例

initiator 的配置样例：

```ini
# default settings for sessions
[DEFAULT]
ConnectionType=initiator
ReconnectInterval=60
SenderCompID=TW

# session definition
[SESSION]
# inherit ConnectionType, ReconnectInterval and SenderCompID from default
BeginString=FIX.4.1
TargetCompID=ARCA
StartTime=12:30:00
EndTime=23:30:00
HeartBtInt=20
SocketConnectPort=9823
SocketConnectHost=123.123.123.123
DataDictionary=somewhere/FIX41.xml

[SESSION]
BeginString=FIX.4.0
TargetCompID=ISLD
StartTime=12:00:00
EndTime=23:00:00
HeartBtInt=30
SocketConnectPort=8323
SocketConnectHost=23.23.23.23
DataDictionary=somewhere/FIX40.xml

[SESSION]
BeginString=FIX.4.2
TargetCompID=INCA
StartTime=12:30:00
EndTime=21:30:00
# overide default setting for RecconnectInterval
ReconnectInterval=30
HeartBtInt=30
SocketConnectPort=6523
SocketConnectHost=3.3.3.3
# (optional) alternate connection ports and hosts to cycle through on failover
SocketConnectPort1=8392
SocketConnectHost1=8.8.8.8
SocketConnectPort2=2932
SocketConnectHost2=12.12.12.12
DataDictionary=somewhere/FIX42.xml
```

acceptor 的配置样例：

```ini
[DEFAULT]
ConnectionType=acceptor
ReconnectInterval=60
SenderCompID=arnice_ac
FileStorePath=/home/arnicedeng/work_space/quickfix/examples/executor/C++/store
FileLogPath=/home/arnicedeng/work_space/quickfix/examples/executor/C++/log

[SESSION]
# FileStorePath,FileLogPath,ConnectionType, ReconnectInterval and SenderCompID 等配置沿用 [DEFAULT] 的值
BeginString=FIX.4.2
TargetCompID=arnice_tgt
StartTime=00:00:00
EndTime=23:30:00
HeartBtInt=20
# acceptor 监听的端口
SocketAcceptPort=9823
DataDictionary=/data/home/arnicedeng/work_space/quickfix/spec/FIX42.xml
```

### 2.2. QuickFix配置的含义

#### 2.2.1. Session相关配置

| ID | 描述 | 合法的值 | 默认 |
| :--- | :--- | :--- | :--- |
| BeginString | 本条会话所采用的 Fix 协议的版本 | FIXT.1.1、FIX.4.4、FIX.4.3、FIX.4.2<br>FIX.4.1、FIX.4.0 | |
| SenderCompID | 与此 Fix 会话相关联发起者的 ID | 区分大小写的字母数字字符串 | |
| TargetCompID | 与此FIX会话关联的交易对手ID | 区分大小写的字母数字字符串 | |
| SessionQualifier | 为了消除相同会话所附加的限定符 | 区分大小写的字母数字字符串 | |
| DefaultApplVerID | 仅在FIXT 1.1(及更新版本)中需要(忽略较早的传输版本)。用于指定会话的缺省应用程序版本号。可以是ApplVerID enum(参见ApplVerID字段)或默认版本的BeginString。| FIX.5.0SP2、FIX.5.0SP1、FIX.5.0、FIX.4.4<br>FIX.4.3、FIX.4.2、FIX.4.1、FIX.4.0<br>9、8、7、6、5、4、3、2 | |
| ConnectionType | 定义会话是充当接受者还是发起者 | initiator、acceptor | |
| StartTime | 这个FIX会话被激活的时间 | 时间格式为HH:MM:SS，时间格式为UTC | |
| EndTime | 这个FIX会话失效的时间 | 时间格式为HH:MM:SS，时间格式为UTC | |
| StartDay | 对于维持时间长达一周的会话，会话在一周中的被激活的时间，配合 StartTime 使用 | 英文中星期几，使用任何缩写(即mo, mon, mond, monda, monday均有效) | |
| EndDay | 对于维持时间长达一周的会话，会话在一周中的失效的时间，配合 EndTime 使用 | 英文中星期几，使用任何缩写(即mo, mon, mond, monda, monday均有效) | |
| LogonTime | 会话在一天中进行 Logon 登录的时间 | 时间格式为HH:MM:SS，时间格式为UTC | 默认为 StartTime 的值 |
| LogoutTime | 会话在一天中进行 Logout 注销的时间 | 时间格式为HH:MM:SS，时间格式为UTC | 默认为 EndTime 的值 |
| LogonDay | 对于维持时间长达一周的会话，会话在一周中的进行登录的时间，配合 LogonTime 使用 | 英文中星期几，使用任何缩写(即mo, mon, mond, monda, monday均有效) | 默认为 StartDay 的值 |
| LogoutDay | 对于维持时间长达一周的会话，会话在一周中的注销的时间，配合 LogoutTime 使用 | 英文中星期几，使用任何缩写(即mo, mon, mond, monda, monday均有效) | 默认为 EndDay 的值 |
| UseLocalTime | 使用 localtime 代替 UTC 来转换 StartTime 和 EndTime。 消息中的时间仍将设置为UTC，这是FIX规范所要求的。| Y 或 N | 默认为 N |
| MillisecondsInTimeStamp | 确定是否应该将毫秒添加到时间戳中。 仅适用于FIX.4.2及更高版本。| Y 或 N | 默认为 Y |
| TimestampPrecision | 用于设置时间戳的小数部分。允许的值是0到9。 如果设置了，则覆盖milliseconds sintimestamp。| 0-9 | |
| SendRedundantResendRequests | 如果设置为Y, QuickFIX将发送所有必要的重新发送请求，即使它们看起来是多余的。当设置为N时，QuickFIX将尝试最小化重发请求(这在大容量系统中特别有用)。| Y 或 N | 默认为 N |
| ResetOnLogon | 确定在接收登录请求时是否应重置序列号。只能是 Acceptors 才能使用。| Y 或 N | 默认为 N |
| ResetOnLogout | 确定在正常注销终止后序列号是否应重置为1。| Y 或 N | 默认为 N |
| ResetOnDisconnect | 确定在异常终止后序列号是否应重置为1。| Y 或 N | 默认为 N |
| RefreshOnLogon | 确定登录时是否应该从持久层恢复会话状态。 用于创建热故障转移会话。| Y 或 N | 默认为 N |

#### 2.2.2. Validation(校验)相关配置

| ID | 描述 | 合法的值 | 默认 |
| :--- | :--- | :--- | :--- |
| UseDataDictionary | 告诉会话是否需要数据字典。如果使用重复组，则应该始终使用数据字典。| Y 或 N | 默认为 Y |
| DataDictionary | XML定义文件，用于验证传入的FIX消息。如果没有提供DataDictionary，则只执行基本的消息验证。<br><br>此设置只应用于高于FIX .1.1的FIX传输版本。FIXT.1.1设置请参见 TransportDataDictionary 和AppDataDictionary。| 有效的XML数据字典文件，QuickFIX在spec目录中提供了以下默认值：<br>FIX44.xml、FIX43.xml、FIX42.xml、FIX41.xml、FIX40.xml | |
| TransportDataDictionary | 用于验证管理(传输)消息的XML定义文件。此设置仅对fix .1.1(或更新的)会话有效。<br><br>请参阅 DataDictionary 了解旧的传输版本(FIX.4.0 - FIX.4.4)以获得更多信息。| 有效的XML数据字典文件，QuickFIX在spec目录中提供了以下默认值：FIXT1.1.xml | |
| AppDataDictionary | 用于验证应用程序消息的XML定义文件。此设置仅对fix .1.1(或更新的)会话有效（请参阅 DataDictionary 了解旧的传输版本(FIX.4.0 - FIX.4.4)以获得更多信息）。<br><br>该设置支持为每个会话定制应用程序数据字典。此设置仅用于FIXT 1.1和新的传输协议。<br><br>这个设置可以用作前缀，为FIXT传输指定多个应用程序字典，例如：<br><pre><code>DefaultApplVerID=FIX.4.2</code><br><code># For default application version ID</code><br><code>AppDataDictionary=FIX42.xml</code><br><code># For nondefault application version ID</code><br><code># Use BeginString suffix for app version</code><br><code>AppDataDictionary.FIX.4.4=FIX44.xml</code></pre> | 有效的XML数据字典文件，QuickFIX在spec目录中提供了以下默认值：FIX50SP2.xml、FIX50SP1.xml、FIX50.xml<br>FIX44.xml、FIX43.xml、FIX42.xml、FIX41.xml、FIX40.xml | |
| ValidateLengthAndChecksum | 如果设置为N，长度或校验和字段不正确的消息将不会被拒绝。您还可以使用它强制接受没有数据字典的重复组，在这种情况下，您将无法访问所有重复组。| Y 或 N | 默认为 Y |
| ValidateFieldsOutOfOrder | 如果设置为N，无序的字段(即消息头中的正文字段或正文中的报头字段)将不会被拒绝。用于连接到没有正确排序字段的系统。| Y 或 N | 默认为 Y |
| ValidateFieldsHaveValues | 如果设置为N，没有值(空)的字段将不会被拒绝。用于连接不正确发送空标签的系统。| Y 或 N | 默认为 Y |
| ValidateUserDefinedFields | 如果设置为N，用户定义的字段不会被拒绝，如果它们没有在数据字典中定义，或者在它们不属于的消息中存在。| Y 或 N | 默认为 Y |
| PreserveMessageFieldsOrder | 是否保留主要传出消息体中的字段顺序(如配置文件中定义的那样)。默认值:只保留指定顺序的组。| Y 或 N | 默认为 N |
| CheckCompID | 如果设置为Y，则必须从具有正确的sendcompid和TargetCompID的交易方接收消息。有些系统会根据设计发送不同的compid，因此必须将其设置为N。| Y 或 N | 默认为 Y |
| CheckLatency | 如果设置为Y，消息必须在定义的秒数内从对方接收(见MaxLatency)。如果系统使用 localtime 而不是 GMT 作为时间戳，则关闭此选项是有用的。| Y 或 N | 默认为 Y |
| MaxLatency | 如果CheckLatency设置为Y，这将定义允许处理消息的秒延迟数。 默认是120。| 正整数 | 120 |

#### 2.2.3. Initiator相关配置

| ID | 描述 | 合法的值 | 默认 |
| :--- | :--- | :--- | :--- |
| ReconnectInterval | 重新连接的间隔时间(以秒为单位)。只能被用于 initiator。| 正整数 | 30 |
| HeartBtInt | 心跳间隔(秒)。只能被用于 initiator | 正整数 | |
| LogonTimeout | 断开连接前等待登录响应的秒数。| 正整数 | 10 |
| LogoutTimeout | 断开连接前等待注销响应的秒数。| 正整数 | 2 |
| SocketConnectPort | 连接到会话的套接字端口。 仅用于SocketInitiator | 正整数 | |
| SocketConnectHost | 要连接的主机。 仅用于SocketInitiator | 一个域名 或 合法的 ip 地址 | |
| SocketConnectPort\<n\> | 备用的套接字端口，发生故障时使用。其中 n 为正整数（例如：SocketConnectPort1 SocketConnectPort2……）<br><br>必须是连续的，并且与 SocketConnectHost[n] 一一对应 | 正整数 | |
| SocketConnectHost\<n\> | 备用的套接字主机，发生故障时使用。其中 n 为正整数（例如：SocketConnectHost1 SocketConnectHost2……）<br><br>必须是连续的，并且与 SocketConnectPort[n] 一一对应 | 域名 或 合法的 ip 地址 | |
| SocketNodelay | 指示应该使用TCP_NODELAY创建套接字。目前，必须在 [DEFAULT] 部分定义。| Y 或 N | 默认为 N |
| SocketSendBufferSize | SO_SNDBUF的大小。目前，必须在 [DEFAULT] 部分定义。| 正整数 | 0 |
| SocketReceiveBufferSize | SO_RCVBUF大小。目前，必须在 [DEFAULT] 部分定义。| 正整数 | 0 |

#### 2.2.4. Acceptor相关配置

| ID | 描述 | 合法的值 | 默认 |
| :--- | :--- | :--- | :--- |
| SocketAcceptPort | 用于侦听传入连接的套接字端口，仅用于SocketAcceptor | 正整数，有效的开放套接字端口。目前，必须在 [DEFAULT] 部分定义。| |
| SocketReuseAddress | 表示套接字应该使用SO_REUSADDR创建，仅与SocketAcceptor一起使用 | Y 或 N | Y |
| SocketNodelay | 指示应该使用TCP_NODELAY创建套接字。目前，必须在 [DEFAULT] 部分定义。| Y 或 N | 默认为 N |

#### 2.2.5. 存储相关配置

| ID | 描述 | 合法的值 | 默认 |
| :--- | :--- | :--- | :--- |
| PersistMessages | 如果设置为N，则不会持久化任何消息。这将强制QuickFIX总是发送GapFills，而不是重新发送消息。(对市场数据流有用) | Y 或 N | 默认为 Y |
| <font color=Crimson>存储到 File</font> | | | |
| FileStorePath | 存储序列号和消息文件的目录。| 存放文件的有效目录，必须具有写访问权限 | |
| <font color=Crimson>存储到 Mysql</font> | | | |
| MySQLStoreDatabase | 要访问的MySQL数据库名称，用于存储消息和会话状态。| 有效的数据库存储文件，必须有写访问和正确的DB shema | quickfix |
| MySQLStoreUser | 登录MySQL数据库的用户名。| 对数据库中适当表具有读/写访问权限的有效用户 | root |
| MySQLStorePassword | 用户密码 | 正确的 Mysql 数据库的用户密码 | 空密码 |
| MySQLStoreHost | mysql 数据库的地址 | 合法的 ip 或 域名 | localhost |
| MySQLStorePort | mysql 数据库的端口 | 正整数 |  Mysql 数据库默认的端口 |
| MySQLStoreUseConnectionPool | 使用数据库连接池。如果可能，会话将共享单个数据库连接。否则，每个会话将获得自己的连接。| Y 或 N | N |
| <font color=Crimson>存储到 PostgreSql</font>| | | |
| PostgreSQLStoreDatabase | 要访问的PostgreSQL数据库名称，用于存储消息和会话状态。| 有效的数据库存储文件，必须有写访问和正确的DB shema | quickfix |
| PostgreSQLStoreUser | 登录PostgreSQL的用户名。| 对数据库中适当表具有读/写访问权限的有效用户 | postgres |
| PostgreSQLStorePassword | 用户密码 | 正确的 PostgreSql 数据库的用户密码 | 空密码 |
| PostgreSQLStoreHost | PostgreSQL 数据库的地址 | 合法的 ip 或域名 | localhost |
| PostgreSQLStorePort | PostgreSQL 数据库的端口 | 正整数 | PostgreSQL 数据库默认的监听端口 |
| PostgreSQLStoreUseConnectionPool | 使用数据库连接池。如果可能，会话将共享单个数据库连接。否则，每个会话将获得自己的连接。| Y 或 N | N |
|<font color=Crimson>存储到 ODBC</font> | | | |
| OdbcStoreUser | 登录ODBC数据库的用户名。| 对数据库中适当表具有读/写访问权限的有效用户。 如果UID在OdbcStoreConnectionString中，则忽略。| sa |
| OdbcStorePassword | 用户密码 | 正确用户的ODBC密码。 如果PWD在OdbcStoreConnectionString中，则忽略 | 空密码 |
| OdbcStoreConnectionString | 用于连接 ODBC 数据库的字符串 | 合法的 ODBC 连接字符串 | DATABASE=quickfix;DRIVER={SQL Server};SERVER=(local); |

#### 2.2.6. 日志相关

| ID | 描述 | 合法的值 | 默认 |
| :--- | :--- | :--- | :--- |
| <font color=Crimson>存储到 File</font> | | | |
| FileLogPath | 存放日志的目录。| 存放文件的有效目录，必须具有写访问权限  | |
| FileLogBackupPath | 备份日志的存放路径。| 存放备份文件的有效目录，必须具有写权限 | |
| <font color=Crimson>输出到屏幕</font> | | | |
| ScreenLogShowIncoming | 打印收到的消息 | Y 或 N | 默认为 Y |
| ScreenLogShowOutgoing | 打印发出的消息 | Y 或 N | 默认为 Y |
| ScreenLogShowEvents | 打印事件信息 | Y 或 N | 默认为 Y |
| <font color=Crimson>存储到 Mysql</font> | | | |
| MySQLLogDatabase | 要访问的MySQL数据库的名称。| 有效的数据库存储文件，必须有写访问和正确的DB shema | quickfix |
| MySQLLogUser | 登录MySQL数据库的用户名。| 对数据库中适当表具有读/写访问权限的有效用户 | root |
| MySQLLogPassword | 用户密码 | 正确的用户密码 | 空密码 |
| MySQLLogHost | 数据库地址 | 合法的 ip 或域名 | localhost |
| MySQLLogPort | 数据库端口 | 正整数 | Mysql监听的默认端口 |
| MySQLLogUseConnectionPool | 使用数据库连接池。如果可能，会话将共享单个数据库连接。 否则，每个会话将获得自己的连接。| Y 或 N | 默认为 N |
| MySQLLogIncomingTable | 记录收到的消息的表的名称。| 具有正确模式的有效表。 | messages_log |
| MySQLLogOutgoingTable | 记录发出的消息的表的名称 | 具有正确模式的有效表。 | messages_log |
| MySQLLogEventTable | 记录事件消息的表的名称 | 具有正确模式的有效表。| event_log |
| <font color=Crimson>存储到 PostgreSql</font>| | | |
| PostgreSQLLogDatabase | 用于日志记录访问的PostgreSQL数据库名称 | 有效的数据库存储文件，必须有写访问和正确的DB shema | quickfix |
| PostgreSQLLogUser | 登录PostgreSQL的用户名。| 对数据库中适当表具有读/写访问权限的有效用户 | postgres |
| PostgreSQLLogPassword | 用户密码 | 正确的 PostgreSql 数据库的用户密码 | 空密码 |
| PostgreSQLLogHost | PostgreSQL 数据库的地址 | 合法的 ip 或域名 | localhost |
| PostgreSQLLogPort | PostgreSQL 数据库的端口 | 正整数 | PostgreSQL 数据库默认的监听端口 |
| PostgreSQLLogUseConnectionPool | 使用数据库连接池。如果可能，会话将共享单个数据库连接。否则，每个会话将获得自己的连接。| Y 或 N | N |
| PostgresSQLLogIncomingTable | 记录收到的消息的表的名称。| 具有正确模式的有效表。 | messages_log |
| PostgresSQLLogOutgoingTable | 记录发出的消息的表的名称 | 具有正确模式的有效表。 | messages_log |
| PostgresSQLLogEventTable | 记录事件消息的表的名称 | 具有正确模式的有效表。| event_log |
|<font color=Crimson>存储到 ODBC</font> | | | |
| OdbcLogUser | 登录ODBC数据库的用户名。| 对数据库中适当表具有读/写访问权限的有效用户。 如果UID在OdbcStoreConnectionString中，则忽略。| sa |
| OdbcLogPassword | 用户密码 | 正确用户的ODBC密码。 如果PWD在OdbcStoreConnectionString中，则忽略 | 空密码 |
| OdbcLogConnectionString | 用于连接 ODBC 数据库的字符串 | 合法的 ODBC 连接字符串 | DATABASE=quickfix;DRIVER={SQL Server};SERVER=(local); |
| OdbcLogIncomingTable | 记录收到的消息的表的名称。| 具有正确模式的有效表。 | messages_log |
| OdbcLogOutgoingTable | 记录发出的消息的表的名称 | 具有正确模式的有效表。 | messages_log |
| OdbcLogEventTable | 记录事件消息的表的名称 | 具有正确模式的有效表。| event_log |

#### 2.2.7. SSL相关配置

| ID | 描述 | 合法的值 | 默认 |
| :--- | :--- | :--- | :--- |
| SSLProtocol | 此指令可用于控制应用程序在建立其环境时应该使用的SSL协议风格。可用的(不区分大小写的)协议有: <br><br>SSLv2<br>这是2.0版本的安全套接字层(SSL)协议，它是由Netscape公司设计的最初的SSL协议。<br><br>SSLv3<br>这是安全套接字层(SSL)协议的3.0版本。它是SSLv2和Netscape公司目前(截至1999年2月)事实上标准化的SSL协议的后继者。几乎所有流行的浏览器都支持它。<br><br>TLSv1<br>这是传输层安全(TLS)协议，1.0版本。<br><br>TLSv1_1<br>这是1.1版本的传输层安全(TLS)协议。<br><br>TLSv1_2<br>这是1.2版本的传输层安全(TLS)协议。<br><br>all<br>这是 +SSLv2 +SSLv3 +TLSv1 +TLSv1_1 +TLSv1_2的快捷方式，也是启用除一个协议以外的所有协议的方便方式，当与协议上的减号结合使用时，如上例所示。例子：<br><pre><code>enable all but not SSLv2</code><br><code>SSL_PROTOCOL = all -SSLv2</code></pre> | | all - SSLv2 |
| SSLCipherSuite | 这个复杂的指令使用一个冒号分隔的cipher-spec字符串，该字符串由OpenSSL密码规范组成，用于配置允许客户端在SSL握手阶段进行协商的密码套件。注意，这个指令可以在每个服务器和每个目录上下文中使用。在每个服务器上下文中，它适用于建立连接时的标准SSL握手,它会在读取HTTP请求之后，但在发送HTTP响应之前，强制使用重新配置的加密套件进行SSL重协商。<br><br>cipher-spec中的SSL密码规范由4个主要属性加上一些次要属性组成。<br><br>密钥交换算法: RSA or Diffie-Hellman variants.<br>身份验证算法:RSA, Diffie-Hellman, DSS or none.<br>密码/加密算法:DES, Triple-DES, RC4, RC2, IDEA or none. <br>MAC消化算法: MD5, SHA or SHA1.<br><br>更多细节请参考mod_ssl文档。例如: RC4+RSA:+HIGH:<br> | | HIGH:!RC4 |
| CertificationAuthoritiesFile | 该指令设置了一个一体化文件，您可以在该文件中组装与您处理客户端的证书颁发机构(CA)。 | | |
| CertificationAuthoritiesDirectory | 此指令设置保存与您处理客户端的证书颁发机构证书(CAs)的目录。| | |
|<font color=Crimson>Acceptor 相关</font> | | | |
| ServerCertificateFile | 这个指令指向pem编码的证书文件，也可以选择指向对应的RSA或DSA私钥文件(包含在同一个文件中)。| | |
| ServerCertificateKeyFile | 这个指令指向pem编码的私钥文件。 如果私钥没有与服务器证书文件中的证书结合，则使用此附加指令指向具有独立私钥的文件。| | |
| CertificateVerifyLevel | 此指令设置证书验证级别。它适用于建立连接时在标准SSL握手中使用的身份验证过程。 0表示不验证。 1意味着验证。| | |
| CertificateRevocationListFile | 该指令设置了一个一体化文件，您可以在该文件中组装与您处理客户端的证书颁发机构(CA)的证书撤销列表(CRL)。| | |
| CertificateRevocationListDirectory | 此指令设置保存与您处理客户端的证书颁发机构(CAs)的证书撤销列表(CRL)的目录。| | |
|<font color=Crimson>Initiator 相关</font> | | | |
| ClientCertificateFile | 这个指令指向pem编码的证书文件，也可以选择指向对应的RSA或DSA私钥文件(包含在同一个文件中)。| | |
| ClientCertificateKeyFile | 这个指令指向pem编码的私钥文件。 如果私钥没有与服务器证书文件中的证书结合，则使用此附加指令指向具有独立私钥的文件。| | |

#### 2.2.8. 其他

| ID | 描述 | 合法的值 | 默认 |
| :--- | :--- | :--- | :--- |
| HttpAcceptPort | 监听HTTP请求的端口，将浏览器指向这个端口将打开一个控制面板。<br><br>必须在 [DEFAULT] 段进行配置 | 正整数 | |

## 3. Fix应用程序

Fix应用程序的作用：<font color=Crimson>让用户可以接收会话中的各种事件、管理消息以及应用消息；并自定义处理这些消息与事件的方式。需要结合 `FIX::SocketAcceptor` 或 `FIX::SocketInitialor` 来使用</font>

### 3.1. Fix::Application接口

创建 Fix 应用程序只需要继承并实现 `Fix::Application` 接口即可，`Fix::Application` 接口代码如下（位于 src/c++/Application.h 中）：

```c++
//! 以下的接口都是回调接口，用于在 Fix 会话中通知某些事件的发生
class Application
{
 public:
    virtual ~Application() {};
    /// 当quickfix创建新会话时调用。
    virtual void onCreate( const SessionID& ) = 0;
        
    /// 当会话登录成功后进行通知
    virtual void onLogon( const SessionID& ) = 0;

    /// 当会话成功登出后进行通知
    virtual void onLogout( const SessionID& ) = 0;
        
    /// 向目标发送管理消息后进行的通知 
    virtual void toAdmin( Message&, const SessionID& ) = 0;
  
    /// 向目标发送应用消息后进行的通知
    virtual void toApp( Message&, const SessionID& ) EXCEPT ( DoNotSend ) = 0;
        
    /// 从目标收到管理消息后进行的通知 
    virtual void fromAdmin( const Message&, const SessionID& )EXCEPT ( FieldNotFound, IncorrectDataFormat, IncorrectTagValue, RejectLogon ) = 0;

    /// 从目标收到应用消息后进行的通知 
    virtual void fromApp( const Message&, const SessionID& ) EXCEPT ( FieldNotFound, IncorrectDataFormat, IncorrectTagValue, UnsupportedMessageType ) = 0;
};
```

回调方法的作用：

- `onCreate`：当quickfix创建新会话时调用。<font color=DeepPink>会话一旦创建，将应用程序的整个生命周期内保持存在。不管对方是否连接到会话，会话都存在。一旦创建了会话，就可以开始向它发送消息。如果没有人登录，则消息将在与对方建立连接时发送。</font>
- `onLogon`：当建立连接并完成FIX登录过程(双方交换有效的登录消息)时调用该函数。
- `onLogout`：当某个 FIX 会话不再在线时进行通知，这可能发生在正常的注销交换过程中，或者由于强制终止或网络连接丢失。
- `toAdmin`：使您可以了解从FIX引擎发送到交易方的管理消息。这通常对应用程序没有用处，但它可以让你进行与管理消息相关的日志记录。<font color=DeepPink>注意：FIX::Message 不是常量，这允许你在发送管理消息之前向其添加字段。</font>
- `toApp`：正在发送给对手方的应用程序消息时进行的回调。<font color=DeepPink>如果在这个函数中抛出一个 `DoNotSend` 异常，应用程序将不会发送消息。这可用于取消重新发送一些不必要的消息，比如与当前市场不再相关的订单。注意：FIX::Message 不是常量，这允许你在发送管理消息之前向其添加字段。</font>
  1. 抛出 `DoNotSend` 异常并将消息的 `PossDupFlag` 设置为 true：a sequence reset will be sent in place of the message.（序列号将被重置为这个要发送的消息的序列号？）
  2. 抛出 `DoNotSend` 异常并将消息的 `PossDupFlag` 设置为 false：不会发送消息。
- `fromAdmin`：当管理消息从交易方发送到 FIX 引擎时通知您。这对于对登录消息(如验证密码)进行额外的验证非常有用。<font color=DeepPink>在函数中抛出 `RejectLogon` 异常将断开对方的连接。</font>
- `fromApp`：接收应用程序级请求。如果您的应用程序是一个卖方OMS，您将从函数中获得的新订单请求；如果你是买方，你会在这里拿到你的执行报告。
  - <font color=DeepPink>如果抛出 `FieldNotFound` 异常，对方将收到一个 `Reject消息`，表示缺少一个条件要求的字段。当试图检索丢失的字段时，Message 类将抛出此异常，因此很少需要显式地抛出。</font>
  - <font color=DeepPink>如果抛出 `UnsupportedMessageType` 异常，对方将收到一个 `Reject消息`，通知他们您的应用程序无法处理这些类型的消息。</font>
  - <font color=DeepPink>如果字段包含您不支持的值，也会抛出 `IncorrectTagValue`。</font>

<font color=Crimson>不能保证是否会在多个线程中回调这些方法。如果应用程序在多个会话之间共享资源，则必须同步这些资源；可以使用 `SynchronizedApplication` 类来自动同步应用程序中的所有函数调用，各种 `MessageCracker` 类可用于将通用消息结构解析为特定的 %FIX 消息。</font>

### 3.2. FIX::SynchronizedApplication实现

`FIX::SynchronizedApplication` 接口与 `FIX::Application` 接口的区别在于：通过 `Mutex` 在回调的时候保证这一次只有一个线程访问应用程序的代码，这样会导致加锁的粒度太大，对性能有影响！代码如下：

```c++
class SynchronizedApplication : public Application
{
 public:
    SynchronizedApplication( Application& app ) : m_app( app ) {}

    void onCreate( const SessionID& sessionID )
    { Locker l( m_mutex ); app().onCreate( sessionID ); }
  
    void onLogon( const SessionID& sessionID )
    { Locker l( m_mutex ); app().onLogon( sessionID ); }
  
    void onLogout( const SessionID& sessionID )
    { Locker l( m_mutex ); app().onLogout( sessionID ); }
  
    void toAdmin( Message& message, const SessionID& sessionID )
    { Locker l( m_mutex ); app().toAdmin( message, sessionID ); }
    
    void toApp( Message& message, const SessionID& sessionID ) EXCEPT ( DoNotSend )
    { Locker l( m_mutex ); app().toApp( message, sessionID ); }
  
    void fromAdmin( const Message& message, const SessionID& sessionID ) EXCEPT ( FieldNotFound, IncorrectDataFormat, IncorrectTagValue, RejectLogon )
    { Locker l( m_mutex ); app().fromAdmin( message, sessionID ); }
  
    void fromApp( const Message& message, const SessionID& sessionID ) EXCEPT ( FieldNotFound, IncorrectDataFormat, IncorrectTagValue, UnsupportedMessageType )
    { Locker l( m_mutex ); app().fromApp( message, sessionID ); }

    Mutex m_mutex;

    Application& app() { return m_app; }
    Application& m_app;
};
```

> 注：另外，m_app 采用了多态特性，需要额外的虚拟表查找，会造成非常小的性能损失。

### 3.3. FIX::NullApplication实现

`FIX::NullApplication` 接口主要用于不想要实现所有回调接口的场景中，比如想要对某些接口进行单元测试时，可以继承该类。该类的代码如下：

```c++
class NullApplication : public Application
{
    void onCreate( const SessionID& ) {}
    void onLogon( const SessionID& ) {}
    void onLogout( const SessionID& ) {}
    void toAdmin( Message&, const SessionID& ) {}

    void toApp( Message&, const SessionID& ) 
        EXCEPT ( DoNotSend ) {}
    void fromAdmin( const Message&, const SessionID& )
        EXCEPT ( FieldNotFound, IncorrectDataFormat, IncorrectTagValue, RejectLogon ) {}
    void fromApp( const Message&, const SessionID& )
        EXCEPT ( FieldNotFound, IncorrectDataFormat, IncorrectTagValue, UnsupportedMessageType ) {}
};
```

### 3.4. Fix创建应用程序的样例

```c++
#include "quickfix/FileStore.h"
#include "quickfix/FileLog.h"
#include "quickfix/SocketAcceptor.h"
#include "quickfix/Session.h"
#include "quickfix/SessionSettings.h"
#include "quickfix/Application.h"

int main( int argc, char** argv )
{
    try
    {
        if(argc < 2) return 1;
        std::string fileName = argv[1];

        FIX::SessionSettings settings(fileName);

        MyApplication application;
        FIX::FileStoreFactory storeFactory(settings);
        FIX::FileLogFactory logFactory(settings);
        FIX::SocketAcceptor acceptor
            (application, storeFactory, settings, logFactory /*optional*/);
        acceptor.start();
        // while( condition == true ) { do something; }
        acceptor.stop();
        return 0;
    }
    catch(FIX::ConfigError& e)
    {
        std::cout << e.what();
        return 1;
    }
}
```

## 4. QuickFix消息

### 4.1. 构造消息

构造消息的方法：

1. 通过构造函数传参的形式，构造消息。<font color=DeepPink>这种方法可以正确填入消息所需的必要字段</font>
2. 通过默认构造函数创建消息对象，通过 `setField` 方法添加字段。<font color=DeepPink>这种方法容易漏填字段</font>

样例：

```c++
// 安全：能确保填入所有的消息必须的字段
void sendOrderCancelRequest()
{
    FIX41::OrderCancelRequest message(
        FIX::OrigClOrdID("123"),
        FIX::ClOrdID("321"),
        FIX::Symbol("LNUX"),
        FIX::Side(FIX::Side_BUY));

    message.set(FIX::Text("Cancel My Order!"));
    FIX::Session::sendToTarget(message, SenderCompID("TW"), TargetCompID("TARGET"));
}

// 不安全，容易漏填
void sendOrderCancelRequest()
{
    FIX::Message message;
    FIX::Header header& = message.getHeader();

    header.setField(FIX::BeginString("FIX.4.2"));
    header.setField(FIX::SenderCompID(TW));
    header.setField(FIX::TargetCompID("TARGET"));
    header.setField(FIX::MsgType(FIX::MsgType_OrderCancelRequest));

    message.setField(FIX::OrigClOrdID("123"));
    message.setField(FIX::ClOrdID("321"));
    message.setField(FIX::Symbol("LNUX"));
    message.setField(FIX::Side(FIX::Side_BUY));
    message.setField(FIX::Text("Cancel My Order!"));

    FIX::Session::sendToTarget(message);
}
```

#### 4.1.1. 存取消息中的域

设置消息中的域的方法：

1. 通过 FIX 库提供的对应的函数来进行构造指定的域，比如：`FIX::BeginString()`、`FIX::SenderCompID()` 等。<font color=DeepPink>不会出现 Tag 填错等情况</font>
2. 直接传入 Tag 跟 value。容易出错，不安全

样例：

```c++
// 不安全：容易填错 Tag。应该使用上述样例中的 "FIX::字段名" 方法
void sendOrderCancelRequest()
{
    FIX::Message message;
    // BeginString
    message.getHeader().setField(8, "FIX.4.2");
    // SenderCompID
    message.getHeader().setField(49, "TW");
    // TargetCompID, with enumeration
    message.getHeader().setField(FIX::FIELD::TargetCompID, "TARGET");
    // MsgType
    message.getHeader().setField(35, 'F');
    // OrigClOrdID
    message.setField(41, "123");
    // ClOrdID
    message.setField(11, "321");
    // Symbol
    message.setField(55, "LNUX");
    // Side, with value enumeration
    message.setField(54, FIX::Side_BUY);
    // Text
    message.setField(58, "Cancel My Order!");

    FIX::Session::sendToTarget(message);
}

// 安全的
void sendOrderCancelRequest()
{
    FIX41::OrderCancelRequest message(
        FIX::OrigClOrdID("123"),
        FIX::ClOrdID("321"),
        FIX::Symbol("LNUX"),
        FIX::Side(FIX::Side_BUY));

    message.set(FIX::Text("Cancel My Order!"));
    FIX::Session::sendToTarget(message, SenderCompID("TW"), TargetCompID("TARGET"));
}
```

通过 `getField()` 方法可以获取到消息中的域，该方法有以下两种调用方法：

1. 传入域对应的类，如：`Fix::Price` 、`Fix::ClOrdID`。<font color=DeepPink>推荐的方式，这种方式能安全地获取到指定的域</font>
2. 直接传入 Tag 数字。<font color=DeepPink>不安全的，不推荐</font>

样例：

```c++
// 推荐的方式
void fromApp( const FIX::Message& message, const FIX::SessionID& sessionID )
    throw( FIX::FieldNotFound&, FIX::IncorrectDataFormat&, FIX::IncorrectTagValue&, FIX::UnsupportedMessageType& )
{
    // retrieve value into field class
    FIX::Price price;
    message.getField(price);

    // another field...
    FIX::ClOrdID clOrdID;
    message.getField(clOrdID);
}

// 不推荐的方式
void fromApp( const FIX::Message& message, const FIX::SessionID& sessionID )
    throw( FIX::FieldNotFound&, FIX::IncorrectDataFormat&, FIX::IncorrectTagValue&, FIX::UnsupportedMessageType& )
{
    // retreive value into string with integer field ID
    std::string value;
    value = message.getField(44);

    // retrieve value into a field base with integer field ID
    FIX::FieldBase field(44, "");
    message.getField(field);

    // retreive value with an enumeration, a little better
    message.getField(FIX::FIELD::Price);
}
```

#### 4.1.2. 获取消息的标准头跟标准尾

- `FIX::Message::getHeader()` 方法可以获取消息的标准头
- `FIX::Message::getTrailer()` 方法可以获取消息的标准尾

通过 `setField()` 方法可以添加或修改标准头或标准尾的中的域。

### 4.2. 发送消息

#### 4.2.1. 发送消息的方法

可以使用静态 `Session::sendToTarget` 方法将消息发送给对手方：

```c++
// 发送一个已经包含 BeginString, sendcompid 和 TargetCompID 的消息 
static bool sendToTarget( Message&, const std::string& qualifier = "" )
    throw(SessionNotFound&);

// 发送基于sessionID的消息，方便使用 
// 在 fromApp 回调中，会传入了一个会话ID
static bool sendToTarget( Message&, const SessionID& )
    throw(SessionNotFound&);

// 在发送之前追加一个sendcompid和TargetCompID 
static bool sendToTarget( Message&, const SenderCompID&, const TargetCompID&, const std::string& qualifier = "" )
    throw(SessionNotFound&);

// 将sendcompid和TargetCompID作为字符串传递 
static bool sendToTarget( Message&, const std::string&, const std::string&, const std::string& qualifier = "" )
    throw(SessionNotFound&);
```

### 4.3. 接收与处理消息

#### 4.3.1. 接收应用消息

如何接收应用消息：通过 `FIX::Application` 跟 `FIX::SocketAcceptor` 或 `FIX::SocketInitialor`

1. 重载 `FIX::Application` 的 `fromApp` 方法
2. 将重载后的 `Fix::Application` 对象传递给 `FIX::SocketAcceptor` 或 `FIX::SocketInitialor` 即可，收到应用消息时会回调 `fromApp`，从而获得应用消息并进行处理。

#### 4.3.2. 处理应用消息

通过重载 `MessageCracker` 的 `onMessage()` 方法来实现对消息的处理。

> 注1：<font color=DeepPink>QuickFix 为Fix 标准协议中的定义的所有消息都提供了一个类；所以 `MessageCracker` 中为每种消息都提供了一个 `onMessage()` 方法用于进行处理对应的消息。</font>

处理消息的流程：

1. 重载 `FIX::MessageCracker`，实现需要处理的消息对应的 `onMessage()` 方法
2. 在接收消息的 `Fix::Application` 的 `fromApp()` 方法中，调用 `MessageCracker` 的 `crack()` 方法。<font color=DeepPink>该方法会根据消息的 `MsgType` 域，将 `Message` 对象强制转换成具体的消息对象，然后调用 `onMessage` 进行处理。</font>

样例：

```c++
// 继承FIX::Application 跟 FIX::MessageCracker
#include "quickfix/Application.h"
#include "quickfix/MessageCracker.h"

class MyApplication
    : public FIX::Application,
        public FIX::MessageCracker
{};

void MyApplication::fromApp( const FIX::Message& message, const FIX::SessionID& sessionID )
    throw( FIX::FieldNotFound&, FIX::IncorrectDataFormat&, FIX::IncorrectTagValue&, FIX::UnsupportedMessageType& )
{
    crack(message, sessionID);
}

void MyApplication::onMessage( const FIX42::NewOrderSingle& message, const FIX::SessionID& )
{
    FIX::ClOrdID clOrdID;
    message.get(clOrdID);

    FIX::ClearingAccount clearingAccount;
    message.get(clearingAccount);
}
```

> 注2：<font color=Crimson>任何未重载的函数将在默认情况下抛出 `UnsupportedMessageType` 异常</font>

由于不同版本的 Fix 标准协议定义的消息不一定一致，所以 QuickFix 为每个 Fix 标准协议版本都提供了一个 `MessageCracker` 以及每种消息的实现；这些类放在了对应的命名空间下，比如 Fix.4.2 版本，对应的命名空间就是 FIX42。<font color=DeepPink>`FIX::MessageCracker` 继承了每个 Fix 版本对应的 `MessageCracker` 对象，根据消息的 `BeginString` 域来识别 Fix 版本，然后调用指定版本的 `MessageCracker::crack()` 进行处理。</font>实现代码如下：

```c++
void FIX::MessageCracker::crack( const Message& message,
        const SessionID& sessionID,
        const BeginString& beginString )
{
    if ( beginString == BeginString_FIX40 )
        ((FIX40::MessageCracker&)(*this)).crack((const FIX40::Message&) message, sessionID);
    else if ( beginString == BeginString_FIX41 )
        ((FIX41::MessageCracker&)(*this)).crack((const FIX41::Message&) message, sessionID);
    else if ( beginString == BeginString_FIX42 )
        ((FIX42::MessageCracker&)(*this)).crack((const FIX42::Message&) message, sessionID);
    else if ( beginString == BeginString_FIX43 )
        ((FIX43::MessageCracker&)(*this)).crack((const FIX43::Message&) message, sessionID);
    else if ( beginString == BeginString_FIX44 )
        ((FIX44::MessageCracker&)(*this)).crack((const FIX44::Message&) message, sessionID);
    else if ( beginString == BeginString_FIXT11 )
    {
        if( message.isAdmin() )
        {
            ((FIXT11::MessageCracker&)(*this)).crack((const FIXT11::Message&) message, sessionID);
        }
        else
        {
            ApplVerID applVerID;
            if(!message.getHeader().getFieldIfSet(applVerID))
            {
                Session* pSession = Session::lookupSession( sessionID );
                applVerID = pSession->getSenderDefaultApplVerID();
            }

            crack( message, sessionID, applVerID );
        }
    }
}
```

#### 4.3.3. QuickFix重复组

QuickFIX能够发送包含重复组甚至递归重复组的消息。所有重复组都以一个字段开始，该字段表示一个集合中有多少重复组。

##### 4.3.3.1. 创建包含重复组的消息

创建消息时，声明重复组数量的必要字段被设置为零。当您添加组时，QuickFIX将自动为您增加字段。样例如下：

```c++
// create a market data message
FIX42::MarketDataSnapshotFullRefresh message(FIX::Symbol("QF"));

// repeating group in the form of “MessageName::NoField”
FIX42::MarketDataSnapshotFullRefresh::NoMDEntries group;
group.set(FIX::MDEntryType('0'));
group.set(FIX::MDEntryPx(12.32));
group.set(FIX::MDEntrySize(100));
group.set(FIX::OrderID("ORDERID"));
message.addGroup(group);

// 如果重用字段，则不需要创建新的组类
group.set(FIX::MDEntryType('1'));
group.set(FIX::MDEntryPx(12.32));
group.set(FIX::MDEntrySize(100));
group.set(FIX::OrderID("ORDERID"));
message.addGroup(group);
```

##### 4.3.3.2. 从消息中读取重复组的内容

1. 获得重复组的总数：通过该 `FIX::NoMDEntries` 与 `Message::get()` 获取
2. 获取每一个重复组：通过 `Message::getGroup()` 获取

样例如下：

```c++
// should be 2
FIX::NoMDEntries noMDEntries;
message.get(noMDEntries);

FIX42::MarketDataSnapshotFullRefresh::NoMDEntries group;
FIX::MDEntryType MDEntryType;
FIX::MDEntryPx MDEntryPx;
FIX::MDEntrySize MDEntrySize;
FIX::OrderID orderID;

message.getGroup(1, group);
group.get(MDEntryType);
group.get(MDEntryPx);
group.get(MDEntrySize);
group.get(orderID);

message.getGroup(2, group);
group.get(MDEntryType);
group.get(MDEntryPx);
group.get(MDEntrySize);
group.get(orderID);
```

## 5. QuickFix用户自定义字段

### 5.1. 安全地进行自定义字段

通过 `setField` 和 `getField` 来进行自定义字段的存储，不是类型安全的，如以下代码所示：

    message.setField(6123, "value");
    message.getField(6123);

因此，不建议使用上述方法来自定义字段，可以采用 QuickFix 提供的宏来创建类型安全的字段对象：

```c++
#include "quickfix/Field.h"

// 用户定义的字段必须在FIX名称空间中声明。
namespace FIX
{
    USER_DEFINE_STRING(MyStringField, 6123);
    USER_DEFINE_PRICE(MyPriceField, 8756);
}

// 使用自定义的字段
MyStringField stringField("string");
MyPriceField priceField(14.54);

message.setField(stringField);
message.setField(priceField);

message.getField(stringField);
message.getField(priceField);
```

### 5.2. 创建自定义字段的宏

这些宏允许您定义所有支持的FIX类型的字段：

```c++
USER_DEFINE_STRING( NAME, NUM )
USER_DEFINE_CHAR( NAME, NUM )
USER_DEFINE_PRICE( NAME, NUM )
USER_DEFINE_INT( NAME, NUM )
USER_DEFINE_AMT( NAME, NUM )
USER_DEFINE_QTY( NAME, NUM )
USER_DEFINE_CURRENCY( NAME, NUM )
USER_DEFINE_MULTIPLEVALUESTRING( NAME, NUM )
USER_DEFINE_EXCHANGE( NAME, NUM )
USER_DEFINE_UTCTIMESTAMP( NAME, NUM )
USER_DEFINE_BOOLEAN( NAME, NUM )
USER_DEFINE_LOCALMKTDATE( NAME, NUM )
USER_DEFINE_DATA( NAME, NUM )
USER_DEFINE_FLOAT( NAME, NUM )
USER_DEFINE_PRICEOFFSET( NAME, NUM )
USER_DEFINE_MONTHYEAR( NAME, NUM )
USER_DEFINE_DAYOFMONTH( NAME, NUM )
USER_DEFINE_UTCDATE( NAME, NUM )
USER_DEFINE_UTCTIMEONLY( NAME, NUM )
USER_DEFINE_NUMINGROUP( NAME, NUM )
USER_DEFINE_SEQNUM( NAME, NUM )
USER_DEFINE_LENGTH( NAME, NUM )
USER_DEFINE_PERCENTAGE( NAME, NUM )
USER_DEFINE_COUNTRY( NAME, NUM )
```

## 6. 验收测试

QuickFIX有一个可编写脚本的测试运行器，它附带一系列自动化验收测试。这些测试验证QuickFIX是否遵守FIX规范。

> 注：QuickFIX附带的基本测试是基于FIX协议组织生成的FIX会话级测试用例和预期行为文档。

建议的做法：利用这些测试来驱动 *QuickFix* 的开发。在编写支持协议特性的代码行之前，先编写其中一个测试。这种优先测试的方法为开发人员建立了一个目标，他们将客观地验证是否正确地实现了标准。

下面是一个测试脚本的例子，它测试引擎在接收到小于预期 `MsgSeqNum` 的 `NewSeqNo` 值时的行为：

    iCONNECT
    I8=FIX.4.235=A34=149=TW52=>TIME>56=ISLD98=0108=30
    E8=FIX.4.29=5735=A34=149=ISLD52=00000000-00:00:0056=TW98=0108=3010=0

    # sequence reset without gap fill flag (default to N)
    I8=FIX.4.235=434=049=TW52=>TIME>56=ISLD36=1
    E8=FIX.4.29=11235=334=249=ISLD52=00000000-00:00:0056=TW45=058=Value is incorrect (out of range) for this tag372=4373=510=0

    I8=FIX.4.235=134=249=TW52=>TIME>56=ISLD112=HELLO
    E8=FIX.4.29=5535=034=349=ISLD52=00000000-00:00:0056=TW112=HELLO10=0

    # sequence reset without gap fill flag (default to N)
    I8=FIX.4.235=434=049=TW52=>TIME>56=ISLD36=1123=N
    E8=FIX.4.29=11235=334=449=ISLD52=00000000-00:00:0056=TW45=058=Value is incorrect (out of range) for this tag372=4373=510=0

    I8=FIX.4.235=134=349=TW52=>TIME>56=ISLD112=HELLO
    E8=FIX.4.29=5535=034=549=ISLD52=00000000-00:00:0056=TW112=HELLO10=0
    iDISCONNECT

只要提供新的宏和可以将类型与字符串进行转换的转换器，就可以自定义任何类型的字段。

## 7. 消息验证

*QuickFix* 会在消息到达应用程序前对其进行验证，拒绝任何格式错误的消息。

*QuickFix* 是通过 XML 文件定义会话支持的消息、字段和值，通过配置：`DataDictionary` 进行配置。*QuickFix* 库的 *spec* 目录下包含了多个标准的 Fix 协议字典。Fix 协议字段的基本骨架如下：

```xml
<fix type="FIX" major="4" minor="1">
  <header>
    <field name="BeginString" required="Y"/>
    ...
  </header>
  <trailer>
    <field name="CheckSum" required="Y"/>
    ...
  </trailer>
  <messages>
    <message name="Heartbeat" msgtype="0" msgcat="admin">
      <field name="TestReqID" required="N"/>
    </message>
    ...
    <message name="NewOrderSingle" msgtype="D" msgcat="app">
      <field name="ClOrdID" required="Y"/>
      ...
    </message>
    ...
  </messages>
  <fields>
    <field number="1" name="Account" type="CHAR" />
    ...
    <field number="4" name="AdvSide" type="CHAR">
     <value enum="B" description="BUY" />
     <value enum="S" description="SELL" />
     <value enum="X" description="CROSS" />
     <value enum="T" description="TRADE" />
   </field>
   ...
  </fields>
</fix>
```

## 8. 测试
