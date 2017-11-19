#pragma once

#include "basic_types.h"

namespace BayChat
{


class DateTime
{
public:
	DateTime();
	virtual ~DateTime();
	DateTime(const DateTime& datetime);
	DateTime& operator=(const DateTime& datetime);

	static DateTime getSystemTime();

public:
	uint32 m_year;
	uint32 m_month;
	uint32 m_day;
	uint32 m_hour;
	uint32 m_minute;
	uint32 m_second;
	uint32 m_millisecond;
};

class ThreadHelper
{
public:
	static DWORD getCurrentThreadId();
};

}