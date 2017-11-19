#pragma once

#include "basic_types.h"

namespace BayChat
{

/*
 * ���߳�MutexMutex
 */
class Mutex
{
public:
	Mutex();
	virtual ~Mutex();

	/* ���м����ͽ������� */
	virtual void lock();
	virtual void unlock();

private:
	Mutex(const Mutex& mutex);
	Mutex operator=(const Mutex& mutex);

	HANDLE m_handle;
};

/*
 * �ļ���д�ӿڷ�װ
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

#define FILE_NAME_MAX_LENGTH 512

class File
{
public:
	File();
	virtual ~File();

	/* ��/�ر��ļ� */
	BOOL open(const WCHAR* fileName, FileMode mode);
	void close();

	/* �ļ��Ƿ���� */
	BOOL isValid();

	/* �ļ����� */
	size_t length();

	/* �ļ���д�ӿ� */
	size_t read(void* buffer, size_t sizeOfBufferElement, size_t countOfBufferElement);
	size_t write(const void* buffer, size_t sizeOfBufferElement, size_t countOfBufferElement);
	void seek(long offset, FileSeekMode mode);
	void rewind();

	/* �Ƿ��ļ���β */
	BOOL isEndOfFile();

	/* ����������д�뵽�ļ� */
	void flush();

public:
	/* �ļ��Ƿ���� */
	static BOOL isExist(const WCHAR* fileName);

private:
	WCHAR m_name[FILE_NAME_MAX_LENGTH];
	FILE* m_file;
	FileMode m_mode;
	size_t m_length;
};


}