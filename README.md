# 由于源码维护一直未更新在此fork分支进行共同管理
一般Dump收集分为采集和后台分析，Crashrpt和CrashFix天作之合，因此在这里成全他们在一起~也减少大家少采坑
代码是分别fork Crashrpt和CrashFix而来
# Dump介绍
Dump是专门解决Crash崩溃收集和分析的解决方案；
分为前端采集和后端分析模块
前端收集库采用CrashRpt、后端分析库采用CrashFix 在此记录！

# 平台

## CrashRpt:
* Windows operating system (Windows xp or later).
## CrashFix：
* Windows operating system (Windows Vista or later)./ Linux operating system.
* Apache HTTP Server (version 2.2 or later).
* PHP (version 5.3 or later).
* Database (MySQL).
* 2 GB of free disk space (to store crash reports and symbols).
* Network connection (either LAN or Internet).

# 官网：
## Crashrpt:
> [Crashrpt](http://crashrpt.sourceforge.net/docs/html/index.html)
## CrashFix:
> [CrashFix](http://crashfix.sourceforge.net/doc/html/index.html)
### Windows下安装
[Windows安装步骤](http://crashfix.sourceforge.net/doc/html/install_windows.html)
### Linux下安装
[Ubuntu14安装步骤](https://blog.csdn.net/aa13058219642/article/details/80431100)

# 目录结构

* --crashfix
	> 源码
* --crashrpt
	> 源码
* --Backup
	> 备份和生成文件
* --README.md
	> 简介
