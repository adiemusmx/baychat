#ifndef _INPUT_HELPER_H_
#define _INPUT_HELPER_H_

#include <thread>

namespace BayChat {

	/*
		按键类型
	*/
	enum KeyCode
	{
		KeyCode_invalid,

		KeyCode_a, KeyCode_b, KeyCode_c, KeyCode_d, KeyCode_e,
		KeyCode_f, KeyCode_g, KeyCode_h, KeyCode_i, KeyCode_j,
		KeyCode_k, KeyCode_l, KeyCode_m, KeyCode_n, KeyCode_o,
		KeyCode_p, KeyCode_q, KeyCode_r, KeyCode_s, KeyCode_t,
		KeyCode_u, KeyCode_v, KeyCode_w, KeyCode_x, KeyCode_y,
		KeyCode_z,

		KeyCode_A, KeyCode_B, KeyCode_C, KeyCode_D, KeyCode_E,
		KeyCode_F, KeyCode_G, KeyCode_H, KeyCode_I, KeyCode_J,
		KeyCode_K, KeyCode_L, KeyCode_M, KeyCode_N, KeyCode_O,
		KeyCode_P, KeyCode_Q, KeyCode_R, KeyCode_S, KeyCode_T,
		KeyCode_U, KeyCode_V, KeyCode_W, KeyCode_X, KeyCode_Y,
		KeyCode_Z,

		KeyCode_0, KeyCode_1, KeyCode_2, KeyCode_3, KeyCode_4, 
		KeyCode_5, KeyCode_6, KeyCode_7, KeyCode_8, KeyCode_9,

		KeyCode_F1, KeyCode_F2, KeyCode_F3, KeyCode_F4, KeyCode_F5,
		KeyCode_F6, KeyCode_F7, KeyCode_F8, KeyCode_F9, KeyCode_F10,
		KeyCode_F11, KeyCode_F12,

		KeyCode_INSERT, KeyCode_DELETE, KeyCode_HOME, KeyCode_END, KeyCode_PAGEUP, KeyCode_PAGEDOWN,
		KeyCode_ESC, KeyCode_ENTER, KeyCode_BACKSPACE, KeyCode_SPACE, KeyCode_TAB,

		KeyCode_UP, KeyCode_DOWN, KeyCode_LEFT, KeyCode_RIGHT,
	};

	/* 
		接受用户输入 
	*/
	class InputHelper
	{
	public:
		static InputHelper* instance();

		/* 初始化 */
		void run();
		void stop();

	private:
		InputHelper();
		virtual ~InputHelper();

		InputHelper(const InputHelper&);
		InputHelper& operator=(const InputHelper&);

		KeyCode getKey();
		static unsigned int runnableProc(void* data);

		volatile BOOL m_exit;
	};

}

#endif // !_INPUT_HELPER_H_