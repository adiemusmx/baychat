#pragma once

#include "basic_types.h"

namespace BayChat
{

/*
 * 多线程MutexMutex
 */
class Mutex
{
public:
	Mutex();
	virtual ~Mutex();

	/* 进行加锁和解锁操作 */
	virtual void lock();
	virtual void unlock();

private:
	Mutex(const Mutex& mutex);
	Mutex operator=(const Mutex& mutex);

	HANDLE m_handle;
};

/*
 * 文件读写接口封装
 */
enum FileMode
{
	FileMode_read,
	FileMode_readBinary,
	FileMode_readText,
	FileMode_readAndWrite,
	FileMode_readAndWriteBinary,
	FileMode_readAndWriteText,
	FileMode_readAndAppendOnly,
	FileMode_readAndAppendBinaryOnly,
	FileMode_readAndAppendTextOnly,
	FileMode_max
};

enum FileSeekMode
{
	FileSeekMode_set,
	FileSeekMode_cur,
	FileSeekMode_end,
	FileSeekMode_max
};

#define FILE_NAME_MAX_LENGTH 260

class File
{
public:
	File();
	virtual ~File();

	/* 打开/关闭文件 */
	BOOL open(const WCHAR* fileName, FileMode mode);
	BOOL open(const CHAR* fileName, FileMode mode);
	void close();

	/* 文件是否可用 */
	BOOL isValid();

	/* 文件长度 */
	size_t length();

	/* 文件读写接口 */
	size_t read(void* buffer, size_t sizeOfBufferElement, size_t countOfBufferElement);
	size_t write(const void* buffer, size_t sizeOfBufferElement, size_t countOfBufferElement);
	void seek(long offset, FileSeekMode mode);
	void rewind();

	/* 是否到文件结尾 */
	BOOL isEndOfFile();

	/* 立即将数据写入到文件 */
	void flush();

public:
	/* 文件是否存在 */
	static BOOL isExist(const WCHAR* fileName);
	static BOOL isExist(const CHAR* fileName);

	/* 重命名 */
	static BOOL rename(const WCHAR* from, const WCHAR* to);
	static BOOL rename(const CHAR* from, const CHAR* to);

	/* 删除文件 */
	static BOOL remove(const WCHAR* fileName);
	static BOOL remove(const CHAR* fileName);

private:
	WCHAR m_name[FILE_NAME_MAX_LENGTH];
	FILE* m_file;
	FileMode m_mode;
	size_t m_length;
};

}