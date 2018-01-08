#include "stdafx.h"
#include "dal.h"
#include <direct.h>

namespace BayChat
{

File::File()
{
	memset(m_name, 0x00, sizeof(m_name));
	m_mode = FileMode_max;
	m_file = NULL;
	m_length = 0;
}

File::~File()
{
	if (m_file != NULL)
	{
		fclose(m_file);
		m_file = NULL;
	}
}

BOOL File::open(const WCHAR* fileName, FileMode mode)
{
	CHAR tempFileName[FILE_NAME_MAX_LENGTH] = { 0 };
	sprintf_s(tempFileName, "%S", fileName);
	return File::open(tempFileName, mode);
}

BOOL File::open(const CHAR* fileName, FileMode mode)
{
	LOGGER_TRACE_LOG("name[%s] mode[%d]", fileName, mode);

	memset(m_name, 0x00, sizeof(m_name));
	swprintf_s(m_name, L"%S", fileName);

	m_mode = mode;

	const CHAR* fileModeTbl[FileMode_max] = { "r", "rb", "rt", "w+", "wb+", "wt+", "a+", "ab+", "at+" };

	errno_t error = fopen_s(&m_file, fileName, fileModeTbl[m_mode]);

	if (m_file != NULL)
	{
		fseek(m_file, 0, SEEK_END);
		m_length = ftell(m_file);
		fseek(m_file, 0, SEEK_SET);
	}
	else
	{
		LOGGER_ERROR_LOG("open filed. errno[%d]", error);
		m_length = 0;
	}
	return m_file != NULL;
}

void File::close()
{
	if (m_file != NULL)
	{
		fclose(m_file);
		m_file = NULL;
	}
}

BOOL File::isOpen()
{
	return m_file != NULL;
}

BOOL File::isExist(const WCHAR* fileName)
{
	CHAR tempFileName[FILE_NAME_MAX_LENGTH] = { 0 };
	sprintf_s(tempFileName, "%S", fileName);
	return _access(tempFileName, 0) == 0 ? TRUE : FALSE;
}

BOOL File::isExist(const CHAR* fileName)
{
	return _access(fileName, 0) == 0 ? TRUE : FALSE;
}

BOOL File::createDir(const WCHAR* fileName)
{
	CHAR tempFileName[FILE_NAME_MAX_LENGTH] = { 0 };
	sprintf_s(tempFileName, "%S", fileName);
	return _mkdir(tempFileName) == 0 ? TRUE : FALSE;
}

BOOL File::createDir(const CHAR* fileName)
{
	return _mkdir(fileName) == 0 ? TRUE : FALSE;
}

BOOL File::removeDir(const WCHAR* fileName)
{
	CHAR tempFileName[FILE_NAME_MAX_LENGTH] = { 0 };
	sprintf_s(tempFileName, "%S", fileName);
	return _rmdir(tempFileName) == 0 ? TRUE : FALSE;
}

BOOL File::removeDir(const CHAR* fileName)
{
	return _rmdir(fileName) == 0 ? TRUE : FALSE;
}

BOOL File::rename(const WCHAR* from, const WCHAR* to)
{
	CHAR tempFrom[FILE_NAME_MAX_LENGTH] = { 0 };
	sprintf_s(tempFrom, "%S", from);
	CHAR tempTo[FILE_NAME_MAX_LENGTH] = { 0 };
	sprintf_s(tempTo, "%S", to);
	return File::rename(tempFrom, tempTo);
}

BOOL File::rename(const CHAR* from, const CHAR* to)
{
	return ::rename(from, to) == 0 ? TRUE : FALSE;
}

BOOL File::remove(const WCHAR* fileName)
{
	CHAR tempFileName[FILE_NAME_MAX_LENGTH] = { 0 };
	sprintf_s(tempFileName, "%S", fileName);
	return File::remove(tempFileName);
}

BOOL File::remove(const CHAR* fileName)
{
	return ::remove(fileName) == 0 ? TRUE : FALSE;
}

size_t File::length()
{
	return m_length;
}

size_t File::read(void* buffer, size_t sizeOfBufferElement, size_t countOfBufferElement)
{
	size_t ret = 0;

	if (m_file != NULL)
	{
		ret = fread(buffer, sizeOfBufferElement, countOfBufferElement, m_file);
	}

	return ret;
}

size_t File::write(const void* buffer, size_t sizeOfBufferElement, size_t countOfBufferElement)
{
	size_t ret = 0;

	if (m_file != NULL)
	{
		ret = fwrite(buffer, sizeOfBufferElement, countOfBufferElement, m_file);
	}

	return ret;
}

void File::seek(long offset, FileSeekMode mode)
{
	uint32 seekFlagTbl[FileSeekMode_max] = { SEEK_SET, SEEK_CUR, SEEK_END };
	if (m_file != NULL)
	{
		fseek(m_file, offset, seekFlagTbl[mode]);
	}
}

void File::rewind()
{
	if (m_file != NULL)
	{
		::rewind(m_file);
	}
}

BOOL File::isEndOfFile()
{
	BOOL ret = TRUE;
	if (m_file != NULL)
	{
		ret = feof(m_file);
	}
	return ret;
}

void File::flush()
{
	if (m_file != NULL)
	{
		fflush(m_file);
	}
}

}