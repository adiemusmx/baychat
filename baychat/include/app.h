#ifndef _APP_H_
#define _APP_H_

namespace BayChat
{
	/*
		App相关函数
	*/
	class App
	{
	public:

		/* 单例模式 */
		static App* instance();

		/* 初始化 */
		void init();

		/* 清理 */
		void clean();

		/* 运行 */
		void run();

		/* 退出 */
		void exit();

	private:
		App();
		virtual ~App();

		App(const App&);
		App* operator=(const App&);
	};

}

#endif //!_APP_H_