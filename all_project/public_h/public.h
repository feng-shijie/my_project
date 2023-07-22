/*
*@desc:		public.h
*@author:	me
*@date:		2023.1.26
*/

#ifndef PUBLC_H
#define PUBLIC_H 1

//socket服务端
#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h>
#include<sys/time.h>
#include<sys/syscall.h>
#include<fstream>

/*********************日志系统*******************************/

#define gettid()	syscall(SYS_gettid)

//extern std::ofstream fp;	//外部变量声明如此使用

enum LOG_LEVEL	//日志级别
{
	LOG_LEVEL_INFO,		//信息
	LOG_LEVEL_WARNING,	//警告
	LOG_LEVEL_ERROR		//错误
};

//__FILE__::获取当前文件名, __FUNCTION__::获取当前函数名, __LINE__::获取当前所在行数, __VA_ARGS__::可变参数列表
#define LOG_INFO(...)           CIULOG::log(LOG_LEVEL_INFO,__FILE__,__FUNCTION__,__LINE__,__VA_ARGS__)
#define LOG_WARNING(...)        CIULOG::log(LOG_LEVEL_WARNING,__FILE__,__FUNCTION__,__LINE__,__VA_ARGS__)
#define LOG_ERROR(...)          CIULOG::log(LOG_LEVEL_ERROR,__FILE__,__FUNCTION__,__LINE__,__VA_ARGS__)

//日志系统
class CIULOG
{
private:
	CIULOG(){}
	~CIULOG();
	CIULOG(const CIULOG &ciulog);
	const CIULOG &operator = (const CIULOG &ciulog);

	//CIULOG() = delete;	//阻止构造
	//~CIULOG() = delete;	//阻止析构
	//CIULOG(const CIULOG& rhs) = delete;	//阻止拷贝
	//CIULOG& operator = (const CIULOG& rhs) = delete;	//阻止赋值

private:
	static bool		m_btoFile;	//日志写入文件还是控制台
	static bool		mutex_val;	//是否为多线程模式
	static LOG_LEVEL	m_nloglevel;	//日志级别
	static char		*fpname;	//日志文件名
	static CIULOG		*ciulog;	//唯一单实列对象指针
	static pthread_mutex_t	log_mutex;	//互斥锁
	static std::ofstream	fp;
public:
	static CIULOG *getInstance()	{return ciulog;}	//获取单列实例
	static void deleteInstance();	//释放单列实例
	//日志初始化,bool输出到文件还是控制台,bool长日志是否截断,PCTSTR统一字符串
	static void init_ciulog(bool btofile, bool bTruncateLongLog);
	static void setlevel(LOG_LEVEL level);	//修改日志级别
	static bool log(LOG_LEVEL level, const char* FileName, const char* FunctionName, int Row, const char* logstr,...);

};

#endif
