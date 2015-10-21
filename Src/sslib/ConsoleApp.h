﻿//ConsoleApp.h

#ifndef _CONSOLEAPP_H_B13A77FD_E1E3_440f_A4EE_44A22D398E61
#define _CONSOLEAPP_H_B13A77FD_E1E3_440f_A4EE_44A22D398E61


extern int main();
#define STDOUT app()->stdOut()

namespace sslib{
class ConsoleApp:public misc::CtrlCEvent{
	friend bool isTerminated();
	friend void terminateApp(bool force);
	friend int ::main();

protected:
	ConsoleApp():
		m_stdout(),
		m_wnd_handle(::GetConsoleWindow()),
		m_exit_code(EXIT_SUCCESS),
		m_show_usage(true){
	}

	virtual ~ConsoleApp(){
	}

	Console m_stdout;
#ifndef _MSC_VER
public:
#endif
	static bool m_terminated;

private:
	HWND m_wnd_handle;
	int m_exit_code;
	bool m_show_usage;

protected:
	virtual bool init();
	virtual bool run(CommandArgument& cmd_arg)=0;
	virtual void cleanup();
	virtual void usage();
	virtual void ctrlCEvent();

public:
	inline HWND wnd()const{return m_wnd_handle;}
	inline sslib::Console& stdOut(){return m_stdout;}
	inline int getExitCode()const{return m_exit_code;}
	inline void setExitCode(int code){m_exit_code=code;}
	inline bool getUsageFlag()const{return m_show_usage;}
	inline void setUsageFlag(bool enable){m_show_usage=enable;}
	inline void ctrlC(){misc::CtrlCEventManager::generateCtrlCEvent();}
};

bool isTerminated();
void terminateApp(bool force=false);

#ifndef _MSC_VER
	#define IS_TERMINATED sslib::ConsoleApp::m_terminated
#endif

//namespace sslib
}

extern sslib::ConsoleApp* app();

#endif //_CONSOLEAPP_H_B13A77FD_E1E3_440f_A4EE_44A22D398E61
