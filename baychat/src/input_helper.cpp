#include "stdafx.h"
#include "input_helper.h"
#include "app.h"

namespace BayChat
{
	const char* g_keyCodeTable[] = 
	{
		"invalid",

		"a", "b", "c", "d", "e",
		"f", "g", "h", "i", "j",
		"k", "l", "m", "n", "o",
		"p", "q", "r", "s", "t",
		"u", "v", "w", "x", "y",
		"z",

		"A", "B", "C", "D", "E",
		"F", "G", "H", "I", "J",
		"K", "L", "M", "N", "O",
		"P", "Q", "R", "S", "T",
		"U", "V", "W", "X", "Y",
		"Z",

		"0", "1", "2", "3", "4",
		"5", "6", "7", "8", "9",

		"F1", "F2", "F3", "F4", "F5",
		"F6", "F7", "F8", "F9", "F10",
		"F11", "F12",

		"INSERT", "DELETE", "HOME", "END", "PAGEUP", "PAGEDOWN",
		"ESC", "ENTER", "BACKSPACE", "SPACE", "TAB",

		"UP", "DOWN", "LEFT", "RIGHT"
	};

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

	void InputHelper::run()
	{
		LOGGER_TRACE_LOG_STEP();
		m_exit = FALSE;
		std::thread m_thread(runnableProc, this);
		m_thread.detach();
	}

	void InputHelper::stop()
	{
		LOGGER_TRACE_LOG_STEP();
		m_exit = TRUE;
	}

	InputHelper::InputHelper()
	{
		LOGGER_TRACE_LOG_STEP();
	}

	InputHelper::~InputHelper()
	{
		LOGGER_TRACE_LOG_STEP();
	}

	KeyCode InputHelper::getKey()
	{
		KeyCode ret = KeyCode_invalid;
		int mainKey = _getch();

		if (mainKey >= 'a' && mainKey <= 'z') { ret = (KeyCode)(KeyCode_a + mainKey - 'a'); }
		else if (mainKey >= 'A' && mainKey <= 'Z') { ret = (KeyCode)(KeyCode_A + mainKey - 'A'); }
		else if (mainKey >= '0' && mainKey <= '9') { ret = (KeyCode)(KeyCode_1 + mainKey - '1'); }
		else if (mainKey == 13) { ret = KeyCode_ENTER; }
		else if (mainKey == 8) { ret = KeyCode_BACKSPACE; }
		else if (mainKey == 9) { ret = KeyCode_TAB; }
		else if (mainKey == 27) { ret = KeyCode_ESC; }
		else if (mainKey == 32) { ret = KeyCode_SPACE; }
		else if (mainKey == 0)
		{
			int subKey = _getch();

			if (subKey >= 59 && subKey <= 68) { ret = (KeyCode)(KeyCode_F1 + subKey - 59); }
			else { LOGGER_WARNING_LOG("unreco code[%d]", subKey); }
		}
		else if (mainKey == 224)
		{
			int subKey = _getch();

			switch (subKey)
			{
			case 82: ret = KeyCode_INSERT; break;
			case 83: ret = KeyCode_DELETE; break;
			case 71: ret = KeyCode_HOME; break;
			case 79: ret = KeyCode_END; break;
			case 73: ret = KeyCode_PAGEUP; break;
			case 81: ret = KeyCode_PAGEDOWN; break;
			case 133: ret = KeyCode_F11; break;
			case 134: ret = KeyCode_F12; break;
			case 72: ret = KeyCode_UP; break;
			case 80: ret = KeyCode_DOWN; break;
			case 75: ret = KeyCode_LEFT; break;
			case 77: ret = KeyCode_RIGHT; break;
			default: LOGGER_WARNING_LOG("unreco code[%d]", subKey); break;
			}
		}
		else { LOGGER_WARNING_LOG("unreco code[%d]", mainKey); }
		return ret;
	}

	unsigned int InputHelper::runnableProc(void* data)
	{
		LOGGER_TRACE_LOG("Thread Start.");
		do
		{
			KeyCode input = instance()->getKey();
			LOGGER_INFO_LOG("key stroke [%s]", g_keyCodeTable[input]);

			// 压下ESC按键，退出输入模块
			if (input == KeyCode_ESC)
				App::instance()->exit();

		} while (!instance()->m_exit);

		LOGGER_TRACE_LOG("Thread End.");
		return 0;
	}
}