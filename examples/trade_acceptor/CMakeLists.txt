cmake_minimum_required(VERSION 2.8)

# 设置项目名称
set(THIS_PROJECT_NAME trade_acceptor)
project( ${THIS_PROJECT_NAME} "CXX")

# 设置 C++11 标准
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORT_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORT_CXX0X)
if(COMPILER_SUPPORT_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -m64 -DSUS_LINUX")
else()
    message(STATUS "the compiler ${CMAKE_CXX_COMPILER} not support C++11.")
endif()

################################################################################################
# debug 或 release 版本编译控制
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

if (CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -Wall")
    message("-- build mode debug: flag=${CMAKE_CXX_FLAGS}")
elseif(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Release"))
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall")
    message("-- build mode release: flag=${CMAKE_CXX_FLAGS}")
else()
    message("-- no CMAKE_BUILD_TYPE, can't build.")
endif()

#######################################################################################################
# 公共库相关的头文件路径
## quickfix库的头文件路径
set(QUICKFIX_BASE_PATH ${PROJECT_SOURCE_DIR}/../quickfix/)
set(QUICKFIX_INCLUDE_PATH ${QUICKFIX_BASE_PATH}/include/)
### quickfix 用到了mysql，以下是mysql路径
set(USR_INCLUDE_BASE_PATH /usr/include)
set(MYSQL_BASE_PATH ${USR_INCLUDE_BASE_PATH}/mysql/)

# 本程序相关的头文件路径
set(THIS_INCLUDE_PATH ${PROJECT_SOURCE_DIR}/src/application/)

#######################################################################################################
# 包含头文件搜索路径
include_directories(${QUICKFIX_INCLUDE_PATH})
include_directories(${THIS_INCLUDE_PATH})
include_directories(${USR_INCLUDE_BASE_PATH})
include_directories(${MYSQL_BASE_PATH})

#######################################################################################################
# 动态库、静态库搜索路径
link_directories(/usr/lib64/)

## quickfix相关库的搜索路径
link_directories(${QUICKFIX_BASE_PATH}/libs/)

#######################################################################################################
# 搜索源文件
aux_source_directory(${PROJECT_SOURCE_DIR}/src MAIN_SRC_DIR)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/application SUB_APPLICATION_SRC_DIR)

#######################################################################################################
# 链接程可执行程序
add_executable(${THIS_PROJECT_NAME} ${MAIN_SRC_DIR} ${SUB_APPLICATION_SRC_DIR})

## 指定可执行文件的后缀
set_target_properties(${THIS_PROJECT_NAME} PROPERTIES RELEASE_POSTFIX "_r")
set_target_properties(${THIS_PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "_d")
## 指定库文件的后缀
#set(CMAKE_DEBUG_POSTFIX "_d")
#set(CMAKE_RELEASE_POSTFIX "_r")

#######################################################################################################
# 链接动态库
## 链接 mysqlclient
target_link_libraries(${THIS_PROJECT_NAME} PUBLIC -lmysqlclient -lquickfix)

## 链接其他动态库
target_link_libraries(${THIS_PROJECT_NAME} PUBLIC -lz -lm -ldl -lssl -lcrypto)
