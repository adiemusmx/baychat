#include "stdafx.h"
#include "logger.h"
#include "util.h"

#define VS_LOG_FORMAT "[%02d:%02d:%02d.%03d][%u][%s:%d|%s][%s]%s\n"
#define LOGGER_BUFFER_TOTAL 1024

namespace BayChat
{

Logger::Logger()
{
	m_mutex = new std::mutex();
	m_level = LogLevel_info;
	m_fileEnable = FALSE;
	memset(m_filePath, 0x00, sizeof(m_filePath));
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

void Logger::setFilePath(const WCHAR* path)
{
	if (!File::isExist(path))
		File::createDir(path);
	wcscpy_s(m_filePath, path);
}

void Logger::setFileEnable(BOOL enable)
{
	if (m_fileEnable != enable)
		m_fileEnable = enable;

	if (m_filePath[0] == 0)
	{
		LOGGER_WARNING_LOG("No log file name. Set enable failed.");
		return;
	}

	if (m_fileEnable)
	{
		if (m_file.isOpen())
			m_file.close();
		WCHAR fileName[FILE_NAME_MAX_LENGTH] = { 0 };
		DateTime current = DateTime::getSystemTime();
		swprintf_s(fileName, L"%s/%04d%02d%02d_%02d02d02d.log", m_filePath, current.m_year, current.m_month, current.m_day, current.m_hour, current.m_minute, current.m_second);
		m_file.open(m_filePath, FileMode_readAndWriteText);
	}
	else
	{
		m_file.close();
	}
}

void Logger::print(const CHAR* file, uint32 lineNum, const CHAR* function, LogLevel level, const CHAR* content)
{
	if (level < m_level)
		return;

	AUTORELEASE_MUTEX_OBJECT(m_mutex);

	CHAR buffer[LOGGER_BUFFER_TOTAL] = { 0 };
	DateTime current = DateTime::getSystemTime();
	const CHAR* levelNameTbl[LogLevel_max] = { "VERBOSE", "TRACE", "INFO", "WARNING", "ERROR"};

	/* 计算缓存 */
	sprintf_s(buffer, VS_LOG_FORMAT, current.m_hour, current.m_minute, current.m_second, current.m_millisecond, ThreadHelper::getCurrentThreadId(), trimPath(file), lineNum, function, levelNameTbl[level], content);

	/* 输出到控制台 */
	printf(buffer);

	/* 文件输出 */
	if (m_fileEnable && m_file.isOpen())
	{
		m_file.write(buffer, sizeof(buffer), element_of(buffer));
	}
}

const CHAR* Logger::trimPath(const char* path)
{
	const CHAR* ret = "!UNKNOWN";
	if (path == NULL || *path == '\0')
		return ret;
	
	ret = path + strlen(path) - 1;
	while (*ret != '/' && *ret != '\\' && ret != path)
	{
		--ret;
	}
	return ret + 1;
}

}