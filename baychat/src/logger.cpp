#include "stdafx.h"
#include "logger.h"
#include "util.h"

#define VS_LOG_FORMAT "[%2d:%2d:%2d.%3d][%u][%s:%d|%s][%s]%s\n"
#define LOGGER_BUFFER_TOTAL 1024

namespace BayChat
{

Logger::Logger()
{
	m_mutex = new Mutex();
}


Logger::~Logger()
{
	if (m_mutex != NULL)
	{
		delete m_mutex;
		m_mutex = NULL;
	}
}

Logger* Logger::instance()
{
	static Logger ins;
	return &ins;
}

void Logger::print(CHAR* file, uint32 lineNum, CHAR* function, LogLevel level, CHAR* content)
{
	CHAR buffer[LOGGER_BUFFER_TOTAL] = { 0 };
	DateTime current = DateTime::getSystemTime();
	const CHAR* levelNameTbl[LogLevel_max] = { "VERBOSE", "TRACE", "INFO", "WARNING", "ERROR"};

	/* 计算缓存 */
	sprintf(buffer, VS_LOG_FORMAT, current.m_hour, current.m_minute, current.m_second, current.m_millisecond, ThreadHelper::getCurrentThreadId(), file, lineNum, function, levelNameTbl[level], content);

	/* 输出到控制台 */
	m_mutex->lock();
	printf(buffer);
	m_mutex->unlock();
}

}