/*
 * Easylog.h
 *
 *  Created on: 2018��12��7��
 *      Author: Administrator
 *      ֧�ַ�C++11,֧����־����֧����ʽ��־,֧��printf������Ҫ���壬ֻ��Ҫ����ͷ�ļ�
 *      ��лԭ����sollyu
 *      by zhushuo
 */

#ifndef EASYLOG_H_
#define EASYLOG_H_

//
//  EasyLog.h
//
//  Created by sollyu on 14/11/20.
//  Copyright (c) 2014�� sollyu. All rights reserved.
//

/**
	#include <iostream>
	#include "Easylog.h"

	ELOGI("i'm %s", "zhushuo");          // ��� INFO (ֻ�� LOGI �����ӡ������)
	ELOGE("I'm " << "sollyu");          // ��� ERROR (���ӡ������)
	ELOG_DEBUG("i'm %s", "sollyu");     // ��� DEBUG (���ӡ������)
	EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "i'm %s", "sollyu");

 // ��������ִ�н��
[2018.12.09] [19:48:41] [LOG_INFO ] ------------------ LOG SYSTEM START ------------------

[2018.12.09] [19:48:41] [LOG_INFO ] i'm zhushuo
[2018.12.09] [19:48:41] [LOG_ERROR] I'm sollyu (..\src\Easylog.cpp : main : 16 )
[2018.12.09] [19:48:41] [LOG_DEBUG] i'm sollyu (..\src\Easylog.cpp : main : 17 )
[2018.12.09] [19:48:41] [LOG_DEBUG] i'm sollyu

 */
 
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <functional>
#include <iomanip>
#include <stdarg.h>
#include <time.h>

#ifndef EASY_LOG_FILE_NAME
#  define EASY_LOG_FILE_NAME			"EasyLog.log"   /** ��־���ļ��� �������� ./log/EasyLog.logʵ�����·��*/
#endif

#ifndef EASY_LOG_FILE_NAME_DATE
#  define EASY_LOG_FILE_NAME_DATE		1            /** 1��ʾʹ��������Ϊ�ļ��� */
#endif

#ifndef EASY_LOG_LINE_BUFF_SIZE
#  define EASY_LOG_LINE_BUFF_SIZE		1024            /** һ�е���󻺳� */
#endif

#ifndef EASY_LOG_DISABLE_LOG
#  define EASY_LOG_DISABLE_LOG          0               /** ��0��ʾ����LOG */
#endif

#ifndef EASY_LOG_PRINT_LOG
#  define EASY_LOG_PRINT_LOG          0               /** ��0��ʾ���ô�ӡLOG */
#endif

#ifndef EASY_LOG_COVER_LOG
#  define EASY_LOG_COVER_LOG          1               /** ��0��ʾ׷��д */
#endif

#ifdef WIN32
#else
#   define  sprintf_s   sprintf
#   define  vsnprintf_s vsnprintf//�˴���Ϊ���õ���mingw��ֱ������vsnprintf���������ж�
#endif

