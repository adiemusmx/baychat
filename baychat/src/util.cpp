#include "stdafx.h"
#include "util.h"

namespace BayChat
{

DateTime::DateTime() :
	m_year(0), m_month(0), m_day(0),
	m_hour(0), m_minute(0), m_second(0), m_millisecond(0)
{
}

DateTime::~DateTime()
{

}

DateTime::DateTime(const DateTime & datetime) :
	m_year(datetime.m_year), m_month(datetime.m_month), m_day(datetime.m_day),
	m_hour(datetime.m_hour), m_minute(datetime.m_minute), m_second(datetime.m_second), m_millisecond(datetime.m_millisecond)
{

}

DateTime& DateTime::operator=(const DateTime & datetime)
{
	m_year = datetime.m_year;
	m_month = datetime.m_month;
	m_day = datetime.m_day;
	m_hour = datetime.m_hour;
	m_minute = datetime.m_minute;
	m_second = datetime.m_second;
	m_millisecond = datetime.m_millisecond;
	return *this;
}

DateTime DateTime::getSystemTime()
{
	DateTime ret;
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	ret.m_year = sys.wYear;
	ret.m_month = sys.wMonth;
	ret.m_day = sys.wDay;
	ret.m_hour = sys.wHour;
	ret.m_minute = sys.wMinute;
	ret.m_second = sys.wSecond;
	ret.m_millisecond = sys.wMilliseconds;
	return ret;
}

DWORD ThreadHelper::getCurrentThreadId()
{
	return GetCurrentThreadId();
}

}