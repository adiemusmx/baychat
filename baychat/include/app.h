#ifndef _APP_H_
#define _APP_H_

namespace BayChat
{
	/*
		App��غ���
	*/
	class App
	{
	public:

		/* ����ģʽ */
		static App* instance();

		/* ��ʼ�� */
		void init();

		/* ���� */
		void clean();

		/* ���� */
		void run();

		/* �˳� */
		void exit();

	private:
		App();
		virtual ~App();

		App(const App&);
		App* operator=(const App&);
	};

}

#endif //!_APP_H_