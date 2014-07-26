//Misc.cpp
//その他諸々

#include"../StdAfx.h"
#include"sslib.h"


namespace sslib{
namespace misc{

CtrlCEvent* CtrlCEvent::m_this_ptr=NULL;

BOOL WINAPI CtrlCEvent::handlerRoutine(DWORD ctrl_type){
	CtrlCEvent* this_ptr=CtrlCEvent::m_this_ptr;

	for(ctrlc_event_list::iterator ite=this_ptr->m_ctrlc_event_list.begin(),
		end=this_ptr->m_ctrlc_event_list.end();
		ite!=end;
		++ite){
		(*ite)->ctrlCEvent();
	}
	return TRUE;
}

void CtrlCEvent::addCtrlCEvent(CtrlCEvent* event){
	if(m_ctrlc_event_list.empty()){
		::SetConsoleCtrlHandler(static_cast<PHANDLER_ROUTINE>(handlerRoutine),true);
	}
	m_ctrlc_event_list.push_back(event);
}

//汎用性に乏しいスレッド関係関数たち
namespace thread{
	HANDLE create(unsigned (__stdcall*func)(void*),unsigned*addr,void* this_ptr,void* data){
		Event ready;
		PARAM thread_param(this_ptr,ready,data);

		HANDLE handle=(HANDLE)_beginthreadex(NULL,0,func,reinterpret_cast<void*>(&thread_param),0,addr);

		//スレッド側はready.signal()で応えること
		ready.wait();
		return handle;
	}

	bool post(DWORD id,UINT msg,void* data1,void* data2,void* this_ptr){
		Event ready;
		PARAM thread_param(this_ptr,ready,data1);

		bool result=::PostThreadMessage(id,msg,(WPARAM)&thread_param,(LPARAM)data2)!=0;

		//スレッド側はready.signal()で応えること
		ready.wait();
		return result;
	}

	bool close(INFO& info){
		DWORD exit_code=0;

		::GetExitCodeThread(info.handle,&exit_code);
		if(exit_code==STILL_ACTIVE){
			::PostThreadMessage(info.id,WM_QUIT,0,0);
			::WaitForSingleObject(info.handle,INFINITE);
			return ::CloseHandle(info.handle)!=0;
		}
		return true;
	}
}//namespace thread

//namespace misc
}
//namespace sslib
}
