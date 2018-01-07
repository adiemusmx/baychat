#include "stdafx.h"
#include "input_helper.h"


namespace BayChat
{
	struct KeyCodeMapInfo
	{
		int key;
		KeyCode code;
	};

	InputHelper* InputHelper::instance()
	{
		static InputHelper ins;
		return &ins;
	}

	void InputHelper::init()
	{
		LOGGER_TRACE_LOG_STEP();
		m_thread = new std::thread(runnableProc, NULL);
	}

	void InputHelper::cleanup()
	{
		LOGGER_TRACE_LOG_STEP();
		if (m_thread != NULL)
		{
			delete m_thread;
			m_thread = NULL;
		}
	}

	InputHelper::InputHelper()
	{
		LOGGER_TRACE_LOG_STEP();
		m_thread == NULL;
	}

	InputHelper::~InputHelper()
	{
		LOGGER_TRACE_LOG_STEP();
	}

	KeyCode InputHelper::getKey()
	{
		KeyCode ret = KeyCode_invalid;
		int mainKey = _getch();
		LOGGER_INFO_LOG("main code[%d|", mainKey);

		if (mainKey >= 'a' && mainKey <= 'z')
		{
			ret = (KeyCode)(KeyCode_a + mainKey - 'a');
		}
		else if (mainKey >= 'A' && mainKey <= 'Z')
		{
			ret = (KeyCode)(KeyCode_A + mainKey - 'A');
		}
		else if (mainKey >= '0' && mainKey <= '9')
		{
			ret = (KeyCode)(KeyCode_1 + mainKey - '1');
		}
		else if (mainKey == 0)
		{
			int subKey = _getch();
			LOGGER_INFO_LOG("sub code[%d|", subKey);
		}
		else if (mainKey == 224)
		{
			int subKey = _getch();
			LOGGER_INFO_LOG("sub code[%d|", subKey);
			switch (subKey)
			{
			case 82: ret = KeyCode_INSERT; break;
			case 83: ret = KeyCode_DELETE; break;
			case 71: ret = KeyCode_HOME; break;
			case 79: ret = KeyCode_END; break;
			case 73: ret = KeyCode_PAGEUP; break;
			case 81: ret = KeyCode_PAGEDOWN; break;
			default: printf("%d", subKey); break;
			}
		}
		else if (mainKey == 'r')
		{
			ret = KeyCode_ENTER;
		}
		else if (mainKey == 8)
		{
			ret = KeyCode_BACKSPACE;
		}
		else
		{
			printf("%d", mainKey);
		}
		return ret;
	}

	unsigned int InputHelper::runnableProc(void* data)
	{
		LOGGER_TRACE_LOG_STEP();
		do
		{
			instance()->getKey();
		} while (TRUE);
	}
}