/** д��־���� */
#define EWRITE_LOG(LEVEL, FMT, ...) \
{ \
    std::stringstream ss; \
    ss << FMT; \
    if (LEVEL != EasyLog::LOG_INFO) \
    { \
        ss << " (" << __FILE__ << " : " << __FUNCTION__ << " : " << __LINE__ << " )"; \
    } \
    EasyLog::GetInstance()->WriteLog(LEVEL, ss.str().c_str(), ##__VA_ARGS__); \
}

//! ���ٺ�
#define ELOG_TRACE(FMT , ...) EWRITE_LOG(EasyLog::LOG_TRACE, FMT, ##__VA_ARGS__)
#define ELOG_DEBUG(FMT , ...) EWRITE_LOG(EasyLog::LOG_DEBUG, FMT, ##__VA_ARGS__)
#define ELOG_INFO(FMT  , ...) EWRITE_LOG(EasyLog::LOG_INFO , FMT, ##__VA_ARGS__)
#define ELOG_WARN(FMT  , ...) EWRITE_LOG(EasyLog::LOG_WARN , FMT, ##__VA_ARGS__)
#define ELOG_ERROR(FMT , ...) EWRITE_LOG(EasyLog::LOG_ERROR, FMT, ##__VA_ARGS__)
#define ELOG_ALARM(FMT , ...) EWRITE_LOG(EasyLog::LOG_ALARM, FMT, ##__VA_ARGS__)
#define ELOG_FATAL(FMT , ...) EWRITE_LOG(EasyLog::LOG_FATAL, FMT, ##__VA_ARGS__)

#define ELOGT( FMT , ... ) ELOG_TRACE(FMT, ##__VA_ARGS__)
#define ELOGD( FMT , ... ) ELOG_DEBUG(FMT, ##__VA_ARGS__)
#define ELOGI( FMT , ... ) ELOG_INFO (FMT, ##__VA_ARGS__)
#define ELOGW( FMT , ... ) ELOG_WARN (FMT, ##__VA_ARGS__)
#define ELOGE( FMT , ... ) ELOG_ERROR(FMT, ##__VA_ARGS__)
#define ELOGA( FMT , ... ) ELOG_ALARM(FMT, ##__VA_ARGS__)
#define ELOGF( FMT , ... ) ELOG_FATAL(FMT, ##__VA_ARGS__)

// ============================================================
// time in 24 hours hh:mm:ss format
// ============================================================
static std::string TimeStamp()
{
	char str[9];

	// get the time, and convert it to struct tm format
	time_t a = time(0);
	//struct tm* b = localtime(&a);
	tm b;
	//struct tm* b;
	localtime_s(&b, &a);
	// print the time to the string
	strftime(str, 9, "%H:%M:%S", &b);

	return str;
}

// ============================================================
// date YYYY:MM:DD format
// ============================================================
static std::string DateStamp()
{
	char str[11];

	// get the time, and convert it to struct tm format
	time_t a = time(0);
	//struct tm* b = localtime(&a);//VS���������޸�
	//struct tm* b;
	tm b;
	localtime_s(&b, &a);
	// print the time to the string
	strftime(str, 11, "%Y.%m.%d", &b);

	return str;
}


class EasyLog
{
public:
    /** ��־����*/
	enum LOG_LEVEL { LOG_TRACE = 0, LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR, LOG_ALARM,  LOG_FATAL };

public:
    /** ����ģʽ */
    static EasyLog * GetInstance() { static EasyLog* m_pInstance = new EasyLog(); return m_pInstance; }
    //void EasyLogDestroy(){delete this;}//���ã�EasyLog::GetInstance()->EasyLogDestroy();��������ã�destroy���ٴε��û������ÿ��д��־�Ѿ�flush�ˣ����Բ�̫��Ҫ�����Ҫ���ã��뱣֤�������

public:


    /** д��־���� */
	void WriteLog(LOG_LEVEL level, const char *pLogText, ...)
	{
		va_list args;
		char logText[EASY_LOG_LINE_BUFF_SIZE] = { 0 };
		va_start(args, pLogText);
		vsnprintf(logText, EASY_LOG_LINE_BUFF_SIZE - 1, pLogText, args);
		WriteLog(logText, level);
	}

	void WriteLog(std::string logText, LOG_LEVEL level = LOG_ERROR)
	{
		static const char *const LOG_STRING[] =
		{
			"LOG_TRACE",
			"LOG_DEBUG",
			"LOG_INFO ",
			"LOG_WARN ",
			"LOG_ERROR",
			"LOG_ALARM",
			"LOG_FATAL",
		};


		// ����һ��LOG�ַ���
        std::stringstream szLogLine;
        szLogLine << "[" << DateStamp() <<"] [" << TimeStamp() << "] [" << LOG_STRING[level] << "] " << logText<<"\n";//�������Ҫ��ĳ�\r\n


#if defined EASY_LOG_DISABLE_LOG && EASY_LOG_DISABLE_LOG == 0
		/* ���LOG�ַ��� - �ļ��򿪲��ɹ�������°��ձ�׼��� */
		if (m_fileOut.is_open())
		{
		    m_fileOut.write(szLogLine.str().c_str(), szLogLine.str().size());
		    m_fileOut.flush();
		}
		else
		{
		    std::cout << szLogLine.str();
		}
#endif

#if defined EASY_LOG_PRINT_LOG && EASY_LOG_PRINT_LOG == 0
		std::cout << szLogLine.str();
#endif

	}

private:
    EasyLog(void)
	{
#if defined EASY_LOG_FILE_NAME_DATE && EASY_LOG_FILE_NAME_DATE == 1
#if defined EASY_LOG_COVER_LOG && EASY_LOG_COVER_LOG == 0
		m_fileOut.open((DateStamp()+".log").c_str(), std::ofstream::out);
#else
		m_fileOut.open((DateStamp()+".log").c_str(), std::ofstream::out | std::ofstream::app);
#endif
#else
#if defined EASY_LOG_COVER_LOG && EASY_LOG_COVER_LOG == 0
		m_fileOut.open(EASY_LOG_FILE_NAME, std::ofstream::out);
#else
		m_fileOut.open(EASY_LOG_FILE_NAME, std::ofstream::out | std::ofstream::app);
#endif
#endif

		WriteLog("------------------ LOG SYSTEM START ------------------\n", EasyLog::LOG_INFO);
	}
    virtual ~EasyLog(void)
	{
		WriteLog("------------------ LOG SYSTEM END ------------------ ", EasyLog::LOG_INFO);
		if (m_fileOut.is_open()) m_fileOut.close();
	}

private:
    /** д�ļ� */
    std::ofstream m_fileOut;
};


#endif /* EASYLOG_H_ */
