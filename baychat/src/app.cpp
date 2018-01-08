#include "stdafx.h"
#include "app.h"
#include "input_helper.h"

namespace BayChat
{

	App* App::instance()
	{
		static App ins;
		return &ins;
	}

	void App::init()
	{
		LOGGER_TRACE_LOG_STEP();
	}

	void App::clean()
	{
		LOGGER_TRACE_LOG_STEP();

	}

	void App::run()
	{
		LOGGER_TRACE_LOG_STEP();
	}

	void App::exit()
	{
		LOGGER_TRACE_LOG_STEP();
		
		/* ÒÆ³ýÓÃ»§ÊäÈë */
		InputHelper::instance()->stop();
	}

	App::App()
	{

	}

	App::~App()
	{

	}

}