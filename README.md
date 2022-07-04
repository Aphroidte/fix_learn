# fix_learn

FIX 协议学习笔记

## 1. 总览

- [Fix协议学习笔记](doc/Fix协议学习笔记.md)
- [QuickFix学习笔记](doc/QuickFix学习笔记.md)
- [QuickFix源码学习笔记](doc/QuickFix源码阅读笔记.md)

## 2. 学习目标

### 2.1. Fix协议的学习目标

1. 掌握 Fix 协议中基础知识，比如：
   1. Fix 协议中消息的格式、消息的完整性校验
   2. Fix 协议标准的消息头以及标准的消息尾格式
   3. Fix 协议会话的基本概念：Fix 会话的组成、Fix 会话序列号的管理机制
2. 掌握 Fix 协议中的一些关键通信规范：
   1. 消息乱序的检测（分为两种）
   2. 如何处理消息乱序（消息的重传逻辑）
   3. 会话的建立与注销的过程
3. 掌握 Fix 协议七种管理消息的格式与作用

### 2.2. QuickFix的学习目标

1. 了解 QuickFix 的配置项及其含义
2. 掌握一个基于 QuickFix 库的程序的基本架构（如何基于 QuickFix 写出一个程序）[参考QuickFix 中的 demo]。主要是掌握如何收发消息
3. 熟悉 QuickFix 的源码
   1. QuickFix 配置的管理
   2. QuickFix 会话的管理
   3. QuickFix 会话序列号的管理
   4. QuickFix 消息的接收，重传的处理
   5. QuickFix 消息的发送
