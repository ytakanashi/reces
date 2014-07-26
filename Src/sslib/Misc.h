//Misc.h

#ifndef _MISC_H_51E592F4_3486_4a21_BA64_02F08397C8CB
#define _MISC_H_51E592F4_3486_4a21_BA64_02F08397C8CB


namespace sslib{
namespace misc{

template<class T>
class NonCopyable{
protected:
	NonCopyable(){}
	~NonCopyable(){}
private:
	NonCopyable(const NonCopyable&);
	NonCopyable&operator=(const NonCopyable&);
};

//排他処理で活躍予定
class CriticalSection{
	CRITICAL_SECTION m_cs;
public:
	CriticalSection():m_cs(){::InitializeCriticalSection(&this->m_cs);}
	virtual ~CriticalSection(){::DeleteCriticalSection(&this->m_cs);}
	CRITICAL_SECTION* get()const{return (CRITICAL_SECTION*)&this->m_cs;}
};

class Lock:private NonCopyable<Lock>{
	const CriticalSection& m_cs;
public:
	Lock(const CriticalSection& cs):m_cs(cs){::EnterCriticalSection(m_cs.get());}
	virtual ~Lock(){::LeaveCriticalSection(m_cs.get());}
};

//イベントによる同期処理に
class Event:private NonCopyable<Event>{
	HANDLE m_event;
public:
	Event():m_event(NULL){
		m_event=::CreateEvent(NULL,true,false,NULL);
	}
	virtual ~Event(){::CloseHandle(m_event);}

	bool signal(){
		return ::SetEvent(m_event)!=0;
	}

	bool wait(DWORD milliseconds=INFINITE){
		return ::WaitForSingleObject(m_event,(milliseconds==0||milliseconds==INFINITE)?INFINITE:milliseconds)==WAIT_OBJECT_0;
	}
};

class CtrlCEvent:NonCopyable<CtrlCEvent>{
public:
	CtrlCEvent():m_ctrlc_event_list(){m_this_ptr=this;}
private:
	typedef std::list<CtrlCEvent*> ctrlc_event_list;
	ctrlc_event_list m_ctrlc_event_list;
private:
	static BOOL WINAPI handlerRoutine(DWORD ctrl_type);
public:
	static CtrlCEvent* m_this_ptr;
public:
	virtual void ctrlCEvent()=0;
	void addCtrlCEvent(CtrlCEvent* event);
};

namespace thread{
	struct INFO{
		HANDLE handle;
		unsigned int id;

		INFO():handle(NULL),id(0){}
		INFO(HANDLE h,unsigned int i):handle(h),id(i){}
	};

	struct PARAM{
		void* this_ptr;
		Event& ready;
		void* data;

		PARAM(void* this_ptr_,Event& ready_,void* data_=NULL):
			this_ptr(this_ptr_),ready(ready_),data(data_){}
	};

	HANDLE create(unsigned (__stdcall*func)(void*),unsigned*addr,void* this_ptr=NULL,void* data=NULL);
	bool post(DWORD id,UINT msg,void* this_ptr=NULL,void* data1=NULL,void* data2=NULL);
	bool close(INFO& info);
};

class StopWatch{
public:
	StopWatch():m_running(false),m_time(0),m_freq(),m_begin(){}
	virtual ~StopWatch(){}
public:
	bool start(){
		if(m_running)return false;
		m_running=true;

		m_freq.QuadPart=0;
		m_begin.QuadPart=0;

		::QueryPerformanceFrequency(&m_freq);
		::QueryPerformanceCounter(&m_begin);
		return true;
	}

	DWORD stop(){
		if(!m_running)return 0;
		m_running=false;

		LARGE_INTEGER end={};

		QueryPerformanceCounter(&end);
		m_time=static_cast<DWORD>((end.QuadPart-m_begin.QuadPart)*1000/m_freq.QuadPart);
		return m_time;
	}

	inline DWORD time()const{return m_time;}
private:
	bool m_running;
	DWORD m_time;
	LARGE_INTEGER m_freq,m_begin;
};


//namespace misc
}
//namespace sslib
}

#endif //_MISC_H_51E592F4_3486_4a21_BA64_02F08397C8CB
