#include "stdafx.h"
#include "logger.h"
#include "util.h"

#define VS_LOG_FORMAT "[%02d:%02d:%02d.%03d][%u][%s:%d|%s][%s]%s\n"
#define LOGGER_BUFFER_TOTAL 1024

namespace BayChat
{

Logger::Logger()
{
	m_mutex = new Mutex();
	m_level = LogLevel_info;
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

void Logger::setLevel(LogLevel level)
{
	m_level = level;
}

void Logger::print(CHAR* file, uint32 lineNum, CHAR* function, LogLevel level, CHAR* content)
{
	if (level < m_level)
		return;

	CHAR buffer[LOGGER_BUFFER_TOTAL] = { 0 };
	DateTime current = DateTime::getSystemTime();
	const CHAR* levelNameTbl[LogLevel_max] = { "VERBOSE", "TRACE", "INFO", "WARNING", "ERROR"};

	/* ���㻺�� */
	sprintf_s(buffer, VS_LOG_FORMAT, current.m_hour, current.m_minute, current.m_second, current.m_millisecond, ThreadHelper::getCurrentThreadId(), file, lineNum, function, levelNameTbl[level], content);

	/* ���������̨ */
	m_mutex->lock();
	printf(buffer);
	m_mutex->unlock();
}

}