#pragma once

#include "basic_types.h"
#include "dal.h"
#include <mutex>

#define LOGGER_BUFFER_TEMP 512

namespace BayChat
{

/*
 * Log输出级别
 */
enum LogLevel
{
	LogLevel_verbose,
	LogLevel_trace,
	LogLevel_info,
	LogLevel_warning,
	LogLevel_error,
	LogLevel_max
};

/*
 * 输出log使用
 */
class Logger
{
public:
	static Logger* instance();

	/* 设定log输出级别 */
	void setLevel(LogLevel level);

	/* 文件路径 */
	void setFilePath(const WCHAR* path);

	/* 设定文件是否输出 */
	void setFileEnable(BOOL enable);

	/* 输出log */
	void print(const CHAR* file, uint32 lineNum, const CHAR* function, LogLevel level, const CHAR* content);

private:
	Logger();
	virtual ~Logger();

	Logger(const Logger&);
	Logger& operator=(const Logger&);

	const CHAR* trimPath(const char* path);

	std::mutex* m_mutex;

	LogLevel m_level;
	BOOL m_fileEnable;
	File m_file;
	WCHAR m_filePath[FILE_NAME_MAX_LENGTH];
};

#define LOGGER_VERBOSE_LOG(format,...) { \
		CHAR _temp_log_buffer[LOGGER_BUFFER_TEMP]; \
		sprintf_s(_temp_log_buffer, format, ##__VA_ARGS__); \
		BayChat::Logger::instance()->print(__FILE__, __LINE__, __FUNCTION__, BayChat::LogLevel_verbose, _temp_log_buffer); }

#define LOGGER_TRACE_LOG(format,...) { \
		CHAR _temp_log_buffer[LOGGER_BUFFER_TEMP]; \
		sprintf_s(_temp_log_buffer, format, ##__VA_ARGS__); \
		BayChat::Logger::instance()->print(__FILE__, __LINE__, __FUNCTION__, BayChat::LogLevel_trace, _temp_log_buffer); }

#define LOGGER_TRACE_LOG_START() { \
		BayChat::Logger::instance()->print(__FILE__, __LINE__, __FUNCTION__, BayChat::LogLevel_trace, "[START]"); }

#define LOGGER_TRACE_LOG_END() { \
		BayChat::Logger::instance()->print(__FILE__, __LINE__, __FUNCTION__, BayChat::LogLevel_trace, "[END]"); }

#define LOGGER_TRACE_LOG_STEP() { \
		BayChat::Logger::instance()->print(__FILE__, __LINE__, __FUNCTION__, BayChat::LogLevel_trace, "[STEP]"); }

#define LOGGER_INFO_LOG(format,...) { \
		CHAR _temp_log_buffer[LOGGER_BUFFER_TEMP]; \
		sprintf_s(_temp_log_buffer, format, ##__VA_ARGS__); \
		BayChat::Logger::instance()->print(__FILE__, __LINE__, __FUNCTION__, BayChat::LogLevel_info, _temp_log_buffer); }

#define LOGGER_WARNING_LOG(format,...) { \
		CHAR _temp_log_buffer[LOGGER_BUFFER_TEMP]; \
		sprintf_s(_temp_log_buffer, format, ##__VA_ARGS__); \
		BayChat::Logger::instance()->print(__FILE__, __LINE__, __FUNCTION__, BayChat::LogLevel_warning, _temp_log_buffer); }

#define LOGGER_ERROR_LOG(format,...) { \
		CHAR _temp_log_buffer[LOGGER_BUFFER_TEMP]; \
		sprintf_s(_temp_log_buffer, format, ##__VA_ARGS__); \
		BayChat::Logger::instance()->print(__FILE__, __LINE__, __FUNCTION__, BayChat::LogLevel_error, _temp_log_buffer); }

}